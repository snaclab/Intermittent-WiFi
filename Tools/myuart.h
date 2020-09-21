#ifndef UART_DRIVER_LIB_H_
#define UART_DRIVER_LIB_H_

#include <stdint.h>

#include "driverlib.h"

#define UARTA3_BUFFERSIZE 512
#define UART_NULL_BASE 0

extern unsigned int FreqLevel;
extern int uartA0setup;
extern int uartA3setup;

//#define DEBUG 1

// Init serial
void uartInit(unsigned int UART);
// Serial printf
void print2uart(unsigned int UART, char* format,...);
// dummy function
void dummyPrint(unsigned int UART, char* format,...);
// Serial read from Uart
void readFromUartA3(unsigned char *data, unsigned int size);
// Check whether device got message
bool uartA3GotMessage();
// Flush Uart
void uartFlush();

// Serial printf for debugging
#ifdef DEBUG
static void (*dprint2uart)(unsigned int UART, char* format,...) = print2uart;
#else
static void (*dprint2uart)(unsigned int UART, char* format,...) = dummyPrint;
#endif

//void dprint2uart(char* format,...);
//Put a string to serial
void print2uartLength(unsigned int UART, char* str,int length);
//Convert integer to a string
char *convert(unsigned int num, int base);
//Convert long integer to a string
char *convertl(unsigned long num, int base);

#endif
