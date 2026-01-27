#include <stdio.h>
#include<stdlib.h>
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<freertos/queue.h>
#define QUEUE_LENGTH 5

QueueHandle_t queue;
TaskHandle_t producer_handle;

void produce_task(void*pvParameters) {
    int value=0;
    while(1) {
        value=rand()%100;
        if(xQueueSend(queue,&value,0)==pdPASS) {
            printf("producer sent: %d\n",value);
            value++;
        } else {
            printf("producer: queue full, suspending...\n");
            vTaskSuspend(NULL);
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void consumer_task(void*pvParameters) {
    int value;
    while(1) {
        if(xQueueReceive(queue,&value,portMAX_DELAY)==pdPASS) {
            printf("consumer received: %d -> %s\n",value,(value%2==0)?"even":"odd");
            if(producer_handle!=NULL) {
                vTaskResume(producer_handle);
            }
        }
    }
}

void app_main(void) {
    queue=xQueueCreate(QUEUE_LENGTH,sizeof(int));
    if(queue==NULL) {
        printf("failed to create queue\n");
        return;
    }
    xTaskCreate(produce_task,"Producer",2048,NULL,5,&producer_handle);
    xTaskCreate(consumer_task,"Consumer",2048,NULL,5,NULL);
}