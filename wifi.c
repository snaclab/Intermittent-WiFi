
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "driverlib.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "TaskManager/taskManager.h"
#include "DataManager/SimpDB.h"
#include "Devices/ESP8266.h"
#include "AppTools/mqtt.h"
#include "Tools/myuart.h"
#include "config.h"
#include "wifi.h"

extern int DID0;

int RF_POWER = 78;
int Tries = 5;

char message[] = "Hello World. Let's debug together. Why the hell the upper message cannot be sent"; 
const unsigned int maxMsgLength = 20;

TickType_t sleepTime = 3000;

void wifiCommunicate(void);

void wifi(void)
{
    xTaskCreate(wifiCommunicate, "wifiCommunicate", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL, IDWIFI, INVM);
}

void wifiCommunicate(void)
{
    registerTCB(IDWIFI);
    dprint2uart(UART_STDOUT, "Begin Wifi test\r\n");

    char *ESP_Data = ESP8266_getBuffer();

    if (!initConnectModule(Tries)) {
        dprint2uart(UART_STDOUT, "Fail to communicate with module.\r\n");
        goto END;
    }

    dprint2uart(UART_STDOUT, "Pass Module UART check\r\n");

    if (!checkConnection(Tries)) {
        if (!establishWiFiConnection(Tries)) {
            dprint2uart(UART_STDOUT,
                        "Fail to establish WiFi connection with response:\r\n%s\r\n", ESP_Data);
            goto END;
        }
    }

    while (true) {
        if (!checkConnection(Tries)) {
            dprint2uart(UART_STDOUT, "WiFi disconnected again!!!!!\r\n");
            if (!establishWiFiConnection(Tries)) {
                dprint2uart(UART_STDOUT,
                            "Fail to establish WiFi connection with response:\r\n%s\r\n", ESP_Data);
                goto END;
            }
        }

        if (!checkMQTTConnection()) {
            if (!connectToBroker()) {
                dprint2uart(UART_STDOUT,
                            "Fail to connect to MQTT broker\r\n");
                goto END;
            }
        }

        unsigned long progressIDX = 0;
        
        dprint2uart(UART_STDOUT, "DID0: %d\r\n", DID0);

        if (DID0 >= 0) {
            DBreadIn(&progressIDX, DID0);
        }

        dprint2uart(UART_STDOUT, "progressIDX: %d\r\n", progressIDX);

        char subMessage[maxMsgLength];
        progressIDX = getSubString(subMessage, progressIDX, 2);

        if (!sendMQTTData(subMessage)) {
            dprint2uart(UART_STDOUT,
                        "Fail to send MQTT data\r\n");
            continue;
        }

        if (progressIDX >= (strlen(message) - 1)) {
            dprint2uart(UART_STDOUT, "A round has been done, take a rest.\r\n");
            progressIDX = 0;
            __delay_cycles(640000000);
            dprint2uart(UART_STDOUT, "Continue next round.\r\n");
        }

        
        struct working data;
        DBworking(&data, DID0);
        unsigned long* progressIdxPtr = data.address;
        *progressIdxPtr = progressIDX;
        DID0 = DBcommit(&data, 4, 1);
        __delay_cycles(640000);
    }


END:
    ESP8266_disconnectFromAP();
    dprint2uart(UART_STDOUT, "At the end \r\n");
    unregisterTCB(IDWIFI);
    return;
}

bool establishWiFiConnection(int tries)
{
    char *ESP_Data = ESP8266_getBuffer();
    while (tries) {
        if (!ESP8266_connectToAP(AP_SSID, AP_PASSWORD, 1)) {
            dprint2uart(UART_STDOUT,
                    "Fail to connect to AP with response:\r\n%s\r\n", ESP_Data);
        }

        if (!ESP8266_getIP()) {
            dprint2uart(UART_STDOUT, "Fail to get IP \r\n");
            continue;
        }

        if (strstr(ESP_Data, "192.168") != NULL) {
            return true;
        }
        tries--;
    }

    return false;
}

bool checkConnection(int tries)
{
    char *ESP_Data = ESP8266_getBuffer();
    while (tries) {
        if (ESP8266_getIP()) {
            break;
        }
        dprint2uart(UART_STDOUT, "Fail to get IP \r\n");
        tries--;
    }

    if (strstr(ESP_Data, "192.168") != NULL) {
        return true;
    }

    return false;
}

bool initConnectModule(int tries)
{
    // Init UART Interface
    uartInit(UART_ESP);
    uartBufferFlush();

    // Hard reset ESP8266
    // ESP8266_hardReset();

    __delay_cycles(24000);

    if (!checkModule(tries)) {
        ESP8266_hardReset();
        dprint2uart(UART_STDOUT, "Reset ESP8266\r\n");
        if (!checkModule(tries)) {
            return false;
        }
    }

    return true;
}

bool checkModule(int tries)
{
    char *ESP_Data = ESP8266_getBuffer();
    while (tries) {
        if (ESP8266_checkModule()) {
            return true;
        }
        tries--;
    }
    return false;
}

unsigned long getSubString(char* subStr, unsigned long start, unsigned int length)
{
    unsigned long cnt = (start + length >= strlen(message)) ? (strlen(message)-start) : length;
    strncpy(subStr, message + start, cnt);
    subStr[cnt] = '\0';
    

    return (start + cnt);
}
