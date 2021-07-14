#ifndef TCP_H_
#define TCP_H_

void constructData(char *data);
unsigned int constructPOSTRequest(char *endPoint, char *data, char *requestBody);
bool sendTCP(char *requestBody, unsigned int requestLength);
bool sendHTTPData(char *path, char *data);

#endif