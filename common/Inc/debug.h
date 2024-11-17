#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>

#include "FreeRTOS.h"
#include "main.h"
#include "queue.h"
#include "stm32f4xx_hal.h"

#define UART_PRINT_TIMEOUT 100
#define PRINT_QUEUE_LENGTH 10
#define PRINT_QUEUE_STRING_SIZE 100
#define PRINT_QUEUE_SEND_TIMEOUT_TICKS 10

extern QueueHandle_t printQueue;
extern char isDebugInitialized;
HAL_StatusTypeDef debugInit(void);

#define uprintf(...)                                                 \
    do {                                                             \
        if (!printQueue || !isDebugInitialized) {                    \
            Error_Handler();                                         \
        }                                                            \
        char _buf[PRINT_QUEUE_STRING_SIZE] = {0};                     \
        snprintf(_buf, PRINT_QUEUE_STRING_SIZE, __VA_ARGS__);         \
        xQueueSend(printQueue, _buf, PRINT_QUEUE_SEND_TIMEOUT_TICKS); \
    } while (0)

#define uprintfISR(...)                                      \
    do {                                                     \
        if (!printQueue || !isDebugInitialized) {            \
            Error_Handler();                                 \
        }                                                    \
        char _buf[PRINT_QUEUE_STRING_SIZE] = {0};             \
        snprintf(_buf, PRINT_QUEUE_STRING_SIZE, __VA_ARGS__); \
        xQueueSendFromISR(printQueue, _buf, NULL);            \
    } while (0)

void _handleError(char *file, int line);

#define handleError() _handleError(__FILE__, __LINE__)
#endif // __DEBUG_H__