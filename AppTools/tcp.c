#include <stdio.h>
#include <string.h>

#include "Tools/myuart.h"
#include "Devices/ESP8266.h"
#include "FreeRTOS.h"
#include "config.h"

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

bool sendHTTPData(char *path, char *data)
{
    char *ESP_Data = ESP8266_getBuffer();
    int dataLength = strlen(data);
    char url[] = "http://192.168.50.110:9000/ESP/1";
    // sprintf(url, "http://%s:%s/%s\0", SERVER_IP, SERVER_PORT, path);
    TickType_t start = xTaskGetTickCount();
    if (!ESP8266_sendHTTPData(url, data, dataLength)) {
        dprint2uart(UART_STDOUT,
                    "Fail to send HTTP data with response\r\n%s\r\n", ESP_Data);
        return false;
    }
    TickType_t end = xTaskGetTickCount();
    dprint2uart(UART_STDOUT,
                "Overall HTTP send time elapse: %d\r\n", (end - start));

    return true;
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
