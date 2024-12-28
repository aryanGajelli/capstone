#include "delay_us.h"

#include "bsp.h"
#include "stm32f4xx_hal.h"

HAL_StatusTypeDef delayInit(void) {
    // Initialize the timer for microsecond delay
    return HAL_TIM_Base_Start(&US_DELAY_TIMER);
}

void delayUS(uint32_t us) {
    __HAL_TIM_SET_COUNTER(&US_DELAY_TIMER, 0);
    while (__HAL_TIM_GET_COUNTER(&US_DELAY_TIMER) < us);
}

