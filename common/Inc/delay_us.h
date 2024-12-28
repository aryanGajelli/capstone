#ifndef __DELAY_US_H__
#define __DELAY_US_H__

#include <stdint.h>
#include "stm32f4xx_hal.h"
HAL_StatusTypeDef delayInit(void);
void delayUS(uint32_t us);

#define delayUSWhile(us) \
    __HAL_TIM_SET_COUNTER(&US_DELAY_TIMER, 0); \
    while (__HAL_TIM_GET_COUNTER(&US_DELAY_TIMER) < us)

#endif  // __DELAY_US_H__