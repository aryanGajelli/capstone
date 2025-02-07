#include "pot.h"

#include "adc.h"
#include "debug.h"
#include "stm32f4xx_hal.h"

#define NUM_AVG_SAMPLES 100
uint32_t potValue[NUM_AVG_SAMPLES];

HAL_StatusTypeDef potInit() {
    // Start adc dma
    return HAL_ADC_Start_DMA(&hadc1, potValue, NUM_AVG_SAMPLES);
}

uint32_t getRawPotValue() {
    uint32_t sum = 0;
    for (int i = 0; i < NUM_AVG_SAMPLES; i++) {
        sum += potValue[i];
    }
    return sum / NUM_AVG_SAMPLES;
}
