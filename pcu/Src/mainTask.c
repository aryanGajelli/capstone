#include "FreeRTOS.h"
#include "debug.h"
#include "main.h"
#include "task.h"
#include "motor.h"
#include "bsp.h"
#include "encoder.h"

#define MAIN_TASK_PERIOD_MS 10
void mainTask(void const* argument) {
    uprintf("Starting mainTask\n");
    // motorSetDutyCycle(7);
    motorSetPulseWidth(1500);
    vTaskDelay(pdMS_TO_TICKS(1000));
    motorSetSpeed(0);
    vTaskDelay(pdMS_TO_TICKS(1000));
    motorSetSpeed(50);
    vTaskDelay(pdMS_TO_TICKS(1000));
    motorSetSpeed(100);
    TickType_t xLastWakeTime = xTaskGetTickCount();


    while (1) {
        uprintf("RPM: %f\n", rpm);
        // HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
      

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(MAIN_TASK_PERIOD_MS));
    }
}