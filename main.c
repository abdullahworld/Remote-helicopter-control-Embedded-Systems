//*****************************************************************************
//
// main.c - An interrupt driven program that measures the height of the helicopter
//
// Authors: Hassan Ali Alhujhoj, Abdullah Naeem and Daniel Page
// Last modified: 12.4.2019
//
//*****************************************************************************
// Based on ADCdemo1.c by P.J. Bones UCECE
// Based on the 'convert' series from 2016
//*****************************************************************************
//
// Inputs: PE4 (Altitude), PB0 (Channel A), PB1 (Channel B)
// Outputs: PC5 (PWM Main), PF1 (PWM Tail)
//


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "circBufT.h"
#include "buttons4.h"
#include "altitude.h"
#include "yaw.h"
#include "display.h"
#include "motors.h"
#include "switch.h"
#include "uart.h"


//*****************************************************************************
// Constant
//*****************************************************************************
#define SYS_TICK_RATE 100


//*****************************************************************************
// Global variables
//*****************************************************************************
uint32_t g_ulSampCnt;    // Counter for the interrupts
uint32_t R_g_ulSampCnt;


//*****************************************************************************
// The interrupt handler for the for SysTick interrupt.
//*****************************************************************************
void SysTickIntHandler(void)
{
    updateButtons();
    updateSwitch();
    checkYawRef();
    // Initiate a conversion
    ADCProcessorTrigger(ADC0_BASE, 3); 
    g_ulSampCnt++;
}


//*****************************************************************************
// Initialisation functions for the clock (incl. SysTick), ADC, display
//*****************************************************************************
void initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
    // Set the PWM clock rate (using the prescaler)

}


void initSysTick(void) {
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SYS_TICK_RATE);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}


void buttonUp(void) {
    if (checkButton(UP) == PUSHED) {
        incrMainPWM();
    }
}


void buttonDown(void) {
    if (checkButton(DOWN) == PUSHED) {
        decrMainPWM();
    }
}


void buttonLeft(void) {
    if (checkButton(LEFT) == PUSHED) {
        decrTailPWM();
    }
}


void buttonRight(void) {
    if (checkButton(RIGHT) == PUSHED) {
        incrTailPWM();
    }
}


void switched(void) {
    if (checkSwitch() > 0) {
        findRef();
    }
}


void initAll(void) {
    SysCtlPeripheralReset(LEFT_BUT_PERIPH);
    SysCtlPeripheralReset(UP_BUT_PERIPH);
    initClock();
    initialiseUSB_UART();
    initialiseMainPWM();
    initialiseTailPWM();
    initADC();
    initADCCircBuf();
    initButtons();  // Initialises 4 pushbuttons (UP, DOWN, LEFT, RIGHT)
    initSwitch();
    initDisplay();
    initYawRef();
    initYawGPIO();
    initSysTick();
    // Enable interrupts to the processor.
    IntMasterEnable();
}


int main(void) {
	initAll();
	while (1)
	{
	    if (g_ulSampCnt > 0) { // Set to approximately 100 Hz
	        ProcessAltData();
	        displayStats();
	        buttonUp();
	        buttonDown();
	        buttonLeft();
	        buttonRight();
	        switched();
            g_ulSampCnt = 0;
            consoleMsgSpaced();
	    }
	}
}
