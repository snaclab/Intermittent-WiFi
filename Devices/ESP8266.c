
#include <string.h>
#include "ESP8266.h"
#include "FreeRTOS.h"
#include "driverlib.h"
#include "Tools/myuart.h"

// TODO: need to check reset pin for msp430 + esp8266-01
#define RESET_PIN       GPIO_PIN4
#define RESET_PORT      GPIO_PORT_P3

#define AT              "AT\r\n"  // Test AT starup
#define AT_RST          "AT+RST\r\n" // Restarts the module (soft reset)
#define AT_GMR          "AT+GMR\r\n" // Check version information
#define AT_GSLP         "AT+GSLP\r\n" // Enter Deep-sleep mode
#define ATE             "ATE\r\n" // Configures echoing of AT commands
#define AT_CWMODE       "AT+CWMODE_CUR" // set WiFi mode (STA/AP/STA+AP)
#define AT_CWMODE_Q     "AT+CWMODE?\r\n" // get current WiFi mode
#define AT_CWJAP        "AT+CWJAP_CUR" // Connect to AP
#define AT_CWLAP        "AT+CWLAP\r\n" // List Available APs
#define AT_CWQAP        "AT+CWQAP\r\n" // Disconnects from AP
#define AT_CWLIF        "AT+CWLIF\r\n" // get info SoftAP station from connected ESP8266
#define AT_CWDHCP       "AT+CWDHCP_CUR" // set DHCP
#define AT_CWDHCP_Q     "AT+CWDHCP_CUR?\r\n" // get DHCP status
#define AT_CIPSTAMAC    "AT+CIPSTAMAC\r\n" // set station MAC address
#define AT_CIPAPMAC     "AT+CIPAPMAC\r\n" // set AP MAC address
#define AT_CIPSTA       "AT+CIPSTA_CUR" // set STA IP address
#define AT_CIPAP        "AT+CIPAP" // set AP IP address
#define AT_CIPSTATUS    "AT+CIPSTATUS\r\n" // check connection status
#define AT_CIPSTART     "AT+CIPSTART" // establish TCP/UDP/SSL connection
#define AT_CIPSEND      "AT+CIPSEND" // send data
#define AT_CIPCLOSE     "AT+CIPCLOSE" // close connection
#define AT_CIFSR        "AT+CIFSR\r\n" // look for local IP address
#define AT_CIPMUX       "AT+CIPMUX" // set multiple conneciton
#define AT_CIPSERVER    "AT+CIPSTO\r\n" // establish TCP server
#define AT_CIPMODE      "AT+CIPMODE\r\n" // set connection mode
#define AT_CIPSTO       "AT+CIPSTO\r\n" // set TCP server timeout 
#define AT_CIUPDATE     "AT+CIUPDATE\r\n" // update software through Wi-Fi
#define IPD             "+IPD\r\n" 

char ESP8266_Buffer[ESP8266_BUFFER_SIZE];
const TickType_t timeout = 20000;

void emptyBuffer(void)
{
    memset(ESP8266_Buffer, 0, ESP8266_BUFFER_SIZE);
}

bool waitForResponse(char *target)
{
    unsigned char c;
    unsigned int i = 0;

    emptyBuffer();

    TickType_t startTime = xTaskGetTickCount();
    
    while(xTaskGetTickCount() - startTime < timeout) {
        while (uartA3GotMessage()) {
            c = readFromUartA3();

            if (i > ESP8266_BUFFER_SIZE) {
                return false;
            } else {
                ESP8266_Buffer[i++] = c;
            }
        }
        // Need to find a smarter solution
        if (strstr(ESP8266_Buffer, target) != NULL) {
            ESP8266_Buffer[i++] = '\0';
            return true;
        }
    }

    return false;
}

bool ESP8266_getCurrentWiFiMode(void)
{
    print2uart(UART_ESP, AT_CWMODE_Q);

    return waitForResponse("OK");
}

bool ESP8266_getIP(void)
{
    print2uart(UART_ESP, AT_CIFSR);

    return waitForResponse("OK");
}

bool ESP8266_changeWiFiMode(unsigned int MODE)
{
    print2uart(UART_ESP, "%s=%d\r\n", AT_CWMODE, MODE);

    return waitForResponse("OK");
}

bool ESP8266_checkModule(void)
{
    print2uart(UART_ESP, AT);

    return waitForResponse("OK");
}

bool ESP8266_getSystemInfo(void)
{
    print2uart(UART_ESP, AT_GMR);

    return waitForResponse("OK");
}

bool ESP8266_availableAPs(void)
{
    print2uart(UART_ESP, AT_CWLAP);

    return waitForResponse("OK");
}

bool ESP8266_getDHCPStatus(void)
{
    print2uart(UART_ESP, AT_CWDHCP_Q);

    return waitForResponse("OK");
}

bool ESP8266_setDHCP(int mode, int en)
{
    print2uart(UART_ESP, "%s=%d,%d\r\n", AT_CWDHCP, mode, en);

    return waitForResponse("OK");
}

bool ESP8266_connectToAP(char *SSID, char *password)
{
    print2uart(UART_ESP, "%s=\"%s\",\"%s\"\r\n", AT_CWJAP, SSID, password);
    
    return waitForResponse("OK");
}

bool ESP8266_getConnectStatus(void)
{
    print2uart(UART_ESP, AT_CIPSTATUS);

    return waitForResponse("OK");
}

bool ESP8266_disconnectFromAP(void)
{
    print2uart(UART_ESP, AT_CWQAP);

    return waitForResponse("OK");
}

bool ESP8266_setStaticIP(char *IP)
{
    print2uart(UART_ESP, "%s=\"%s\"\r\n", AT_CIPSTA, IP);

    return waitForResponse("OK");
}

bool ESP8266_establishTCPConnection(unsigned char type, char *address, char *port)
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

    print2uart(UART_ESP, "%s=\"%s\",\"%s\",%s\r\n", AT_CIPSTART, ct, address, port);

    return waitForResponse("OK");
}

bool ESP8266_disconnectServer(char *linkID)
{
    print2uart(UART_ESP, "%s=%s\r\n", AT_CIPCLOSE, linkID);

    return waitForResponse("OK");
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

    return waitForResponse("OK");
}

bool ESP8266_sendData(char *data, unsigned int dataSize)
{
    print2uart(UART_ESP, "%s=%d\r\n", AT_CIPSEND, dataSize);

    if (!waitForResponse(">")) {
        return false;
    }

    print2uart(UART_ESP, data);

    return waitForResponse("OK");
}

char *ESP8266_getBuffer(void)
{
    return ESP8266_Buffer;
}

void ESP8266_reset(void)
{
    print2uart(UART_ESP, AT_RST);

    __delay_cycles(24000000);
}

void ESP8266_hardReset(void)
{
    GPIO_setOutputLowOnPin(RESET_PORT, RESET_PIN);

    __delay_cycles(24000);

    GPIO_setOutputHighOnPin(RESET_PORT, RESET_PIN);
}
