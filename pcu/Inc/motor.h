#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "stm32f4xx_hal.h"

#define PPM_FREQ_HZ 50
#define PPM_PERIOD_US (1000000/PPM_FREQ_HZ)
#define MIN_PULSE_WIDTH_US 1560.
#define MAX_PULSE_WIDTH_US 2350.

#define MIN_DUTY_CYCLE (MIN_PULSE_WIDTH_US/PPM_PERIOD_US)
#define MAX_DUTY_CYCLE (MAX_PULSE_WIDTH_US/PPM_PERIOD_US)

#define getCurrentPulseWidthUS() __HAL_TIM_GET_COMPARE(&MOTOR_TIM_HANDLE, MOTOR_TIM_CHANNEL)



HAL_StatusTypeDef motorInit(void);
HAL_StatusTypeDef motorSetSpeed(double speed);
HAL_StatusTypeDef motorSetRPM(double requestedRPM);
HAL_StatusTypeDef motorSetDutyCycle(double dutyCycle);
HAL_StatusTypeDef motorSetPulseWidth(double pulseWidth);

#endif // __MOTOR_H__