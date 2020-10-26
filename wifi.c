
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

extern char *squirtle_ptr, *charmander_ptr, *bulbasaur_ptr;
char data[2048];

const int Tries = 5;

char images[3][10] = {
        "squirtle",
        "charmander",
        "bulbasaur"
    };

void communicate(void);

void wifi(void)
{
    xTaskCreate(communicate, "communicate", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL, IDWIFI, INVM);
}

void communicate(void)
{
    registerTCB(IDWIFI);

    char endpoint[] = "/post";
    int tries = Tries;
    char *ESP_Data = ESP8266_getBuffer();

    if (!checkModule(tries)) {
        dprint2uart(UART_STDOUT, "Fail to communicate with module.\r\n");
        goto END;
    }

    if (!checkConnection(tries)) {
        if (!establishWiFiConnection()) {
            dprint2uart(UART_STDOUT,
                        "Fail to establish WiFi connection.\r\n");
            goto END;
        }
    }

    while (true) {
        registerTCB(IDWIFI);

        srand(3);
        int x = rand();

        constructData(x, data);

        const TickType_t sleepTime = 1000;

        while (tries) {
            if (sendTCP(endpoint, data)) {
                break;
            }
            dprint2uart(UART_STDOUT, "Fail to send TCP data\r\n");
            tries--;
        }

        unregisterTCB(IDWIFI);
        // sleep for 1 sec
        TickType_t startTime = xTaskGetTickCount();
        while (xTaskGetTickCount() - startTime < sleepTime) {
            __delay_cycles(2400);
        }
    }

END:
    ESP8266_disconnectFromAP();
    dprint2uart(UART_STDOUT, "At the end \r\n");
    return;
}

bool establishWiFiConnection(int Tries)
{
    char *ESP_Data = ESP8266_getBuffer();
    while (Tries) {
        if (!ESP8266_connectToAP(AP_SSID, AP_PASSWORD)) {
            dprint2uart(UART_STDOUT,
                    "Fail to connect to AP with response:\r\n%s\r\n", ESP_Data);
        }

        if (!ESP8266_getIP()) {
            dprint2uart(UART_STDOUT, "Fail to get IP \r\n");
            continue;
        }

        if (strstr(ESP_Data, STATIC_IP) != NULL) {
            return true;
        }
        Tries--;
    }

    return false;
}

bool checkConnection(int Tries)
{
    char *ESP_Data = ESP8266_getBuffer();
    while (Tries) {
        if (ESP8266_getIP()) {
            break;
        }
        dprint2uart(UART_STDOUT, "Fail to get IP \r\n");
        Tries--;
    }

    if (strstr(ESP_Data, STATIC_IP) != NULL) {
        return true;
    }

    return false;
}

bool sendTCP(char *endPoint, char *data)
{
    bool isSuccess = false;
    char requestBody[512];
    char *ESP_Data = ESP8266_getBuffer();
    unsigned int requestLength = constructPOSTRequest(endPoint, data, requestBody);

    if (!ESP8266_establishTCPConnection(TCP, SERVER_IP, SERVER_PORT)) {
        dprint2uart(UART_STDOUT,
                    "Fail to establish connection to AP with response:\r\n%s\r\n", ESP_Data);
    }

    if (!ESP8266_sendData(requestBody, requestLength)) {
        dprint2uart(UART_STDOUT,
                    "Fail to send data to remote with response:\r\n%s\r\n", ESP_Data);
        goto END;
    }

    isSuccess = true;

END:
    ESP8266_disconnectServer("0");
    return isSuccess;
}

bool initConnectModule(void)
{
    char* ESP_Data = ESP8266_getBuffer();
    // Init UART Interface
    uartInit(UART_ESP);
    uartBufferFlush();

    // Hard reset ESP8266
    ESP8266_hardReset();

    if (!checkModule()) {
        return false;
    }

    return true;
}

bool checkModule(int Tries)
{
    while (Tries) {
        if (ESP8266_checkModule()) {
            return true;
        }
        Tries--;
    }
    return false;
}

unsigned int constructPOSTRequest(char *endPoint, char *data, char *requestBody)
{
    int dataSize = strlen(data);

    sprintf(requestBody,
            "POST %s HTTP/1.0\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n\0",
            endPoint,
            dataSize,
            data);

    return strlen(requestBody);
}

void constructData(int img, char *data)
{
    char *image_data[3] = {
        squirtle_ptr,
        charmander_ptr,
        bulbasaur_ptr
    };
    sprintf(data,
            "{\"message\": \"%s\", \"image\": \"%s\"}\0",
            images[img],
            image_data[img]);

    return;
}