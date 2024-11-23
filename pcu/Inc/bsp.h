#ifndef __BSP_H__
#define __BSP_H__

#include "stm32f4xx_hal.h"
#include "main.h"
#include "tim.h"
#include "usart.h"

#define DEBUG_UART_HANDLE huart2
#define ENCODER_TIMER_HANDLE htim2
#define RPM_TIMER_HANDLE htim7

#endif // __BSP_H__