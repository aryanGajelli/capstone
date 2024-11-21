#include "lduWifi.h"

bool ldu_wifi_module_awake(void)
{   
    char* receive_buffer[100];
    HAL_UART_Transmit(&huart1, (uint8_t *)AT_COMMAND_AT, sizeof(AT_COMMAND_AT), 10);

    if (HAL_UART_Receive(&huart1, (uint8_t *)receive_buffer, sizeof(receive_buffer), 100) != HAL_OK)
    {
        return false;
    }
    else 
    {
        if (receive_buffer == AT_OK_STRING)
        {
            return true;
        }
    }
    return false;
}

void ldu_wifi_init(void)
{
   while(!ldu_wifi_module_awake()){}
   printf("LDU WiFi Module Awake\n");
}