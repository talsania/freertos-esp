#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define BUTTON_GPIO 0
#define TAG "krishang"

SemaphoreHandle_t button_semaphore;

static void IRAM_ATTR button_isr_handler(void*arg) {
    (void)arg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(button_semaphore,&xHigherPriorityTaskWoken);
    if(xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

void button_task1(void*pvParameter) {
    (void)pvParameter;
    while(1) {
        if(xSemaphoreTake(button_semaphore,portMAX_DELAY)) {
            ESP_LOGI(TAG,"button pressed!");
            // debounce/action/etc here
        }
    }
}

void button_task2(void*pvParameter) {
    (void)pvParameter;
    while(1) {
        if(xSemaphoreTake(button_semaphore,portMAX_DELAY)) {
            ESP_LOGI(TAG, "button pressed! (task2)");
            // debounce
        }
    }
}

void app_main(void) {
    button_semaphore=xSemaphoreCreateBinary();
    if(button_semaphore==NULL) {
        ESP_LOGI(TAG,"failed to create semaphore");
        return;
    }
    gpio_config_t io_conf={
        .intr_type=GPIO_INTR_NEGEDGE,
        .mode=GPIO_MODE_INPUT,
        .pin_bit_mask=(1ULL<<BUTTON_GPIO),
        .pull_down_en=GPIO_PULLDOWN_DISABLE,
        .pull_up_en=GPIO_PULLUP_ENABLE,
    };
    gpio_config(&io_conf);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_GPIO,button_isr_handler,NULL);
    xTaskCreate(button_task1,"ButtonTask1",2048,NULL,10,NULL);
    xTaskCreate(button_task2,"ButtonTask2",2048,NULL,10,NULL);
}