#include "userInit.h"
#include "debug.h"
#include "delay_us.h"
#include "bsp.h"
#include "led.h"
#include "stm32f4xx_hal_flash.h"

HAL_StatusTypeDef ARTInit(){
    __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
    __HAL_FLASH_DATA_CACHE_ENABLE();
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
    return HAL_OK;
}
void userInit(void) {

    if (debugInit() != HAL_OK) {
        Error_Handler();
    }
    if (ARTInit() != HAL_OK) {
        Error_Handler();
    }

    if (delayInit() != HAL_OK) {
        Error_Handler();
    }
    
    if (ledInit() != HAL_OK) {
        Error_Handler();
    }
    // if (encodersInit() != HAL_OK) {
    //     Error_Handler();
    // }

    printf("----------------------------------\nFinished User Init\n");
    
}

// void vApplicationStackOverflowHook( TaskHandle_t xTask,
//                                     signed char *pcTaskName )
// {
//     _handleError(pcTaskName, __LINE__, __builtin_return_address(0));
// }