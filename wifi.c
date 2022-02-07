
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "driverlib.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "TaskManager/taskManager.h"
#include "DataManager/SimpDB.h"
#include "Devices/ESP8266.h"
#include "AppTools/mqtt.h"
#include "AppTools/tcp.h"
#include "Tools/myuart.h"
#include "config.h"
#include "wifi.h"

#pragma NOINIT(subMessage)
char subMessage[ 4000 ];

extern int DID0;
extern int DID1;
extern int SENDMQTT;

int RF_POWER = 78;
int Tries = 5;
int total_char = 22555;

// char message[] = "Hello World. Let's debug together. Why the hell the upper message cannot be sent";

// might need #pragma DATA_SECTION or NOINIT to make sure msgLength is stored at NVM.
int msgLength = 2048;
// char message[] = "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";
// char message[] = "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";
char message[] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";


TickType_t updatePeriod = 3000;

void wifiCommunicate(void);

void wifi(void)
{
    xTaskCreate(wifiCommunicate, "wifiCommunicate", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL, IDWIFI, INVM);
}

int mqttPublish(char *ESP_Data)
{
    if (!sendMQTTData("start")) {
        dprint2uart(UART_STDOUT,
                    "Fail to start sending data\r\n");
        return -2;
    }

    while (true) {
        unsigned long progressIDX = 0;
        
        dprint2uart(UART_STDOUT, "DID1: %d\r\n", DID1);

        if (DID1 >= 0) {
            DBreadIn(&progressIDX, DID1);
        }

        dprint2uart(UART_STDOUT, "progressIDX: %d\r\n", progressIDX);

        progressIDX = getSubString(subMessage, progressIDX, msgLength);

        if (!sendMQTTData(subMessage)) {
            dprint2uart(UART_STDOUT,
                        "Fail to send MQTT data\r\n");
            continue;
        }

        if (progressIDX >= total_char) {
            progressIDX = 0;
            struct working data;
            DBworking(&data, DID1);
            unsigned long* progressIdxPtr = data.address;
            *progressIdxPtr = progressIDX;
            DID1 = DBcommit(&data, 4, 1);

            break;
        }

        struct working data;
        DBworking(&data, DID1);
        unsigned long* progressIdxPtr = data.address;
        *progressIdxPtr = progressIDX;
        DID1 = DBcommit(&data, 4, 1);

        __delay_cycles(6400);
    }

    return -1;
}

void wifiCommunicate(void)
{
    registerTCB(IDWIFI);
    dprint2uart(UART_STDOUT, "Begin Wifi test\r\n");
    // dprint2uart(UART_STDOUT, "DID1: %d\r\n", DID1);

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
    
    if (!checkMQTTConnection()) {
        if (!connectToBroker()) {
            dprint2uart(UART_STDOUT,
                        "Fail to connect to MQTT broker\r\n");
            goto END;
        }
    }

    while (true) {
        dprint2uart(UART_STDOUT, "DID0: %d\r\n", DID0);

        if (DID0 >= 0) {
            DBreadIn(&SENDMQTT, DID0);
        }

        dprint2uart(UART_STDOUT, "SENDMQTT: %d\r\n", SENDMQTT);

        if (SENDMQTT == 1) {
            SENDMQTT = mqttPublish(ESP_Data);
        }

        if (SENDMQTT == 0) {
            break;
        }

        struct working data;
        DBworking(&data, DID0);
        unsigned int* sendMQTTPtr = data.address;
        *sendMQTTPtr = SENDMQTT;
        DID0 = DBcommit(&data, 4, 1);
        __delay_cycles(6400000);
    }

END:
    // ESP8266_disconnectFromAP();
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

bool initConnectModule(int tries)
{
    // Init UART Interface
    uartInit(UART_ESP);
    // init button interrupt
    initButtonInterrupt();
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

bool getDataLength(void)
{
    char strMsgLength[5];
    if (!getMQTTRecvMessage("DataLength/Device1", strMsgLength)) {
        dprint2uart(UART_STDOUT, "Fail to get data length.\r\n");
        return false;
    }
    sscanf(strMsgLength, "%d", &msgLength);

    return true;
}

unsigned long getSubString(char* subStr, unsigned long start, unsigned int length)
{
    unsigned int cnt = 0;
    // char sHead[] = "{\"D1\":\"";
    // char sFoot[] = "\"}";

    // strncpy(subStr, sHead, strlen(sHead));

    while (cnt < msgLength) {
        if (msgLength - cnt >= strlen(message)) {
            strncpy(subStr + cnt, message, strlen(message));
            cnt += strlen(message);
        } else {
            strncpy(subStr + cnt, message, msgLength - cnt);
            cnt = msgLength;
        }
    }

    // strncpy(subStr+ strlen(sHead) + cnt, sFoot, strlen(sFoot));
    // cnt = cnt + strlen(sHead) + strlen(sFoot);
    // int msg_ptr = start % strlen(message);
    // unsigned long cnt = (msg_ptr + length >= strlen(message)) ? (strlen(message)-msg_ptr) : length;
    // strncpy(subStr, message + msg_ptr, cnt);
    subStr[cnt] = '\0';

    return (start + msgLength);

    // strncpy(subStr, message, strlen(message));
    // subStr[strlen(message)+1] = '\0';

    // return (start + strlen(message));

}

void initButtonInterrupt(void)
{
	GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN5);
	GPIO_selectInterruptEdge(GPIO_PORT_P5, GPIO_PIN5, GPIO_HIGH_TO_LOW_TRANSITION);

	// Disable the GPIO power-on default high-impedance mode to activate previously configured port settings
    PMM_unlockLPM5();

	// Set all P5IFG to zero

    P5IFG = 0x00;

    GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN5);                                      // S2 P5.5: PxIE register

    __bis_SR_register(GIE);  // Enable all interrupts
}

#pragma vector=PORT5_VECTOR
__interrupt void Port_5(void)
{
	dprint2uart(UART_STDOUT, "In button interrupt.\r\n");

    switch (P5IFG)
    {
        // case 0b01000000: // S1 P5.6 = 64: toggle red LED
        // {
        //     P1OUT ^= BIT0;          // Toggle P1.0
        //     P5IFG &= ~BIT6;         // P5.6 clear interrupt flag
        // }
        // break; 
        case 0b00100000: // P5.5
        {
	        P5IFG &= ~BIT5; // P5.5 clear interrupt flag
            if (DID0 >= 0) {
                DBreadIn(&SENDMQTT, DID0);
            }
			
            SENDMQTT = 1;

            struct working data;
            DBworking(&data, DID0);
            unsigned long* sendMQTTPtr = data.address;
            *sendMQTTPtr = SENDMQTT;
            DID0 = DBcommit(&data, 4, 1);
            __delay_cycles(6400000);
        }
        break;
        default: // should not be here!
        break;
    }

}

