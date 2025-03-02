#include "photo.h"

#include "bsp.h"
#include "stm32f4xx_hal.h"
#include "tim.h"
#include "debug.h"

HAL_StatusTypeDef photoInit() {
    if (HAL_TIM_IC_Start_IT(&PHOTO_TIMER_HANDLE, PHOTO_CHANNEL_1) != HAL_OK) {
        return HAL_ERROR;
    }
    if (HAL_TIM_IC_Start_IT(&PHOTO_TIMER_HANDLE, PHOTO_CHANNEL_2) != HAL_OK) {
        return HAL_ERROR;
    }
    __HAL_TIM_ENABLE_IT(&PHOTO_TIMER_HANDLE, TIM_IT_UPDATE);
    return HAL_TIM_Base_Start_IT(&PHOTO_TIMER_HANDLE);
}


uint16_t getRPM() {
    uint32_t cc1 = __HAL_TIM_GET_COMPARE(&PHOTO_TIMER_HANDLE, PHOTO_CHANNEL_1);
    uint32_t cc2 = __HAL_TIM_GET_COMPARE(&PHOTO_TIMER_HANDLE, PHOTO_CHANNEL_2);

    const double F_CLK = HAL_RCC_GetSysClockFreq();
    double on_time = cc1 / F_CLK;
    double off_time = cc2 / F_CLK;
    double t = on_time+off_time;
    
    // uprintf("on_time: %.3f, off_time: %.3f, t: %.3f, RPM: %.3f\n", t1, t2, t, 60/t);
    return 60 / t;
}