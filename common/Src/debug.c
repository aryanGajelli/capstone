#include "debug.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "bsp.h"
#include "main.h"
#include "stm32f4xx_hal.h"

// UART PRINT
void uprintf(const char *format, ...) {
    char buffer[DEBUG_PRINT_STRING_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    HAL_UART_Transmit(&DEBUG_UART_HANDLE, (uint8_t *)buffer, strnlen(buffer, DEBUG_PRINT_STRING_SIZE), UART_TIMEOUT_MS);
}