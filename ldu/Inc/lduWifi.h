/*
    LDU WiFi Configuration
*/

#include "stdbool.h"
#include "esp_wifi.h"
#include "debug.h"
#include "bsp.h"
#include "usart.h"
#include "stdbool.h"

//-------- Customise these values -----------
const char* ldu_ssid = "LDU WIFI Point";
const char* ldu_password = "123456";

bool ldu_wifi_module_awake(void);
void ldu_wifi_init(void);