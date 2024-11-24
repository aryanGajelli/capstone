#include "motor.h"

#include "bsp.h"
#include "debug.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "tim.h"

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
    {2000, 3085}
};

uint32_t motorARR;
HAL_StatusTypeDef motorInit() {
    motorARR = __HAL_TIM_GET_AUTORELOAD(&MOTOR_TIM_HANDLE) + 1;
    if (HAL_TIM_PWM_Start(&MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

double map(double x, double in_min, double in_max, double out_min, double out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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
    if (requestedRPM < PW_2_RPM_LUT[0][1]){
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