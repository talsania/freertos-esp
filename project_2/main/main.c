#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#define LED_PIN 2

TaskHandle_t task_handle_blink=NULL;

void blink_task(void *pvParameters) {
    int n=0,x=0;
    while(1) {
        x=!x;
        gpio_set_level(LED_PIN,x);
        printf("blink task running #%d\n",n++);
        vTaskDelay(pdMS_TO_TICKS(500));
        if(n>10) {
            printf("blink task self-deleting\n");
            vTaskDelete(NULL);
        }
    }
}

void manager_task(void *pvParameters) {
    printf("manager task started, wait 5 seconds\n");
    vTaskDelay(pdMS_TO_TICKS(5000));
    if(task_handle_blink!=NULL) {
    printf("manager task deleting blink task\n");
    vTaskDelete(task_handle_blink);
    task_handle_blink=NULL;
    }
    vTaskDelete(NULL);
}

void app_main() {
    esp_rom_gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN,GPIO_MODE_OUTPUT);
    xTaskCreate(blink_task,"blink task",2048,NULL,5,&task_handle_blink);
    xTaskCreate(manager_task,"manager task",2048,NULL,4,NULL);
}