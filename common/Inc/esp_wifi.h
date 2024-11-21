#define AT_COMMAND_AT (uint8_t *)"AT\r\n"
#define AT_COMMAND_CWMODE_3 (uint8_t *)"AT+CWMODE=3\r\n" //Set the Wi-Fi mode to Station+SoftAP
#define AT_COMMAND_CWLAP (uint8_t *)"AT+CWLAP\r\n" //List Available APs
#define AT_OK_STRING "\r\nOK\r\n"
#define AT_OK_STRING_LEN sizeof(AT_OK_STRING)
#define AT_COMMAND_CIFSR (uint8_t *)"AT+CIFSR\r\n" //Get IP Address

