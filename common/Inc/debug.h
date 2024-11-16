#ifndef __DEBUG_H__
#define __DEBUG_H__


#define DEBUG_PRINT_STRING_SIZE 256
#define UART_TIMEOUT_MS 1000

void uprintf(const char *format, ...);
#endif // __DEBUG_H__