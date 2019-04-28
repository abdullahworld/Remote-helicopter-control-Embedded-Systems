// main.c - An interrupt driven program that measures the height of the helicopter

// Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Daniel Page
// Last modified: 28.4.2019

// Based on ADCdemo1.c by P.J. Bones UCECE
// Based on the 'convert' series from 2016

// Inputs: PE4 (Altitude), PB0 (Channel A), PB1 (Channel B)
// Outputs: PC5 (PWM Main), PF1 (PWM Tail)


#include <buttons.h>
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
#include "altitude.h"
#include "yaw.h"
#include "display.h"
#include "motors.h"
#include "switch.h"
#include "uart.h"
#include "control.h"


// Constant
#define SYS_TICK_RATE 200


// Sets variable
static uint32_t ticksCount;    // Counter for the interrupts


// The interrupt handler for the for SysTick interrupt.
void
SysTickIntHandler(void)
{
    // Check for button changes
    updateButtons();
    updateSwitch();

    // Pulses the PWM of the tail motor to find the reference point
    // Only occurs while in the initialising state
    refPulse();

    // Updates control for the main and tail rotors using PI control
    // Only occurs while in the flying state
    piMainUpdate();
    piTailUpdate();

    // Initiate an ADC conversion
    ADCProcessorTrigger(ADC0_BASE, 3);

    ticksCount++; // Counts the number of system ticks
}


// Initialisation of the clock
void
initClock(void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
}


void
initSysTick(void)
{
    // Set up the period for the SysTick timer
    SysTickPeriodSet(SysCtlClockGet() / SYS_TICK_RATE);

    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);

    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}


// Initialises all of the peripherals and processes
void
initAll(void)
{
    initClock();
    initialiseUSB_UART();
    initialiseMainPWM();
    initialiseTailPWM();
    initADC();
    initADCCircBuf();
    initResetBut();
    SysCtlPeripheralReset(LEFT_BUT_PERIPH);
    SysCtlPeripheralReset(UP_BUT_PERIPH);
    initButtons();  // Initialises 4 pushbuttons (UP, DOWN, LEFT, RIGHT)
    initSwitch();
    initDisplay();
    initYawRef();
    initYawGPIO();
    initSysTick();
    IntMasterEnable(); // Enable interrupts to the processor.
}


// Calls the initAll() function and runs the background task loop
int
main(void)
{
	initAll();
	while (1)
	{
	    if (ticksCount > 0) { // Loop set to approximately to the system tick rate
	        ProcessAltData();
	        displayStats();
	        buttonUp();
	        buttonDown();
	        buttonLeft();
	        buttonRight();
	        switched();
	        buttonReset();
	        ticksCount = 0;
            consoleMsgSpaced();
	    }
	}
}
