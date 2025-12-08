#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void task_low(void *pvParameters) {
    while(1) {
        printf("low priority task running on core %d\n",xPortGetCoreID());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task_mid(void *pvParameters) {
    while(1) {
        printf("mid priority task running on core %d\n",xPortGetCoreID());
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void task_high(void *pvParameters) {
    int n=0;
    while(1) {
        printf("high priority task running on core %d\n",xPortGetCoreID());
        n++;
        if(n>5) {
            vTaskPrioritySet(NULL,0);
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void app_main(void) {
    xTaskCreate(task_low,"low priority",2048, NULL, 3, NULL);
    xTaskCreate(task_mid,"mid priority",2048,NULL,5,NULL);
    xTaskCreate(task_high,"high priority",2048,NULL,8,NULL);
}