#ifndef __BSP_H__
#define __BSP_H__

#include "stm32f4xx_hal.h"
#include "main.h"
#include "usart.h"
#include "tim.h"

#define DEBUG_UART_HANDLE huart2
#define MOTOR_TIM_HANDLE htim2
#define MOTOR_TIM_CHANNEL TIM_CHANNEL_3
#endif // __BSP_H__