#include "encoder.h"

#include "bsp.h"
#include "debug.h"
#include "stm32f4xx_hal.h"
#include "tim.h"

HAL_StatusTypeDef encodersInit(void) {
    // reset encoder structs

    // enable the encoder interrupts to handle overflow
    // __HAL_TIM_ENABLE_IT(&ENCODER_TIMER_HANDLE, TIM_IT_UPDATE);
    


    if (HAL_TIM_Encoder_Start(&ENCODER_TIMER_HANDLE, TIM_CHANNEL_ALL) != HAL_OK) {
        return HAL_ERROR;
    }

    __HAL_TIM_ENABLE_IT(&RPM_TIMER_HANDLE, TIM_IT_UPDATE);
    return HAL_TIM_Base_Start(&RPM_TIMER_HANDLE);
}

// void encoderHandleOverflow() {

//     uint32_t counter = __HAL_TIM_GET_COUNTER(ENCODER_TIMER_HANDLE);
//     uint32_t cc2 = __HAL_TIM_GET_COMPARE(ENCODER_TIMER_HANDLE, TIM_CHANNEL_2);

//     encoder->overflow += (cc2 > counter) - (cc2 < counter);
// }