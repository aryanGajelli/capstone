#include "FreeRTOS.h"
#include "debug.h"
#include "main.h"
#include "task.h"
#include "usart.h"
#include "cmsis_os.h"
#include "dma.h"
#include "gpio.h"

// char rx_byte[1];
char buffer[128]; // Fixed buffer size for now

// Doesn't work for now
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
//     if (huart->Instance == USART1) {
//         HAL_UART_Receive_IT(&huart1,(uint8_t *)rx_byte, 10);
//         uprintfISR(rx_byte, "Received: %s\n");
//         uprintfISR("HAL_UART_RxCpltCallback\n");
//     }
//     uprintfISR("HAL_UART_RxCpltCallback\n");
// }


#define MAIN_TASK_PERIOD_MS 500
void mainTask(void const* argument) {
    uprintf("Starting mainTask\n");
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1) {
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(MAIN_TASK_PERIOD_MS));

        // char send_data[] = "AT\r\n";
        // // Check AT status
        // HAL_UART_Transmit(&huart1, (uint8_t *)send_data, sizeof(send_data), 10);
        // uprintf("Sent: %s\n", send_data);
        // HAL_UART_Receive(&huart1, (uint8_t *)buffer, sizeof(buffer), 1000);
        // uprintf("Received: %s\n", buffer);
        // HAL_Delay(100);
        
    }
}