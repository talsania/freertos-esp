#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_log.h"

#ifndef LED1_GPIO
#define LED1_GPIO GPIO_NUM_2
#endif

#ifndef LED2_GPIO
#define LED2_GPIO GPIO_NUM_4
#endif

#ifndef BUTTON_GPIO
#define BUTTON_GPIO GPIO_NUM_0 // BOOT button
#endif

static const char *TAG = "main";
static TaskHandle_t taskA_handle = NULL;
static bool taskA_suspended = false;
static SemaphoreHandle_t log_mutex = NULL;

static inline void configure_led(gpio_num_t pin)
{
    gpio_config_t io = {
        .pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io);
    gpio_set_level(pin, 0);
}

static inline void configure_button(gpio_num_t pin)
{
    gpio_config_t io = {
        .pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE // interrupt on falling edge
    };
    gpio_config(&io);
    gpio_install_isr_service(0);
}

static inline void toggle_led(gpio_num_t pin)
{
    int current = gpio_get_level(pin);
    gpio_set_level(pin, !current);
}

static void IRAM_ATTR button_isr_handler(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    if (taskA_handle != NULL) {
        if (taskA_suspended) {
            vTaskResume(taskA_handle);
            taskA_suspended = false;
        } else {
            vTaskSuspend(taskA_handle);
            taskA_suspended = true;
        }
    }
    
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void taskA_delay(void *pv)
{
    const TickType_t period_ticks = pdMS_TO_TICKS(500);
    uint32_t iteration = 0;

    if (xSemaphoreTake(log_mutex, portMAX_DELAY) == pdTRUE) {
        ESP_LOGI(TAG, "taskA started on core %d", xPortGetCoreID());
        xSemaphoreGive(log_mutex);
    }

    while (1) {
        toggle_led(LED1_GPIO);
        iteration++;

        // busy loop
        TickType_t busy_start = xTaskGetTickCount();
        while ((xTaskGetTickCount() - busy_start) < pdMS_TO_TICKS(7)) {
        }

        TickType_t now_ticks = xTaskGetTickCount();
        uint32_t now_ms = now_ticks * portTICK_PERIOD_MS;
        
        if (xSemaphoreTake(log_mutex, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(TAG, "[A] t=%" PRIu32 " ms, iter=%" PRIu32, now_ms, iteration);
            xSemaphoreGive(log_mutex);
        }

        vTaskDelay(period_ticks);
    }
}

static void taskB_delay_until(void *pv)
{
    const TickType_t period_ticks = pdMS_TO_TICKS(1000);
    TickType_t last_wake = xTaskGetTickCount();
    uint32_t iteration = 0;

    if (xSemaphoreTake(log_mutex, portMAX_DELAY) == pdTRUE) {
        ESP_LOGI(TAG, "taskB started on core %d", xPortGetCoreID());
        xSemaphoreGive(log_mutex);
    }

    while (1) {
        toggle_led(LED2_GPIO);
        iteration++;

        TickType_t now_ticks = xTaskGetTickCount();
        uint32_t now_ms = now_ticks * portTICK_PERIOD_MS;
        
        if (xSemaphoreTake(log_mutex, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(TAG, "[B] t=%" PRIu32 " ms, iter=%" PRIu32, now_ms, iteration);
            xSemaphoreGive(log_mutex);
        }

        vTaskDelayUntil(&last_wake, period_ticks);
    }
}

static void task_status(void *pv)
{
    const TickType_t period_ticks = pdMS_TO_TICKS(2000);
    TickType_t last_wake = xTaskGetTickCount();
    uint32_t seconds = 0;

    if (xSemaphoreTake(log_mutex, portMAX_DELAY) == pdTRUE) {
        ESP_LOGI(TAG, "taskStatus started on core %d", xPortGetCoreID());
        xSemaphoreGive(log_mutex);
    }

    while (1) {
        vTaskDelayUntil(&last_wake, period_ticks);
        seconds += 2;

        TickType_t ticks = xTaskGetTickCount();
        uint32_t ms = ticks * portTICK_PERIOD_MS;
        
        if (xSemaphoreTake(log_mutex, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(TAG, "[STATUS] uptime ~%" PRIu32 " ms (%" PRIu32 "s), TaskA: %s", 
                     ms, seconds, taskA_suspended ? "SUSPENDED" : "RUNNING");
            xSemaphoreGive(log_mutex);
        }
    }
}

void app_main(void)
{
    // create mutex before any tasks
    log_mutex = xSemaphoreCreateMutex();
    if (log_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create log mutex");
        return;
    }

    configure_led(LED1_GPIO);
    configure_led(LED2_GPIO);
    configure_button(BUTTON_GPIO);
    BaseType_t okA = xTaskCreatePinnedToCore(taskA_delay, "TaskA_Delay", 2048, NULL, 5, &taskA_handle, 0);
    BaseType_t okB = xTaskCreatePinnedToCore(taskB_delay_until, "TaskB_DelayUntil", 2048, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(task_status, "TaskStatus", 2048, NULL, 3, NULL, 0);
    if (okA != pdPASS || okB != pdPASS) {
        ESP_LOGE(TAG, "failed to create one or more tasks");
    }
    gpio_isr_handler_add(BUTTON_GPIO, button_isr_handler, NULL); // attach interrupt handler to button
    ESP_LOGI(TAG, "Press button on GPIO%d to suspend/resume TaskA", BUTTON_GPIO);
}