#include "FreeRTOS.h"
#include "bsp.h"
#include "debug.h"
#include "encoder.h"
#include "main.h"
#include "mathUtils.h"
#include "motor.h"
#include "photo.h"
#include "pot.h"
#include "task.h"

#define MAIN_TASK_PERIOD_MS 100
void mainTask(void const* argument) {
    uprintf("Starting mainTask\n");
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1) {
        getRPM();
        // uprintf("RPM: %f\n", getRPM());
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(MAIN_TASK_PERIOD_MS));
    }
}