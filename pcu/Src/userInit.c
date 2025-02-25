#include "userInit.h"

#include "bsp.h"
#include "debug.h"
#include "encoder.h"
#include "motor.h"
#include "pot.h"
#include "photo.h"

void userInit(void) {
    if (debugInit() != HAL_OK) {
        handleError();
    }

    if (motorInit() != HAL_OK) {
        Error_Handler();
    }

    // if (encodersInit() != HAL_OK) {
    //     Error_Handler();
    // }

    if (potInit() != HAL_OK) {
        Error_Handler();
    }

    if (photoInit() != HAL_OK) {
        Error_Handler();
    }

    printf("----------------------------------\nFinished User Init\n");
}

// void vApplicationStackOverflowHook( TaskHandle_t xTask,
//                                     signed char *pcTaskName )
// {
//     _handleError(pcTaskName, __LINE__, __builtin_return_address(0));
// }