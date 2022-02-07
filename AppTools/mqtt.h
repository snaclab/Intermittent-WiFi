#ifndef MQTT_H_
#define MQTT_H_

bool checkMQTTClientConfig(void);
bool setupMQTTClientConfig(void);
bool connectToBroker(void);
bool checkMQTTConnection(void);
bool sendMQTTData(char *data);
bool subScribeTopic(char *topic);
bool getMQTTRecvMessage(char *topic, char *recvData);

#endif