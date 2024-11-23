#ifndef __ENCODERS_H__
#define __ENCODERS_H__

#include "stm32f4xx_hal.h"
#include "bsp.h"
#include <math.h>

#define TICKS_PER_REV (2048*4)


HAL_StatusTypeDef encodersInit(void);

#define encoderCount() ((int32_t)__HAL_TIM_GET_COUNTER(&ENCODER_TIMER_HANDLE))

#define encoderAngle(count) fmodf(count * 360. / TICKS_PER_REV, 360)

extern float rpm;

#endif  // __ENCODERS_H__