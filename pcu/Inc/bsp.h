#ifndef __BSP_H__
#define __BSP_H__

#include "main.h"
#include "stm32f4xx_hal.h"
#include "tim.h"
#include "usart.h"

#define DEBUG_UART_HANDLE huart2
#define MOTOR_TIM_HANDLE htim4
#define MOTOR_TIM_CHANNEL TIM_CHANNEL_1
#define ENCODER_TIMER_HANDLE htim2
#define RPM_TIMER_HANDLE htim5
#endif  // __BSP_H__