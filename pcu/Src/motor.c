#include "motor.h"

#include "bsp.h"
#include "debug.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "tim.h"

uint32_t motorARR;
HAL_StatusTypeDef motorInit() {
    motorARR = __HAL_TIM_GET_AUTORELOAD(&MOTOR_TIM_HANDLE);
    if (HAL_TIM_PWM_Start(&MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

float map(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

HAL_StatusTypeDef motorSetSpeed(float speed) {
    if (speed < 0 || speed > 100) {
        uprintf("Invalid speed: %f\n", speed);
        return HAL_ERROR;
    }
    float pulseWidth = map(speed, 0, 100, MIN_PULSE_WIDTH_US, MAX_PULSE_WIDTH_US);
    return motorSetPulseWidth(pulseWidth);
}

HAL_StatusTypeDef motorSetDutyCycle(float dutyCycle) {
    if (dutyCycle < 0 || dutyCycle > 100) {
        uprintf("Invalid duty cycle: %f\n", dutyCycle);
        return HAL_ERROR;
    }

    uint32_t CRR = dutyCycle * motorARR / 100.;
    __HAL_TIM_SET_COMPARE(&MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL, CRR);
    return HAL_OK;
}

HAL_StatusTypeDef motorSetPulseWidth(float pulseWidth){
    // if (pulseWidth < MIN_PULSE_WIDTH_US || pulseWidth > MAX_PULSE_WIDTH_US) {
    //     uprintf("Invalid pulse width: %f\n", pulseWidth);
    //     return HAL_ERROR;
    // }

    uint32_t CRR = pulseWidth / PPM_PERIOD_US * motorARR;
    __HAL_TIM_SET_COMPARE(&MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL, CRR);
    return HAL_OK;
}