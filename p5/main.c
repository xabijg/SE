#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"

#define QUEUE_LENGTH    5
#define QUEUE_ITEM_SIZE sizeof(int32_t)

QueueHandle_t xQueue;

void vProducerTask(void *pvParameters) {
    TickType_t xLastWakeTime;
    const TickType_t xDelay = pdMS_TO_TICKS(500);
    int32_t i = 0;

    xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        xQueueSend(xQueue, &i, 0);
        i++;
        PRINTF("Productor: Enviado %d a la cola.\r\n", i);
        vTaskDelayUntil(&xLastWakeTime, xDelay);
    }
}

void vConsumerTask(void *pvParameters) {
    int32_t i;
    BaseType_t xStatus;

    for (;;) {
        xStatus = xQueueReceive(xQueue, &i, portMAX_DELAY);
        if (xStatus == pdPASS) {
            PRINTF("Consumidor: Recibido %d de la cola.\r\n", i);
        }

        // Actualiza la pantalla LCD
        static int32_t queue_count = 0;
        if (xStatus == pdPASS) {
            queue_count--;
        } else {
            queue_count++;
        }
        PRINTF("Cantidad de elementos en la cola: %d\r\n", queue_count);
    }
}

int main(void) {
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    xQueue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);

    if (xQueue != NULL) {
        xTaskCreate(vProducerTask, "Productor", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
        xTaskCreate(vConsumerTask, "Consumidor", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
        vTaskStartScheduler();
    } else {
        PRINTF("Fallo al crear la cola.\r\n");
    }

    for (;;);
    return 0;
}


