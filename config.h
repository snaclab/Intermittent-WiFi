/*
 * config.h
 *
 *  Description: Configurations parameters
 */

#ifndef CONFIG_H_
#define CONFIG_H_

//use the ADC to set the low voltage detection parameters, please refer to our paper and your device manual to set the following two parameters
#define ADC_MONITOR_THRESHOLD 2.4 //the operating voltage of the used capacitor
#define ADC_MONITOR_THRESHOLD_GAP 0.1 //The maximum required energy that is sufficient to run the device before switching out a task

#define DEBUGOVERFLOW //take it out for fast recovery

#define NUMTASK   5 //3 user tasks + 2 FreeRTOS tasks
#define MAXREAD   8

//Task ID
#define IDIDLE    0
#define IDTIMER   1
#define IDMATMUL  2
#define IDMATH32  3
#define IDWIFI    4

//WiFi
// #define AP_SSID         "User"
// #define AP_PASSWORD     "0975222283"
// #define STATIC_IP       "192.168.50.23"
// #define SERVER_IP       "192.168.50.2"
// #define SERVER_PORT     "9000"
// #define AP_SSID            "LinIPHONE"
// #define AP_PASSWORD        "0978637728"
// #define STATIC_IP          "172.20.10.24"
// #define SERVER_IP          "172.20.10.2"
#define AP_SSID            "CITI_Guest"
#define AP_PASSWORD        ""
#define STATIC_IP          "192.168.49.123"
#define SERVER_IP          "192.168.49.183"
#define SERVER_PORT        "9000"

// MQTT config
#define MQTT_CLIENT_ID     "ESP8266"
#define MQTT_TOPIC         "ESP"
#define MQTT_SCHEME        1 // 1 for connect MQTT through TCP
#define MQTT_PORT          1883
#define MQTT_RECONNECT     0 // 0: MQTT will not auto-reconnect, 1: MQTT will auto-reconnect
#define MQTT_QOS           1 // could be 0, 1, and 2, default is 0
#define MQTT_RETAIN        0 // Retain flag

#endif /* CONFIG_H_ */
