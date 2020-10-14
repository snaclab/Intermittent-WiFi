/*
 * myserial.c
 *
 *  Created on: 2017�~5��25��
 *      Author: Meenchen
 */
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

#include "driverlib.h"
#include "FreeRTOS.h"
#include "Tools/dvfs.h"
#include "Tools/myuart.h"

// Implement a circular queue
volatile unsigned char UARTA3Data[UARTA3_BUFFERSIZE];
volatile unsigned int UARTA3ReadIndex;
volatile unsigned int UARTA3WriteIndex;

#define UARTA3_INCREMENT_READ_INDEX     __disable_interrupt(); UARTA3ReadIndex = (UARTA3ReadIndex + 1) % UARTA3_BUFFERSIZE; __enable_interrupt();
#define UARTA3_INCREMENT_WRITE_INDEX    UARTA3WriteIndex = (UARTA3WriteIndex + 1) % UARTA3_BUFFERSIZE
#define UARTA3_BUFFER_IS_EMPTY          (UARTA3WriteIndex - UARTA3ReadIndex) == 0 ? true : false
#define UARTA3_BUFFER_IS_FULL           (UARTA3WriteIndex + 1) % UARTA3_BUFFERSIZE == UARTA3ReadIndex ? true : false


// The following structure will configure the EUSCI_A port to run at 9600 baud from an 1~16MHz ACLK
// The baud rate values were calculated at: http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
EUSCI_A_UART_initParam UartParams[8] = {
{//1MHz
    EUSCI_A_UART_CLOCKSOURCE_SMCLK,
    6,                                                                         // clockPrescalar
    8,                                                                         // firstModReg
    17,                                                                        // secondModReg
    EUSCI_A_UART_NO_PARITY,
    EUSCI_A_UART_LSB_FIRST,
    EUSCI_A_UART_ONE_STOP_BIT,
    EUSCI_A_UART_MODE,
    EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
},{//2.66MHz
    EUSCI_A_UART_CLOCKSOURCE_SMCLK,
    17,                                                                        // clockPrescalar
    5,                                                                         // firstModReg
    2,                                                                         // secondModReg
    EUSCI_A_UART_NO_PARITY,
    EUSCI_A_UART_LSB_FIRST,
    EUSCI_A_UART_ONE_STOP_BIT,
    EUSCI_A_UART_MODE,
    EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
},{//3.5MHz
   EUSCI_A_UART_CLOCKSOURCE_SMCLK,
   22,                                                                         // clockPrescalar
   12,                                                                         // firstModReg
   107,                                                                        // secondModReg
   EUSCI_A_UART_NO_PARITY,
   EUSCI_A_UART_LSB_FIRST,
   EUSCI_A_UART_ONE_STOP_BIT,
   EUSCI_A_UART_MODE,
   EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
},{//4MHz
   EUSCI_A_UART_CLOCKSOURCE_SMCLK,
   26,                                                                         // clockPrescalar
   0,                                                                          // firstModReg
   214,                                                                        // secondModReg
   EUSCI_A_UART_NO_PARITY,
   EUSCI_A_UART_LSB_FIRST,
   EUSCI_A_UART_ONE_STOP_BIT,
   EUSCI_A_UART_MODE,
   EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
},{//5.33MHz
   EUSCI_A_UART_CLOCKSOURCE_SMCLK,
   34,                                                                         // clockPrescalar
   11,                                                                         // firstModReg
   17,                                                                         // secondModReg
   EUSCI_A_UART_NO_PARITY,
   EUSCI_A_UART_LSB_FIRST,
   EUSCI_A_UART_ONE_STOP_BIT,
   EUSCI_A_UART_MODE,
   EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
},{//7MHz
   EUSCI_A_UART_CLOCKSOURCE_SMCLK,
   45,                                                                         // clockPrescalar
   9,                                                                          // firstModReg
   17,                                                                         // secondModReg
   EUSCI_A_UART_NO_PARITY,
   EUSCI_A_UART_LSB_FIRST,
   EUSCI_A_UART_ONE_STOP_BIT,
   EUSCI_A_UART_MODE,
   EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
},{//8MHz
   EUSCI_A_UART_CLOCKSOURCE_SMCLK,
   52,                                                                         // clockPrescalar
   1,                                                                          // firstModReg
   73,                                                                         // secondModReg
   EUSCI_A_UART_NO_PARITY,
   EUSCI_A_UART_LSB_FIRST,
   EUSCI_A_UART_ONE_STOP_BIT,
   EUSCI_A_UART_MODE,
   EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
},{//16MHz
   EUSCI_A_UART_CLOCKSOURCE_SMCLK,
   104,                                                                        // clockPrescalar
   2,                                                                          // firstModReg
   182,                                                                        // secondModReg
   EUSCI_A_UART_NO_PARITY,
   EUSCI_A_UART_LSB_FIRST,
   EUSCI_A_UART_ONE_STOP_BIT,
   EUSCI_A_UART_MODE,
   EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
}};

EUSCI_A_UART_initParam ESP_UART_Config = 
{
   EUSCI_A_UART_CLOCKSOURCE_SMCLK,
   8,                                                                        // clockPrescalar
   10,                                                                          // firstModReg
   247,                                                                        // secondModReg
   EUSCI_A_UART_NO_PARITY,
   EUSCI_A_UART_LSB_FIRST,
   EUSCI_A_UART_ONE_STOP_BIT,
   EUSCI_A_UART_MODE,
   EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
};

int isTimeSet = 0;

