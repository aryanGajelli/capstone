#ifndef __ENCODERS_H__
#define __ENCODERS_H__

#include "stm32f4xx_hal.h"
#include "bsp.h"
#include <math.h>

#define QUAD_ENCODER 4
#define TICKS_PER_REV (2048*QUAD_ENCODER)
#define RPS_TO_RPM 60
#define SAMPLE_PERIOD_MS 100
#define SAMPLES_PER_SEC (1000 / SAMPLE_PERIOD_MS)



HAL_StatusTypeDef encodersInit(void);

#define encoderCount() ((int32_t)__HAL_TIM_GET_COUNTER(&ENCODER_TIMER_HANDLE))

#define encoderAngle(count) fmodf(count * 360. / TICKS_PER_REV, 360)

extern float rpm;

#endif  // __ENCODERS_H__