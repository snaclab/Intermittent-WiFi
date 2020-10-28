#ifndef DEVICES_ESP8266_H_
#define DEVICES_ESP8266_H_

#include "FreeRTOS.h"

#include "driverlib.h"

#define ESP8266_BUFFER_SIZE    1024

#define TCP                    0
#define UDP                    1

// typedef CONNECTION_INFO {
//     blabla
//     blalba
// } ConnectionInfo;

bool ESP8266_getCurrentWiFiMode(void);
bool ESP8266_changeWiFiMode(unsigned int MODE);
bool ESP8266_checkModule(void);
bool ESP8266_getSystemInfo(void);
bool ESP8266_availableAPs(void);
bool ESP8266_getDHCPStatus(void);
bool ESP8266_setDHCP(int mode, int en);
bool ESP8266_connectToAP(char *SSID, char *password);
void ESP8266_ping(char *url);
bool ESP8266_disconnectFromAP(void);
bool ESP8266_establishTCPConnection(unsigned char type, char *address, char *port);
bool ESP8266_disconnectServer(char *linkID);
bool ESP8266_getConnectStatus(void);
bool ESP8266_enableMultipleConnecitons(bool enable);
bool ESP8266_sendData(char *data, unsigned int dataSize);
bool ESP8266_setStaticIP(char *IP);
bool ESP8266_getIP(void);

void ESP8266_hardReset(void);

char *ESP8266_getBuffer(void);


#endif // DEVCIES_ESP8266_H_
