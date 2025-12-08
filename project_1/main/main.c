#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

int n=0;
void hello_task(void *pvParameter) {
    while (1) {
        printf("krishang is running rtos");
        vTaskDelay(pdMS_TO_TICKS(2000));
        printf(" (count = %d)\n", n++);
    }
}

void app_main(void) {
    xTaskCreate(hello_task, "hello_task", 2048, NULL, 5, NULL);
}