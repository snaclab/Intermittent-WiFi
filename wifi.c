
#include "config.h"
#include "driverlib.h"
#include "FreeRTOSConfig.h"
#include "TaskManager/taskManager.h"
#include "DataManager/SimpDB.h"
#include "wifi.h"
#include "Devices/ESP8266.h"
#include "Tools/myuart.h"

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
        goto END;
    }

    if (!ESP8266_getSystemInfo()) {
        dprint2uart(UART_STDOUT,
                    "Get system info fail with response: %s \r\n", ESP_Data);
        goto END;
    }
    dprint2uart(UART_STDOUT, "%s \r\n", ESP_Data);

    if (!ESP8266_getCurrentWiFiMode()) {
        dprint2uart(UART_STDOUT,
                    "Get WiFi mode fail with response: %s \r\n", ESP_Data);
        goto END;
    }
    dprint2uart(UART_STDOUT, "%s \r\n", ESP_Data);

    if (!ESP8266_connectToAP("LinIPHONE", "0978637728")) {
        dprint2uart(UART_STDOUT,
                    "Fail to connect to AP with response:\r\n%s\r\n", ESP_Data);
        goto END;
    }
    dprint2uart(UART_STDOUT, "%s \r\n", ESP_Data);
 
    dprint2uart(UART_STDOUT, "sleep for a while.\r\n");
    __delay_cycles(96000000);

    if (!ESP8266_disconnectFromAP()) {
        dprint2uart(UART_STDOUT,
                    "Fail to disconnect from AP with response: %s \r\n", ESP_Data);
        goto END;
    }
    dprint2uart(UART_STDOUT, "%s \r\n", ESP_Data);

    dprint2uart(UART_STDOUT, "pass pass \r\n");

END:
    unregisterTCB(IDWIFI);
    return;
}

bool initConnectModule(void)
{
    char* ESP_Data = ESP8266_getBuffer();
    int Tries = 10;
    // Init UART Interface
    uartInit(UART_ESP);
    uartBufferFlush();

    // Reset ESP8266
    if (!ESP8266_reset()) {
        return false;
    }

    // Working: hard reset ESP8266
    // ESP8266_hardReset();

    while (Tries) {
        if (ESP8266_checkConnection()) {
            return true;
        }

        __delay_cycles(240000);
        Tries--;
    }
    return false;
}


