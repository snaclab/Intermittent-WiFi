
#include "ESP8266.h"
#include "Tools/myuart.h"

// TODO: need to check reset pin for msp430 + esp8266-01
#define RESET_PIN       GPIO_PIN1
#define RESET_PORT      GPIO_PORT_P6

#define AT              "AT\r\n"  // Test AT starup
#define AT_RST          "AT+RST\r\n" // Restarts the module (soft reset)
#define AT_GMR          "AT+GMR\r\n" // Check version information
#define AT_GSLP         "AT+GSLP\r\n" // Enter Deep-sleep mode
#define ATE             "ATE\r\n" // Configures echoing of AT commands
#define AT_CWMODE       "AT+CWMODE" // set WiFi mode (STA/AP/STA+AP)
#define AT_CWMODE_Q     "AT+CWMODE?\r\n" // get current WiFi mode
#define AT_CWJAP        "AT+CWJAP" // Connect to AP
#define AT_CWLAP        "AT+CWLAP\r\n" // List Available APs
#define AT_CWQAP        "AT+CWQAP\r\n" // Disconnects from AP
#define AT_CWLIF        "AT+CWLIF\r\n" // get info SoftAP station from connected ESP8266
#define AT_CWDHCP       "AT+CWDHCP\r\n" // set DHCP
#define AT_CIPSTAMAC    "AT+CIPSTAMAC\r\n" // set station MAC address
#define AT_CIPAPMAC     "AT+CIPAPMAC\r\n" // set AP MAC address
#define AT_CIPSTA       "AT+CIPSTA\r\n" // set STA IP address
#define AT_CIPAP        "AT+CIPAP" // set AP IP address
#define AT_CIPSTATUS    "AT+CIPSTATUS\r\n" // check connection status
#define AT_CIPSTART     "AT+CIPSTART" // establish TCP/UDP/SSL connection
#define AT_CIPSEND      "AT+CIPSEND" // send data
#define AT_CIPCLOSE     "AT+CIPCLOSE\r\n" // close connection
#define AT_CIFSR        "AT+CIFSR\r\n" // look for local IP address
#define AT_CIPMUX       "AT+CIPMUX" // set multiple conneciton
#define AT_CIPSERVER    "AT+CIPSTO\r\n" // establish TCP server
#define AT_CIPMODE      "AT+CIPMODE\r\n" // set connection mode
#define AT_CIPSTO       "AT+CIPSTO\r\n" // set TCP server timeout 
#define AT_CIUPDATE     "AT+CIUPDATE\r\n" // update software through Wi-Fi
#define IPD             "+IPD\r\n" 

char ESP8266_Buffer[ESP8266_BUFFER_SIZE] = "";

bool waitForResponse(unsigned int Tries)
{
    unsigned char c;
    unsigned int i = 0;

    while(Tries) {
        if (uartA3GotMessage()) {
            while (uartA3GotMessage()) {
                c = readFromUartA3();

                if (i > ESP8266_BUFFER_SIZE) {
                    return false;
                } else {
                    ESP8266_Buffer[i++] = c;
                }
            }

            ESP8266_Buffer[i++] = '\0';
            return true;
        }

        Tries--;
        __delay_cycles(2400000);
    }

    return false;
}

bool commandSuccess()
{
    if (!waitForResponse(ESP8266_RECEIVE_TRIES)) {
        return false;
    }
    if (strstr(ESP8266_Buffer, "OK") == NULL) {
        return false;
    }
    return true;
}

bool ESP8266_getCurrentWiFiMode(void)
{
    print2uart(UART_ESP, AT_CWMODE_Q);
    __delay_cycles(12000000);

    return commandSuccess();
}

bool ESP8266_changeWiFiMode(unsigned int MODE)
{
    print2uart(UART_ESP, "%s=%d\r\n", AT_CWMODE, MODE);
    __delay_cycles(24000000);

    return commandSuccess();
}

bool ESP8266_checkConnection(void)
{
    print2uart(UART_ESP, AT);
    __delay_cycles(12000000);

    return commandSuccess();
}

bool ESP8266_getSystemInfo(void)
{
    print2uart(UART_ESP, AT_GMR);
    __delay_cycles(12000000);

    return commandSuccess();
}

bool ESP8266_availableAPs(void)
{
    print2uart(UART_ESP, AT_CWLAP);
    __delay_cycles(48000000);

    return commandSuccess();
}

bool ESP8266_connectToAP(char *SSID, char *password)
{
    print2uart(UART_ESP, "%s=\"%s\",\"%s\"\r\n", AT_CWJAP, SSID, password);

    __delay_cycles(72000000);
    
    return commandSuccess();
}

bool ESP8266_disconnectFromAP(void)
{
    print2uart(UART_ESP, AT_CWQAP);
    __delay_cycles(48000000);

    return commandSuccess();
}

bool ESP8266_establishConnection(char id, unsigned char type, char *address, char *port)
{
    char ct[3];

    switch (type) {
        case TCP:
            ct[0] = 'T'; ct[1] = 'C'; ct[2] = 'P';
            break;
        case UDP:
            ct[0] = 'U'; ct[1] = 'D'; ct[2] = 'P';
            break;
    }

    print2uart(UART_ESP, "%s=%c,\"%s\",\"%s\",%s\r\n", AT_CIPSTART, id, ct, address, port);

    __delay_cycles(24000000);

    return commandSuccess();
}

bool ESP8266_enableMultipleConnecitons(bool enable)
{
    char c;

    switch (enable) {
        case 0:
            c = '0';
            break;
        case 1:
            c = '1';
            break;
    }

    print2uart(UART_ESP, "%s=%c\r\n", AT_CIPMUX, c);

    __delay_cycles(12000000);

    return commandSuccess();
}

bool ESP8266_sendData(char id, char *data, unsigned int dataSize)
{
    char size[5];
    ltoa(dataSize, size);
    print2uart(UART_ESP, "%s=%c,%s\r\n", AT_CIPSEND, id, size);

    __delay_cycles(24000000);

    if (!commandSuccess()) {
        return false;
    }

    print2uart(UART_ESP, data);

    __delay_cycles(48000000);

    return commandSuccess();
}

char *ESP8266_getBuffer(void)
{
    return ESP8266_Buffer;
}

bool ESP8266_reset(void)
{
    print2uart(UART_ESP, AT_RST);

    return commandSuccess();
}