void print2uart(unsigned int UART, char* format,...)
{
    char *traverse;
    int i;
    unsigned long l;
    char *s;

    //Module 1: Initializing Myprintf's arguments
    va_list arg;
    va_start(arg, format);

    for(traverse = format; *traverse != '\0'; traverse++)
    {
        while( *traverse != '%' && *traverse != '\0' )
        {
            EUSCI_A_UART_transmitData(UART, (uint8_t)*traverse);
            traverse++;
        }

        if(*traverse == '\0')
            break;

        traverse++;

        //Module 2: Fetching and executing arguments
        switch(*traverse)
        {
            case 'c' :
                i = va_arg(arg,int);        //Fetch char argument
                EUSCI_A_UART_transmitData(UART, (uint8_t)i);
                break;
            case 'l' :
                l = va_arg(arg,unsigned long);        //Fetch Decimal/Integer argument
                print2uart(UART, convertl(l,10));
                break;
            case 'd' :
                i = va_arg(arg,int);        //Fetch Decimal/Integer argument
                if(i<0)
                {
                    i = -i;
                    EUSCI_A_UART_transmitData(UART, (uint8_t)'-');
                }
                print2uart(UART, convert(i,10));
                break;
            case 's':
                s = va_arg(arg,char *);         //Fetch string
                print2uart(UART, s);
                break;
            case 'x':
                i = va_arg(arg,unsigned int); //Fetch Hexadecimal representation
                print2uart(UART, convert(i,16));
                break;
        }
    }
    //Module 3: Closing argument list to necessary clean-up
    va_end(arg);
}

void dummyPrint(unsigned int UART, char* format,...)
{
    return;
}

unsigned char readFromUartA3()
{
    unsigned char c;
    c = UARTA3Data[UARTA3ReadIndex];
    UARTA3_INCREMENT_READ_INDEX;

    return c;
}

bool uartA3GotMessage()
{
    return !UARTA3_BUFFER_IS_EMPTY;
}

void uartBufferFlush()
{
    UARTA3ReadIndex = UARTA3WriteIndex = 0;
}

// UART A3 Interrumpt
#pragma vector = EUSCI_A3_VECTOR
__interrupt void EUSCIA3_ISR(void)
{
    unsigned char c;
    unsigned char status = EUSCI_A_UART_getInterruptStatus(UART_ESP,
                                                           EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG);
    EUSCI_A_UART_clearInterrupt(UART_ESP, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG);

    if (status == EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) {
        c = EUSCI_A_UART_receiveData(UART_ESP);

        if (UARTA3_BUFFER_IS_FULL) {
            // TODO: Handle overflow, try buffer overflow
            dprint2uart(UART_STDOUT, "uart a3 buffer is full.\r\n");
        } else {
            UARTA3Data[UARTA3WriteIndex] = c;
            UARTA3_INCREMENT_WRITE_INDEX;
        }
    }
}

void print2uartLength(unsigned int UART, char* str, int length)
{
    int i;

    for(i = 0; i < length; i++)
    {
        EUSCI_A_UART_transmitData(UART, (uint8_t)*(str+i));
    }
}

char *convert(unsigned int num, int base)
{
    static char Representation[]= "0123456789ABCDEF";
    static char buffer[50];
    char *ptr;

    ptr = &buffer[49];
    *ptr = '\0';

    do
    {
        *--ptr = Representation[num%base];
        num /= base;
    }while(num != 0);

    return(ptr);
}

char *convertl(unsigned long num, int base)
{
    static char Representation[]= "0123456789ABCDEF";
    static char buffer[50];
    char *ptr;

    ptr = &buffer[49];
    *ptr = '\0';

    do
    {
        *--ptr = Representation[num%base];
        num /= base;
    }while(num != 0);

    return(ptr);
}

/* Initialize serial */
void uartInit(unsigned int UART) {
    // Configure UART
    
    switch(UART) {
        case UART_STDOUT:
            if(uartA0setup == 0) {
                EUSCI_A_UART_initParam param = UartParams[FreqLevel-1];
                if(STATUS_FAIL == EUSCI_A_UART_init(UART_STDOUT, &param))
                    return;

                EUSCI_A_UART_enable(UART_STDOUT);

                EUSCI_A_UART_clearInterrupt(UART_STDOUT,
                                            EUSCI_A_UART_RECEIVE_INTERRUPT);

                // Enable USCI_A0 RX interrupt
                EUSCI_A_UART_enableInterrupt(UART_STDOUT,
                                             EUSCI_A_UART_RECEIVE_INTERRUPT); // Enable interrupt

                // Select UART TXD on P2.0
                GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN0, GPIO_SECONDARY_MODULE_FUNCTION);
                
                // Enable global interrupt
                __enable_interrupt();
                
                uartA0setup = 1;
            }
            break;

        case UART_ESP:
            if (uartA3setup == 0) {
                EUSCI_A_UART_initParam param = ESP_UART_Config;
                
                if (STATUS_FAIL == EUSCI_A_UART_init(UART_ESP, &param))
                    return;

                EUSCI_A_UART_enable(UART_ESP);

                EUSCI_A_UART_clearInterrupt(UART_ESP,
                                            EUSCI_A_UART_RECEIVE_INTERRUPT);

                // Enable USCI_A3 RX interrupt
                EUSCI_A_UART_enableInterrupt(UART_ESP,
                                             EUSCI_A_UART_RECEIVE_INTERRUPT);

                // Select UART TXD for UART_ESP, P6.0
                GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P6, GPIO_PIN0, GPIO_PRIMARY_MODULE_FUNCTION);
                // Select UART RXD for UART_ESP, P6.1
                GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN1, GPIO_PRIMARY_MODULE_FUNCTION);

                // Enable global interrupt, not sure if there's problem to enable twice
                __enable_interrupt();
                
                uartA3setup = 1;
            }
            break;

        default:
            break;
    }
}

//DEBUG
char *printUARTBuffer(void)
{
    return UARTA3Data;
}
//End DEBUG
