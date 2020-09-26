#ifndef DEVICES_ESP8266_H_
#define DEVICES_ESP8266_H_

#include <string.h>

#include "driverlib.h"

#define ESP8266_BUFFER_SIZE    1024
#define ESP8266_RECEIVE_TRIES  10

#define TCP                    0
#define UDP                    1

// typedef CONNECTION_INFO {
//     blabla
//     blalba
// } ConnectionInfo;

bool ESP8266_getCurrentWiFiMode(void);
bool ESP8266_changeWiFiMode(unsigned int MODE);
bool ESP8266_checkConnection(void);
bool ESP8266_getSystemInfo(void);
bool ESP8266_availableAPs(void);
bool ESP8266_reset(void);
bool ESP8266_connectToAP(char *SSID, char *password);
bool ESP8266_disconnectFromAP(void);
bool ESP8266_establishConnection(char id, unsigned char type, char *address, char *port);
bool ESP8266_enableMultipleConnecitons(bool enable);
bool ESP8266_sendData(char id, char *data, unsigned int dataSize);

char *ESP8266_getBuffer(void);

#endif // DEVCIES_ESP8266_H_