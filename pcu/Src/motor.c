#include "motor.h"

#include "bsp.h"
#include "debug.h"
#include "main.h"
#include "mathUtils.h"
#include "pot.h"
#include "stm32f4xx_hal.h"
#include "tim.h"
#include "photo.h"

#define MOTOR_TASK_PERIOD_MS 100

#define MOTOR_MIN_PULSE_WIDTH_US 1075
#define MOTOR_MAX_PULSE_WIDTH_US 2200

#define MOTOR_LIMIT_PCT 30.

uint16_t PW_2_RPM_LUT[][2] = {
    {1565, 463},
    {1567, 500},
    {1570, 530},
    {1575, 639},
    {1580, 745},
    {1585, 850},
    {1590, 925},
    {1595, 1005},
    {1600, 1105},
    {1610, 1260},
    {1650, 1805},
    {1700, 2224},
    {1750, 2505},
    {1800, 2650},
    {1850, 2784},
    {1900, 2860},
    {1950, 2950},
    {2000, 3085}};

void motorStop() {
    motorSetPulseWidth(1000);
}

uint32_t motorARR;
HAL_StatusTypeDef motorInit() {
    motorARR = __HAL_TIM_GET_AUTORELOAD(&MOTOR_TIM_HANDLE) + 1;
    if (HAL_TIM_PWM_Start(&MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL) != HAL_OK) {
        return HAL_ERROR;
    }
    // motorStop();
    return HAL_OK;
}

HAL_StatusTypeDef motorSetSpeed(double speed) {
    if (speed < 0 || speed > 100) {
        uprintf("Invalid speed: %f\n", speed);
        return HAL_ERROR;
    }
    double pulseWidth = map(speed, 0, 100, MIN_PULSE_WIDTH_US, MAX_PULSE_WIDTH_US);
    return motorSetPulseWidth(pulseWidth);
}

HAL_StatusTypeDef motorSetDutyCycle(double dutyCycle) {
    if (dutyCycle < 0 || dutyCycle > 100) {
        uprintf("Invalid duty cycle: %f\n", dutyCycle);
        return HAL_ERROR;
    }

    uint32_t CRR = dutyCycle * motorARR / 100.;
    __HAL_TIM_SET_COMPARE(&MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL, CRR);
    return HAL_OK;
}

HAL_StatusTypeDef motorSetRPM(double requestedRPM) {
    if (requestedRPM < PW_2_RPM_LUT[0][1]) {
        uprintf("Invalid RPM: %f, min reqd: %u\n", requestedRPM, PW_2_RPM_LUT[0][1]);
        return HAL_ERROR;
    }
    // linear interpolate with LUT
    double pw = 0;
    for (int i = 0; i < sizeof(PW_2_RPM_LUT) / sizeof(PW_2_RPM_LUT[0]); i++) {
        if (requestedRPM < PW_2_RPM_LUT[i][1]) {
            pw = map(requestedRPM, PW_2_RPM_LUT[i - 1][1], PW_2_RPM_LUT[i][1], PW_2_RPM_LUT[i - 1][0], PW_2_RPM_LUT[i][0]);
            break;
        }
    }
    return motorSetPulseWidth(pw);
}

HAL_StatusTypeDef motorSetPulseWidth(double pulseWidth) {
    // if (pulseWidth < MIN_PULSE_WIDTH_US || pulseWidth > MAX_PULSE_WIDTH_US) {
    //     uprintf("Invalid pulse width: %f\n", pulseWidth);
    //     return HAL_ERROR;
    // }

    uint32_t CRR = pulseWidth * motorARR / PPM_PERIOD_US;
    __HAL_TIM_SET_COMPARE(&MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL, CRR);
    return HAL_OK;
}

void motorTask(void const* argument) {
    uprintf("Starting motorTask\n");
    TickType_t xLastWakeTime = xTaskGetTickCount();
    motorStop();
    vTaskDelay(pdMS_TO_TICKS(1000));
    uint32_t potValue;
    while ((potValue = getRawPotValue()) > 100) {
        uprintf("Reset pot to <100 raw, currently raw value: %ld\n", potValue);
        vTaskDelay(pdMS_TO_TICKS(350));
    }

    uint32_t UPPER_LIMIT = MOTOR_LIMIT_PCT * POT_MAX_VALUE / 100;
    motorStop();
    vTaskDelay(pdMS_TO_TICKS(1000));
    // vTaskDelay(pdMS_TO_TICKS(1000));
    // uint32_t start = HAL_GetTick();

    while (1) {
        potValue = getRawPotValue();
        if (potValue > UPPER_LIMIT) {
            uprintf("Pot value above limit [%.1f%% ~ %ld]: %ld, rpm: %d\n", MOTOR_LIMIT_PCT, UPPER_LIMIT, potValue, getRPM());
            vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(MOTOR_TASK_PERIOD_MS));
            continue;
        }
        double pulseWidth = map(potValue, POT_MIN_VALUE, POT_MAX_VALUE, MOTOR_MIN_PULSE_WIDTH_US, MOTOR_MAX_PULSE_WIDTH_US);

        motorSetPulseWidth(pulseWidth);
        // if (HAL_GetTick() - start > 1000) {
        uprintf("pot: %.2lf%%, mapped pw: %.3lf, rpm: %d\n", 100.*potValue/POT_MAX_VALUE, pulseWidth, getRPM());
        //     start = HAL_GetTick();
        // }
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(MOTOR_TASK_PERIOD_MS));
    }
}