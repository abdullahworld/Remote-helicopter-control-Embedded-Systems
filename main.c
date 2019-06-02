/*  main.c - A program that controls a model helicopter. */

/*  Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Daniel Page
    Last modified: 1.6.2019
    Based on ADCdemo1.c by P.J. Bones UCECE */

/*  Inputs: PE4 (Altitude), PB0 (Channel A), PB1 (Channel B)
    Outputs: PC5 (PWM Main), PF1 (PWM Tail) */

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
#include "buttons.h"
#include "altitude.h"
#include "yaw.h"
#include "display.h"
#include "motors.h"
#include "switch.h"
#include "uart.h"
#include "control.h"

/* Constant */
#define SYS_TICK_RATE 100 // [Hz]

/* Global variable */
uint32_t g_ulSampCnt; // Counter for the interrupts


/* The interrupt handler for the for SysTick interrupt. */
void
SysTickIntHandler(void) {

    ADCProcessorTrigger(ADC0_BASE, 3); // Initiate a conversion

    piMainUpdate();

    piTailUpdate();

    updateButtons();

    updateSwitch();

    incrementDispTimer(); // Updates the OLED display timer

    g_ulSampCnt++; // Increment counter until it reaches its limit and sits the flag to remove user system delay.
}

/* Initialisation functions for the clock (incl. SysTick), ADC, display */
void
initClock(void) {
    // Set the clock rate to 20 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
    // Set the PWM clock rate (using the prescaler)
}

/* Initialises the SysTick Timer */
void
initSysTick(void) {
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

/* Initialises all of the peripherals and processes */
void
initAll(void) {
    initClock();
    initialiseUSB_UART();
    initialiseMainPWM();
    initialiseTailPWM();
    initADC();
    initADCCircBuf();
    initResetBut();
    initButtons(); // Initialises 4 pushbuttons (UP, DOWN, LEFT, RIGHT)
    initSwitch();
    initDisplay();
    initYawRef();
    initYawGPIO();
    initSysTick();
    IntMasterEnable(); // Enable interrupts to the processor.
}

/* Main loop using a round robin approach */
int
main(void){
    initAll();
    while (1){
        if (g_ulSampCnt > 0){ // Set approximately to <SYS_TICK_RATE

            // Data processing
            ProcessAltData();
            // Checking program states
            landingSet();
            landedCheck();
            refPulse();

            // Polling peripherals
            buttonPressed(); // Up, down, left, right
            switched();
            buttonReset();

            // OLEDD Display function and UART
            displayStats(); // Updates the stats for a different line every loop on the OLED screen
            consoleMsgSpaced();

            // Resets sample count
            g_ulSampCnt = 0;
        }
    }
}