#include "photo.h"

#include "bsp.h"
#include "debug.h"
#include "stm32f4xx_hal.h"
#include "tim.h"


HAL_StatusTypeDef photoInit() {
    // if (HAL_TIM_IC_Start_IT(&PHOTO_TIMER_HANDLE, PHOTO_CHANNEL_1) != HAL_OK) {
    //     return HAL_ERROR;
    // }
    // if (HAL_TIM_IC_Start_IT(&PHOTO_TIMER_HANDLE, PHOTO_CHANNEL_2) != HAL_OK) {
    //     return HAL_ERROR;
    // }
    // __HAL_TIM_ENABLE_IT(&PHOTO_TIMER_HANDLE, TIM_IT_UPDATE);
    // return HAL_TIM_Base_Start_IT(&PHOTO_TIMER_HANDLE);
    __HAL_TIM_ENABLE_IT(&RPM_TIMER_HANDLE, TIM_IT_UPDATE);
    return HAL_TIM_Base_Start(&RPM_TIMER_HANDLE);
}

uint32_t currTime = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == PHOTO_IN_Pin) {
        // uprintf("Photo interrupt\n");
        currTime = __HAL_TIM_GET_COUNTER(&RPM_TIMER_HANDLE);
        __HAL_TIM_SET_COUNTER(&RPM_TIMER_HANDLE, 0);
    }
}

uint16_t getRPM() {
    // uprintf("currTime: %d\n", currTime);
    double t = currTime / 1e6;
    return 60/t;
}