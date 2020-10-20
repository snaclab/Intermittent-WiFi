
#include "config.h"
#include "driverlib.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "TaskManager/taskManager.h"
#include "DataManager/SimpDB.h"
#include "wifi.h"
#include "Devices/ESP8266.h"
#include "Tools/myuart.h"

TickType_t startTime, endTime;

void wifiConnect(void);
bool initConnectModule(void);

char data[] = "message=Cool";
// char dataLength[5];
// itoa(sizeof(data)-1, dataLength, 10);
char HTTP_Request[] = "POST /post HTTP/1.0\r\nContent-Type: application/json\r\nContent-Length: 18\r\n\r\n{\"message\":\"ABAB\"}\r\n\r\n";
// char HTTP_Request[] = "POST /post HTTP/1.0\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\nmessage=Cool\r\n\r\n";
unsigned int HTTP_Size = sizeof(HTTP_Request) - 1;

void wifi(void)
{
    xTaskCreate(wifiConnect, "wifiConneciton", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL, IDWIFI, INVM);
}

void wifiConnect(void)
{
    registerTCB(IDWIFI);
    char *ESP_Data = ESP8266_getBuffer();

    if (!initConnectModule()) {
        dprint2uart(UART_STDOUT, "Initialize fail!!\r\n");
        dprint2uart(UART_STDOUT, "%s \r\n", ESP_Data);
        goto END;
    }

    //dprint2uart(UART_STDOUT, "begin get SystemInfo \r\n");
    //startTime = xTaskGetTickCount();
    //if (!ESP8266_getSystemInfo()) {
    //    dprint2uart(UART_STDOUT,
    //                "Get system info fail with response: %s \r\n", ESP_Data);
    //    goto END;
    //}
    //endTime = xTaskGetTickCount();
    //dprint2uart(UART_STDOUT, "GetSystemInfo Time cost: %l ms \r\n", (endTime - startTime));
    //dprint2uart(UART_STDOUT, "%s \r\n", ESP_Data);

    //dprint2uart(UART_STDOUT, "begin get WiFi Mode \r\n"); 
    //startTime = xTaskGetTickCount();
    //if (!ESP8266_getCurrentWiFiMode()) {
    //    dprint2uart(UART_STDOUT,
    //                "Get WiFi mode fail with response: %l \r\n", ESP_Data);
    //    goto END;
    //}
    //endTime = xTaskGetTickCount();
    //dprint2uart(UART_STDOUT, "GetWiFiMode Time cost: %l ms \r\n", (endTime - startTime));
    //dprint2uart(UART_STDOUT, "%s \r\n", ESP_Data);

    if (!ESP8266_setDHCP(1, 0)) {
        dprint2uart(UART_STDOUT,
                    "Fail to disable DHCP \r\n");
        goto END;
    }

    if (!ESP8266_getDHCPStatus()) {
        dprint2uart(UART_STDOUT,
                    "Fail to get DHCP status with response:\r\n%s\r\n", ESP_Data);
    }

    dprint2uart(UART_STDOUT, "%s \r\n", ESP_Data);

    if (!ESP8266_setStaticIP("192.168.50.23")) {
        dprint2uart(UART_STDOUT,
                    "Fail to set static IP with response:\r\n%s\r\n", ESP_Data);
        goto END;
    }

    dprint2uart(UART_STDOUT, "begin connect to AP \r\n");

    startTime = xTaskGetTickCount();
    if (!ESP8266_connectToAP("User", "0975222283")) {
        dprint2uart(UART_STDOUT,
                    "Fail to connect to AP with response:\r\n%s\r\n", ESP_Data);
    }
    endTime = xTaskGetTickCount();

    dprint2uart(UART_STDOUT, "ConnectToAP Time cost: %l ms \r\n", (endTime - startTime));
    dprint2uart(UART_STDOUT, "%s \r\n", ESP_Data);

    if (!ESP8266_getIP()) {
        dprint2uart(UART_STDOUT, "Fail to get IP \r\n");
    }

    dprint2uart(UART_STDOUT, "%s \r\n", ESP_Data);

    dprint2uart(UART_STDOUT, "begin establish TCP conneciton \r\n");

    if (!ESP8266_establishConnection(TCP, "192.168.50.181", "9000")) {
        dprint2uart(UART_STDOUT,
                    "Fail to establish connection to AP with response:\r\n%s\r\n", ESP_Data);
        // goto END;
    }

    if (!ESP8266_getConnectStatus()) {
        dprint2uart(UART_STDOUT,
                    "Fail to get connect Info with response:\r\n%s\r\n", ESP_Data);
        goto END;
    }

    dprint2uart(UART_STDOUT, "%s \r\n", ESP_Data);

    dprint2uart(UART_STDOUT, "begin send http request \r\n");

    dprint2uart(UART_STDOUT, "Request: %s\r\n", HTTP_Request);
    dprint2uart(UART_STDOUT, "size: %d \r\n", HTTP_Size);

    if (!ESP8266_sendData(HTTP_Request, HTTP_Size)) {
        dprint2uart(UART_STDOUT,
                    "Fail to send data to remote with response:\r\n%s\r\n", ESP_Data);
        goto END;
    }

    // dprint2uart(UART_STDOUT, "Idle for a while.\r\n");

    // __delay_cycles(96000000);

    //if (!ESP8266_disconnectFromAP()) {
    //    dprint2uart(UART_STDOUT,
    //                "Fail to disconnect from AP with response: %s \r\n", ESP_Data);
    //    goto END;
    //}

END:
    ESP8266_disconnectServer("0");
    ESP8266_disconnectFromAP();
    dprint2uart(UART_STDOUT, "At the end \r\n");
    unregisterTCB(IDWIFI);
    return;
}

bool initConnectModule(void)
{
    char* ESP_Data = ESP8266_getBuffer();
    int Tries = 5;
    // Init UART Interface
    uartInit(UART_ESP);
    uartBufferFlush();

    // Hard reset ESP8266
    // ESP8266_hardReset();

    TickType_t startTime = xTaskGetTickCount();
    while (Tries) {
        if (ESP8266_checkConnection()) {
            return true;
        }

        Tries--;
    }
    return false;
}


