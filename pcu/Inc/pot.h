#ifndef __POT_H__
#define __POT_H__

#include "stm32f4xx_hal.h"

HAL_StatusTypeDef potInit();

uint32_t getRawPotValue();
#endif  // __POT_H__