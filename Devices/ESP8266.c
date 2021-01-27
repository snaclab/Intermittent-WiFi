
#include <string.h>

#include "ESP8266.h"
#include "FreeRTOS.h"
#include "driverlib.h"
#include "Tools/myuart.h"

// TODO: need to check reset pin for msp430 + esp8266-01
#define RESET_PIN       GPIO_PIN4
#define RESET_PORT      GPIO_PORT_P3

#define AT                "AT\r\n"  // Test AT starup
#define AT_RST            "AT+RST\r\n" // Restarts the module (soft reset)
#define AT_GMR            "AT+GMR\r\n" // Check version information
#define AT_GSLP           "AT+GSLP\r\n" // Enter Deep-sleep mode
#define AT_CWMODE         "AT+CWMODE" // set WiFi mode (STA/AP/STA+AP)
#define AT_CWJAP          "AT+CWJAP" // Connect to AP
#define AT_CWLAP          "AT+CWLAP\r\n" // List Available APs
#define AT_CWQAP          "AT+CWQAP\r\n" // Disconnects from AP
#define AT_CWDHCP         "AT+CWDHCP" // set DHCP
#define AT_SYSLOG         "AT+SYSLOG" // set syslog
#define AT_CIPSTA         "AT+CIPSTA" // set STA IP address
#define AT_CIPAP          "AT+CIPAP" // set AP IP address
#define AT_CIPSTATUS      "AT+CIPSTATUS\r\n" // check connection status
#define AT_CIPSTART       "AT+CIPSTART" // establish TCP/UDP/SSL connection
#define AT_CIPSEND        "AT+CIPSEND" // send data
#define AT_CIPCLOSE       "AT+CIPCLOSE" // close connection
#define AT_CIFSR          "AT+CIFSR\r\n" // look for local IP address
#define AT_CIPMODE        "AT+CIPMODE\r\n" // set connection mode
#define AT_MQTTUSERCFG    "AT+MQTTUSERCFG" // set MQTT user config
#define AT_MQTTCONN       "AT+MQTTCONN" // connect to MQTT broker
#define AT_MQTTSUB        "AT+MQTTSUB" // subscribe to MQTT topic
#define AT_MQTTPUB        "AT+MQTTPUB" // publish MQTT message
#define AT_MQTTCLEAN      "AT+MQTTCLEAN" // close MQTT connection
#define AT_RFPOWER        "AT+RFPOWER" // set RF power

#define MQTTSUBRECV       "MQTTSUBRECV" // MQTT sub received

char ESP8266_Buffer[ESP8266_BUFFER_SIZE];
const TickType_t timeout = 10000;

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
    print2uart(UART_ESP, "%s?\r\n", AT_CWMODE);

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

bool ESP8266_enableSysLog(void)
{
    print2uart(UART_ESP, "%s=1\r\n", AT_SYSLOG);

    return waitForResponse("OK");
}

bool ESP8266_getSysLog(void)
{
    print2uart(UART_ESP, "%s?\r\n", AT_SYSLOG);

    return waitForResponse("OK");
}

bool ESP8266_getDHCPStatus(void)
{
    print2uart(UART_ESP, "%s?\r\n", AT_CWDHCP);

    return waitForResponse("OK");
}

bool ESP8266_setDHCP(int mode, int en)
{
    print2uart(UART_ESP, "%s=%d,%d\r\n", AT_CWDHCP, mode, en);

    return waitForResponse("OK");
}

bool ESP8266_connectToAP(char *SSID, char *password, int reconn)
{
    print2uart(UART_ESP, "%s=\"%s\",\"%s\",,,%d,,\r\n", AT_CWJAP, SSID, password, reconn);
    
    return waitForResponse("OK");
}

bool ESP8266_getAPConnectStatus(void)
{
    print2uart(UART_ESP, "%s?\r\n", AT_CWJAP);

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

bool ESP8266_sendData(char *data, unsigned int dataSize)
{
    print2uart(UART_ESP, "%s=%d\r\n", AT_CIPSEND, dataSize);

    if (!waitForResponse(">")) {
        return false;
    }

    print2uart(UART_ESP, data);

    return waitForResponse("OK");
}

bool ESP8266_getRFPower(void)
{
    print2uart(UART_ESP, "%s?\r\n", AT_RFPOWER);

    return waitForResponse("OK");
}

bool ESP8266_setRFPower(int dbm)
{
    print2uart(UART_ESP, "%s=%d\r\n", AT_RFPOWER, dbm);

    return waitForResponse("OK");
}

bool ESP8266_setMQTTUserConf(int scheme, char *clientID, char *username, char *passwd, char *path)
{
    print2uart(UART_ESP, "%s=0,%d,\"%s\",\"%s\",\"%s\",0,0,\"%s\"\r\n", AT_MQTTUSERCFG, scheme, clientID, username, passwd, path);

    return waitForResponse("OK");
}

bool ESP8266_getMQTTUserConf(void)
{
    print2uart(UART_ESP, "%s?\r\n", AT_MQTTUSERCFG);

    return waitForResponse("OK");
}

bool ESP8266_connectToMQTTBroker(char *serverIP, int port, int reconnect)
{
    print2uart(UART_ESP, "%s=0,\"%s\",%d,%d\r\n", AT_MQTTCONN, serverIP, port, reconnect);

    return waitForResponse("OK");
}

bool ESP8266_getMQTTConnectStatus(void)
{
    print2uart(UART_ESP, "%s?\r\n", AT_MQTTCONN);

    return waitForResponse("OK");
}

bool ESP8266_publishMessage(char *topic, char *data, int qos, int retain)
{
    print2uart(UART_ESP, "%s=0,\"%s\",\"%s\",%d,%d\r\n", AT_MQTTPUB, topic, data, qos, retain);

    return waitForResponse("OK");
}

bool ESP8266_subscribeTopic(char *topic, int qos)
{
    print2uart(UART_ESP, "%s=0,\"%s\", %d", AT_MQTTSUB, topic, qos);

    return waitForResponse("OK");
}

bool ESP8266_MQTTClean(void)
{
    print2uart(UART_ESP, "%s=0\r\n", AT_MQTTCLEAN);

    return waitForResponse("OK");
}

bool ESP8266_getMQTTRecvMessage(void)
{
    if (strstr(ESP8266_Buffer, "_END_") == NULL) {
        if (!waitForResponse("_END_")) {
            return false;
        }
    }

    return true;
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