#include "FreeRTOS.h"
#include "task.h"

#include "main.h"

#define MAIN_TASK_PERIOD_MS 500
void mainTask(void const * argument) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while(1) {
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(MAIN_TASK_PERIOD_MS));
    }
}