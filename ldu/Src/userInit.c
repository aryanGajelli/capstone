#include "userInit.h"
#include "debug.h"
#include "delay_us.h"
#include "bsp.h"
#include "led.h"

void userInit(void) {
   
    if (debugInit() != HAL_OK) {
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