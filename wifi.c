
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

    if (!ESP8266_setDHCP(1, 1)) {
        dprint2uart(UART_STDOUT,
                    "Fail to disable DHCP \r\n");
        goto END;
    }

    if (!ESP8266_getDHCPStatus()) {
        dprint2uart(UART_STDOUT,
                    "Fail to get DHCP status with response:\r\n%s\r\n", ESP_Data);
    }

    dprint2uart(UART_STDOUT, "%s \r\n", ESP_Data);

    // if (!ESP8266_setStaticIP("172.20.10.6")) {
    //     dprint2uart(UART_STDOUT,
    //                 "Fail to set static IP with response:\r\n%s\r\n", ESP_Data);
    //     goto END;
    // }

    dprint2uart(UART_STDOUT, "begin connect to AP \r\n");

    startTime = xTaskGetTickCount();
    if (!ESP8266_connectToAP("LinIPHONE", "0978637728")) {
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

    dprint2uart(UART_STDOUT, "Idle for a while.\r\n");

    __delay_cycles(96000000);

    //if (!ESP8266_disconnectFromAP()) {
    //    dprint2uart(UART_STDOUT,
    //                "Fail to disconnect from AP with response: %s \r\n", ESP_Data);
    //    goto END;
    //}

END:
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


