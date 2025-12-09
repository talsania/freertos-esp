#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#define LED_PIN 2

void sensor_task(void*pvParameter) {
    TickType_t last_wake=xTaskGetTickCount();
    while(1) {
        printf("sensor sample: %lums\n",(unsigned long)(xTaskGetTickCount()*portTICK_PERIOD_MS));
        vTaskDelayUntil(&last_wake,pdMS_TO_TICKS(200));
    }
}

void blink_task(void*pvParameter) {
    int x=0;
    gpio_set_direction(LED_PIN,GPIO_MODE_OUTPUT);
    while(1) {
        x=!x;
        gpio_set_level(LED_PIN,x);
        printf("LED toggle: %lums\n",(unsigned long)(xTaskGetTickCount()*portTICK_PERIOD_MS));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void) {
    xTaskCreate(sensor_task,"sensor task",2048,NULL,5,NULL);
    xTaskCreate(blink_task,"led blink",2048,NULL,5,NULL);
}