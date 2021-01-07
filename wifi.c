
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

        if (progressIDX >= (strlen(message) - 1))
            break;

        
        struct working data;
        DBworking(&data, DID0);
        unsigned long* progressIdxPtr = data.address;
        *progressIdxPtr = progressIDX;
        DID0 = DBcommit(&data, 4, 1);
        __delay_cycles(320000);
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
    ESP8266_disconnectServer("5");
    return isSuccess;
}

bool checkMQTTClientConfig(void)
{
    char *ESP_Data = ESP8266_getBuffer();
    if (!ESP8266_getMQTTUserConf()) {
        dprint2uart(UART_STDOUT,
                    "Fail to get MQTT user config with response:\r\n%s\r\n", ESP_Data);
    }

    if (strstr(ESP_Data, MQTT_CLIENT_ID) != NULL) {
        return true;
    }
    return false;
}

bool setupMQTTClientConfig(void)
{
    char *ESP_Data = ESP8266_getBuffer();
    if (!ESP8266_setMQTTUserConf(MQTT_SCHEME, MQTT_CLIENT_ID, "", "", "")) {
        dprint2uart(UART_STDOUT,
                    "Fail to setup MQTT user config with response:\r\n%s\r\n", ESP_Data);
    }

    if (!checkMQTTClientConfig()) {
        dprint2uart(UART_STDOUT,
                    "MQTT Config setup unsuccessful\r\n");
        return false;
    }

    return true;
}

bool connectToBroker(void)
{
    char *ESP_Data = ESP8266_getBuffer();

    if (!checkMQTTClientConfig()) {
        if (!setupMQTTClientConfig()) {
            dprint2uart(UART_STDOUT,
                        "Fail to setup MQTT Config\r\n");
            return false;
        }
    }

    if (!ESP8266_connectToMQTTBroker(SERVER_IP, MQTT_PORT, 0)) {
        dprint2uart(UART_STDOUT,
                    "Fail to connect to MQTT broker with response:\r\n%s\r\n", ESP_Data);
        return false;
    }

    return true;
}

bool checkMQTTConnection(void)
{
    char *ESP_Data = ESP8266_getBuffer();
    if (!ESP8266_getMQTTConnectStatus()) {
        dprint2uart(UART_STDOUT,
                    "Fail to get MQTT Connection Status with response:\r\n%s\r\n", ESP_Data);
    }
    if (strstr(ESP_Data, SERVER_IP) != NULL) {
        return true;
    }
    return false;
}

bool sendMQTTData(char *data)
{
    char *ESP_Data = ESP8266_getBuffer();
    if (!ESP8266_publishMessage(MQTT_TOPIC, data, 0, 0)) {
        dprint2uart(UART_STDOUT,
                    "Fail to publish MQTT message with response:\r\n%s\r\n", ESP_Data);
        return false;
    }
    return true;
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

unsigned int constructPOSTRequest(char *endPoint, char *data, char *requestBody)
{
    int16_t dataSize = strlen(data);
    // requestBody = "POST /post HTTP/1.0\r\nContent-Type: application/json\r\nContent-Length: 48\r\n\r\n{\"message\": \"hello world\", \"content\": \"abcabc\"}\r\n\r\n\0";
    // requestBody = "POST /post HTTP/1.0\r\nContent-Type: application/json\r\nContent-Length: 648\r\n\r\n{\"message\": \"hello world\", \"content\": \"sdj;asliebjdurghdurghdkgsdgsdlhrgsdhr gsdrjsdl;jbsdlirjsrilbjsrlijsdgsdjilrgsijdrl;nsdijbsdl;rigjsd;lijsd;rlisjdrl;nirjl;gsdrjgsd;gjsd;lrifjsl;rfjisd;flbisdjl;bsjir;blfj'aesgjisdl;rbjisd;lsdjlri;hsjdrlgsdrglsdjrglisdjsl;dirjnsdrigshdlrgjsdrl;gisjdblsdirjbs;dlrigjsdl;rigjsdrl;gisdjr;lsdbjILELSijslefjas;elgjialeirjasleifj;eiljfiljfasuhgughuerfhai;efsjJ:LIEJ:ILJIEHFGSEFUH:faeafsleivjbl;aeifjas;leifjasl;ibjasel;fijskfhrukgsdhgushurhsrguidhr89348tyghjaslekf;asebhaksuehf;egasefkljhgsdkgjsl;rgkjdltuhgisejrg;i5jg4e5i8gj;ijs;lrijdbfl;tmnidftl;hndf'gdrjg;dlritjg;\"}\r\n\r\n\0"
    /*sprintf(requestBody,
            "POST %s HTTP/1.0\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n\0",
            endPoint,
            dataSize,
            data_ptr);*/

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

unsigned long getSubString(char* subStr, unsigned long start, unsigned int length)
{
    unsigned long cnt = (start + length >= strlen(message)) ? (strlen(message)-start) : length;
    strncpy(subStr, message + start, cnt);
    subStr[cnt] = '\0';
    

    return (start + cnt);
}
