#ifndef __DELAY_US_H__
#define __DELAY_US_H__

#include <stdint.h>
#include "stm32f4xx_hal.h"
HAL_StatusTypeDef delayInit(void);
void delayUS(uint32_t us);

#endif  // __DELAY_US_H__