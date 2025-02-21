#ifndef __POT_H__
#define __POT_H__

#include "stm32f4xx_hal.h"
#define POT_MAX_VALUE 4094
#define POT_MIN_VALUE 1

HAL_StatusTypeDef potInit();

uint32_t getRawPotValue();
#endif  // __POT_H__