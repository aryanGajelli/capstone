#include "debug.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "bsp.h"
#include "main.h"
#include "stm32f4xx_hal.h"

char buffer[DEBUG_PRINT_STRING_SIZE];
// UART PRINT
void uprintf(const char *format, ...) {
    buffer[DEBUG_PRINT_STRING_SIZE - 1] = '\0';
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    size_t len = strlen(buffer);
    HAL_UART_Transmit_DMA(&DEBUG_UART_HANDLE, (uint8_t *)buffer, len);
}