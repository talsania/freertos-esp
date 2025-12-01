#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void hello_task(void *pvParameter) {
    while (1) {
        printf("krishang is running rtos\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void) {
    xTaskCreate(hello_task, "hello_task", 2048, NULL, 5, NULL);
}