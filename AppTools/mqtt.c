#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Devices/ESP8266.h"
#include "Tools/myuart.h"
#include "config.h"

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

bool getMQTTRecvMessage(char *topic, char *recvData)
{
    char *ESP_Data = ESP8266_getBuffer();
    if (!ESP8266_getMQTTRecvMessagge()) {
        dprint2uart(UART_STDOUT,
                    "Cannot get MQTT message with response:\r\n%s\r\n", ESP_Data);
        return false;
    }

    {
        unsigned delimCount = 0, dataLength = 0;
        char * token = strtok(ESP_Data, ":");
        while (token != NULL) {
            token = strtok(NULL, ",");
            delimCount++;
            if (delimCount == 2) {
                if (strstr(token, topic) == NULL) {
                    dprint2uart(UART_STDOUT,
                                "Wrong Topic with response:\r\n%s\r\n", ESP_Data);
                    return false;
                }
            }

            if (delimCount == 3)
                sscanf(token, "%d", &dataLength);

            if (delimCount == 4) {
                strncpy(recvData, token, dataLength-5);
                recvData[dataLength-5] = '\0';
            }
        }
    }

    return true;
}