#ifndef __PHOTO_H__
#define __PHOTO_H__

#include "stm32f4xx_hal.h"

HAL_StatusTypeDef photoInit();

double getRPM();

#endif // __PHOTO_H__