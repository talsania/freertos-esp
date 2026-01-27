#include <stdio.h>
#include<inttypes.h>
#include"freertos/FreeRTOS.h"
#include"freertos/task.h"
#include"freertos/queue.h"
#include"driver/gpio.h"
#include"esp_log.h"

#define BUTTON1_GPIO 0
#define BUTTON2_GPIO 4
#define TAG "krishang"

QueueHandle_t button_queue;

static void IRAM_ATTR button_isr_handler(void*arg) {
    uint32_t gpio_num = (uint32_t) arg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(button_queue,&gpio_num,&xHigherPriorityTaskWoken);
    if(xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}


void button_task(void*pvParameter) {
    uint32_t io_num;
    while(1) {
        if(xQueueReceive(button_queue,&io_num,portMAX_DELAY)) {
            if (io_num == BUTTON1_GPIO) {
                ESP_LOGI(TAG, "button 1 pressed (GPIO %" PRIu32 ")", io_num);
            }
            else if (io_num == BUTTON2_GPIO) {
                ESP_LOGI(TAG, "button 2 pressed (GPIO %" PRIu32 ")", io_num);
            }
            else {
                ESP_LOGW(TAG, "Unknown GPIO interrupt: %" PRIu32, io_num);
            }
            // debounce logic here
        }
    }
}

void app_main(void) {
    button_queue=xQueueCreate(10,sizeof(uint32_t));
    gpio_config_t io_conf={
        .intr_type=GPIO_INTR_NEGEDGE,
        .mode=GPIO_MODE_INPUT,
        .pin_bit_mask=(1ULL<<BUTTON1_GPIO)|(1ULL<<BUTTON2_GPIO),
        .pull_up_en=GPIO_PULLUP_ENABLE,
        .pull_down_en=GPIO_PULLDOWN_DISABLE
    };
    gpio_config(&io_conf);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON1_GPIO,button_isr_handler,(void*)BUTTON1_GPIO);
    gpio_isr_handler_add(BUTTON2_GPIO,button_isr_handler,(void*)BUTTON2_GPIO);
    xTaskCreate(button_task,"ButtonTask",2048,NULL,10,NULL);
}