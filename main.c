/*
 *  main.c
 *
 *  Description: This is a main function of program
 */
/* Scheduler include files. */
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <stdio.h>

#include <msp430fr5994.h>

#include "config.h"
#include "driverlib.h"
#include "RecoveryHandler/Recovery.h"
#include "Tools/myuart.h"
#include "Tools/hwsetup.h"
#include "TaskManager/taskManager.h"
#include "DataManager/SimpDB.h"
#include "main.h"
#include "demo.h"
#include "wifi.h"

//record some information for status of lengthy tasks
#pragma NOINIT(runHigh)
int runHigh;
#pragma NOINIT(runLow)
int runLow;
#pragma NOINIT(failCount)
int failCount;
#pragma NOINIT(lengthyFail)
int lengthyFail;
#pragma NOINIT(aboveFail)
int aboveFail;
//data id
#pragma NOINIT(DID0)
int DID0;
#pragma NOINIT(DID1)
int DID1;


/*
 * description: Initialize variables used for debugging and logging
 * parameters: none
 * return: none
 * note: These variables are not must needed, but help to debug the runtime, and this function should be called once
 * */
void intializeLOGVar()
{
    runLow = 0;
    runHigh = 0;
    failCount = 0;
    lengthyFail = 0;
    aboveFail = 0;
    timeCounter = 0;
    DID0 = -1;
    DID1 = -1;
    resetTasks();//no task is created before
    constructor();//init data structures of data manager
    pvInitHeapVar();//init variables for the NVM heap
    resetAllTasks();//all tasks are executed from the beginning
}

/*
 * description: turn on or turn off LED
*/
void ledOn(bool turnOn)
{
	P1DIR = BIT0;

	P1OUT = 0;

	if (turnOn) {
		// dprint2uart(UART_STDOUT, "led blink\r\n");
		unsigned int i = 1;
		while (i != 0) {
			P1OUT ^= BIT0;
			__delay_cycles(10000000);
		}
	} else {
		P1OUT = 0;
	}
}

/*
 * description: Main function of this program
 * parameters: none
 * return: none
 * note: Create your application tasks here using the xTackCreate() function
 *      e.g., xTaskCreate( your_function_name, "task_name", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL, taskID, INVM;
 *      *: taskID should be consistent with the setting in config.h
 * */
int main( void )
{
    /* Configure the hardware to run the demo. */
    prvSetupHardware();

	if(recoverable != 1){//run from the scratch
	    intializeLOGVar();

        //low voltage detector
	    initVDetector();

	    //create application tasks here
	    //demo();

		// ledOn(true);

		// create wifi
		wifi();

		// ledOn(false);

	    //start scheduler of freeRTOS
	    vTaskStartScheduler();
	}
	else{//system recovery
	    failCount++;//logging the time of power failures
		dprint2uart(UART_STDOUT, "MSP Restarted\r\n");

	    /* DEBUG: if the device dies before we trigger the low voltage interrupt, the voltage is not set properly */
	    if(voltage == ABOVE)//if we die before switch out
	        aboveFail++;

	    //low voltage detector
	    initVDetector();

	    //recover all tasks
	    failureRecovery();
	}
	return 0;
}




