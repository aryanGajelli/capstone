#include "FreeRTOS.h"
#include "main.h"
#include "photo.h"
#include "task.h"

// #define RPM_DISPLAY_TASK_PERIOD_MS

uint8_t NUM_2_SEGMENT[10] = {
    0b00111111,  // 0
    0b00000110,  // 1
    0b01011011,  // 2
    0b01001111,  // 3
    0b01100110,  // 4
    0b01101101,  // 5
    0b01111101,  // 6
    0b00000111,  // 7
    0b01111111,  // 8
    0b01101111   // 9
};

void resetDisplay() {
    HAL_GPIO_WritePin(SEG_A_GPIO_Port, SEG_A_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_B_GPIO_Port, SEG_B_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_C_GPIO_Port, SEG_C_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_D_GPIO_Port, SEG_D_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_E_GPIO_Port, SEG_E_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_F_GPIO_Port, SEG_F_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_G_GPIO_Port, SEG_G_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_DIG0_GPIO_Port, SEG_DIG0_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_DIG1_GPIO_Port, SEG_DIG1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_DIG2_GPIO_Port, SEG_DIG2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SEG_DIG3_GPIO_Port, SEG_DIG3_Pin, GPIO_PIN_RESET);
}

void displayDigit(uint8_t digit, uint8_t place) {
    
    resetDisplay();
    HAL_GPIO_WritePin(SEG_DIG0_GPIO_Port, SEG_DIG0_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SEG_DIG1_GPIO_Port, SEG_DIG1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SEG_DIG2_GPIO_Port, SEG_DIG2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SEG_DIG3_GPIO_Port, SEG_DIG3_Pin, GPIO_PIN_SET);
    place = place % 4;
    uint8_t segment = NUM_2_SEGMENT[digit];

    HAL_GPIO_WritePin(SEG_A_GPIO_Port, SEG_A_Pin, segment & 0b00000001);
    HAL_GPIO_WritePin(SEG_B_GPIO_Port, SEG_B_Pin, segment & 0b00000010);
    HAL_GPIO_WritePin(SEG_C_GPIO_Port, SEG_C_Pin, segment & 0b00000100);
    HAL_GPIO_WritePin(SEG_D_GPIO_Port, SEG_D_Pin, segment & 0b00001000);
    HAL_GPIO_WritePin(SEG_E_GPIO_Port, SEG_E_Pin, segment & 0b00010000);
    HAL_GPIO_WritePin(SEG_F_GPIO_Port, SEG_F_Pin, segment & 0b00100000);
    HAL_GPIO_WritePin(SEG_G_GPIO_Port, SEG_G_Pin, segment & 0b01000000);

    switch (place) {
        case 0:
            HAL_GPIO_WritePin(SEG_DIG0_GPIO_Port, SEG_DIG0_Pin, GPIO_PIN_RESET);
            break;
        case 1:
            HAL_GPIO_WritePin(SEG_DIG1_GPIO_Port, SEG_DIG1_Pin, GPIO_PIN_RESET);
            break;
        case 2:
            HAL_GPIO_WritePin(SEG_DIG2_GPIO_Port, SEG_DIG2_Pin, GPIO_PIN_RESET);
            break;
        case 3:
            HAL_GPIO_WritePin(SEG_DIG3_GPIO_Port, SEG_DIG3_Pin, GPIO_PIN_RESET);
            break;
        default:
            break;
    }
}

void rpmDisplayTask(void *pvParameters) {
    // TickType_t xLastWakeTime = xTaskGetTickCount();
    uint16_t rpm = 1111;
    while (1) {
        rpm = (rpm + 1) % 2000;

        uint8_t rpm_1000 = rpm / 1000;
        uint8_t rpm_100 = (rpm % 1000) / 100;
        uint8_t rpm_10 = (rpm % 100) / 10;
        uint8_t rpm_1 = rpm % 10;

        
        if (rpm_1000 > 0) {
            displayDigit(rpm_1000, 3);
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        
        // resetDisplay();
        if (rpm_1000 > 0 || rpm_100 > 0) {
            displayDigit(rpm_100, 2);
            vTaskDelay(pdMS_TO_TICKS(3));
        }
        

        // resetDisplay();
        if (rpm_1000 > 0 || rpm_100 > 0 || rpm_10 > 0) {
            displayDigit(rpm_10, 1);
            vTaskDelay(pdMS_TO_TICKS(3));
        }
        

        // resetDisplay();
        displayDigit(rpm_1, 0);
        vTaskDelay(pdMS_TO_TICKS(3));
        // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(RPM_DISPLAY_TASK_PERIOD_MS));
    }
}