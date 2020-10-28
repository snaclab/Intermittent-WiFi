
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "driverlib.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "TaskManager/taskManager.h"
#include "DataManager/SimpDB.h"
#include "Devices/ESP8266.h"
#include "Tools/myuart.h"
#include "config.h"
#include "wifi.h"
#include "data.h"

extern const char *squirtle_ptr;

#pragma NOINIT(data)
char data[DATA_SIZE];

char *data_ptr = data;

#pragma NOINIT(requestBody)
char requestBody[REQUEST_SIZE];

void wifiCommunicate(void);

void wifi(void)
{
    xTaskCreate(wifiCommunicate, "wifiCommunicate", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL, IDWIFI, INVM);
}

void wifiCommunicate(void)
{
    registerTCB(IDWIFI);

    const int Tries = 5;
    char endpoint[] = "/post";
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

    dprint2uart(UART_STDOUT, "WiFi connected\r\n");

    ESP8266_ping("www.google.com");

    dprint2uart(UART_STDOUT, "PING:\r\n%s\r\n", ESP_Data);

    constructData(data);
    unsigned int requestLength = constructPOSTRequest(endpoint, data, requestBody);

    dprint2uart(UART_STDOUT, "request:\r\n%s\r\n", requestBody);

    int count = 50;
    while (count) {
        int tries = Tries;

        dprint2uart(UART_STDOUT, "data length: %d\r\n", strlen(data));
        dprint2uart(UART_STDOUT, "Request length: %d\r\n", strlen(requestBody));

        const TickType_t sleepTime = 1000;

        while (tries) {
            if (sendTCP(requestBody, requestLength)) {
                break;
            }
            dprint2uart(UART_STDOUT, "Fail to send TCP data\r\n");
            tries--;
        }
        // sleep for 1 sec
        TickType_t startTime = xTaskGetTickCount();
        while (xTaskGetTickCount() - startTime < sleepTime) {
            __delay_cycles(2400);
        }
        count--;
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
        if (!ESP8266_connectToAP(AP_SSID, AP_PASSWORD)) {
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

    if (strstr(ESP_Data, "172.20") != NULL) {
        dprint2uart(UART_STDOUT, "ESP_DATA:\r\n%s\r\n", ESP_Data);
        return true;
    }

    return false;
}

bool sendTCP(char *requestBody, unsigned int requestLength)
{
    bool isSuccess = false;
    char *ESP_Data = ESP8266_getBuffer();

    if (!ESP8266_establishTCPConnection(TCP, SERVER_IP, SERVER_PORT)) {
        dprint2uart(UART_STDOUT,
                    "Fail to establish connection to AP with response:\r\n%s\r\n", ESP_Data);
    }

    dprint2uart(UART_STDOUT, "TCP Connected:\r\n%s\r\n", ESP_Data);

    if (!ESP8266_sendData(requestBody, requestLength)) {
        dprint2uart(UART_STDOUT,
                    "Fail to send data to remote with response:\r\n%s\r\n", ESP_Data);
        goto END;
    }

    dprint2uart(UART_STDOUT, "TCP Sent:\r\n%s\r\n", ESP_Data);

    isSuccess = true;

END:
    ESP8266_disconnectServer("0");
    return isSuccess;
}

bool initConnectModule(int tries)
{
    // Init UART Interface
    uartInit(UART_ESP);
    uartBufferFlush();

    // Hard reset ESP8266
    //ESP8266_hardReset();

    if (!checkModule(tries)) {
        return false;
    }

    return true;
}

bool checkModule(int tries)
{
    while (tries) {
        if (ESP8266_checkModule()) {
            return true;
        }
        tries--;
    }
    return false;
}

unsigned int constructPOSTRequest(char *endPoint, char *data, char *requestBody)
{
    int16_t dataSize = strlen(data);
    dprint2uart(UART_STDOUT, "dataSize: %d\r\n", dataSize);
    dprint2uart(UART_STDOUT, "begin construct Post\r\n");
    sprintf(requestBody,
            "POST %s HTTP/1.0\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n\0",
            endPoint,
            dataSize,
            data_ptr);

    dprint2uart(UART_STDOUT, "pass sprintf request body\r\n");
    return strlen(requestBody);
}

void constructData(char *data)
{
    sprintf(data,
            "{\"message\": \"%s\", \"image\": \"%s\"}\0",
            "abcc",
            "avbas");

    dprint2uart(UART_STDOUT, "pass construct data\r\n");
    return;
}
