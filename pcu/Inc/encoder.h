#ifndef __ENCODERS_H__
#define __ENCODERS_H__

#include "stm32f4xx_hal.h"
#include "bsp.h"

#define TICKS_PER_REV 2048


HAL_StatusTypeDef encodersInit(void);

#define encoderCount() __HAL_TIM_GET_COUNTER(&ENCODER_TIMER_HANDLE)

#endif  // __ENCODERS_H__