#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#ifndef LED1_GPIO
#define LED1_GPIO GPIO_NUM_2
#endif

#ifndef LED2_GPIO
#define LED2_GPIO GPIO_NUM_4
#endif

static const char *TAG = "main";

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

static inline void toggle_led(gpio_num_t pin)
{
    int current = gpio_get_level(pin);
    gpio_set_level(pin, !current);
}

static void taskA_delay(void *pv)
{
    const TickType_t period_ticks = pdMS_TO_TICKS(500);
    uint32_t iteration = 0;

    ESP_LOGI(TAG, "taskA started on core %d", xPortGetCoreID());

    while (1) {
        toggle_led(LED1_GPIO);
        iteration++;

        TickType_t now_ticks = xTaskGetTickCount();
        uint32_t now_ms = now_ticks * portTICK_PERIOD_MS;
        ESP_LOGI(TAG, "[A] t=%" PRIu32 " ms, iter=%" PRIu32, now_ms, iteration);

        vTaskDelay(period_ticks);
    }
}

static void taskB_delay_until(void *pv)
{
    const TickType_t period_ticks = pdMS_TO_TICKS(1000);
    TickType_t last_wake = xTaskGetTickCount();
    uint32_t iteration = 0;

    ESP_LOGI(TAG, "taskB started on core %d", xPortGetCoreID());

    while (1) {
        toggle_led(LED2_GPIO);
        iteration++;

        TickType_t now_ticks = xTaskGetTickCount();
        uint32_t now_ms = now_ticks * portTICK_PERIOD_MS;
        ESP_LOGI(TAG, "[B] t=%" PRIu32 " ms, iter=%" PRIu32, now_ms, iteration);

        vTaskDelayUntil(&last_wake, period_ticks);
    }
}

static void task_status(void *pv)
{
    const TickType_t period_ticks = pdMS_TO_TICKS(2000); // every 2 s
    TickType_t last_wake = xTaskGetTickCount();
    uint32_t seconds = 0;

    ESP_LOGI(TAG, "taskStatus started on core %d", xPortGetCoreID());

    while (1) {
        vTaskDelayUntil(&last_wake, period_ticks);
        seconds += 2;

        TickType_t ticks = xTaskGetTickCount();
        uint32_t ms = ticks * portTICK_PERIOD_MS;
        ESP_LOGI(TAG, "[STATUS] uptime ~%" PRIu32 " ms (%" PRIu32 "s)", ms, seconds);
    }
}

void app_main(void)
{
    configure_led(LED1_GPIO);
    configure_led(LED2_GPIO);

    BaseType_t okA = xTaskCreate(taskA_delay, "TaskA_Delay", 2048, NULL, 5, NULL);
    BaseType_t okB = xTaskCreate(taskB_delay_until, "TaskB_DelayUntil", 2048, NULL, 5, NULL);
    xTaskCreate(task_status, "TaskStatus", 2048, NULL, 3, NULL);

    if (okA != pdPASS || okB != pdPASS) {
        ESP_LOGE(TAG, "Failed to create one or more tasks");
    }
}