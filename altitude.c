//*****************************************************************************
//
// Altitude.c - An interrupt driven program that measures the height of the helicopter
//
// Authors: Hassan Ali Alhujhoj, Abdullah Naeem and Daniel Page
// Last modified: 4.4.2019
//
//*****************************************************************************
// Based on ADCdemo1.c by P.J. Bones UCECE
// Based on the 'convert' series from 2016
//*****************************************************************************


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
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "buttons4.h"
#include "altitude.h"


//*****************************************************************************
// Constants
//*****************************************************************************
#define BUF_SIZE 10 // Matches number of samples per second and enough will not significantly deviate
#define SAMPLE_RATE_HZ 40 // 25 samples per second assuming a jitter of 4Hz
#define VOLTAGE_SENSOR_RANGE 800 // in mV
#define SCREEN_ALTITUDE 1 // A screen state that shows the altitude of the helicopter as a percentage
#define SCREEN_MEAN_ADC 2 // A screen state that shows the mean ADC value and the number of samples
#define SCREEN_BLANK 3 // A screen state where everything on the display is wiped
#define COUNTER_RATE 2000 // In Hz (This does not include the delay from running the functions)


//*****************************************************************************
// Global variables
//*****************************************************************************
static circBuf_t g_inBuffer;		// Buffer of size BUF_SIZE integers (sample values)
static uint32_t g_ulSampCnt;	// Counter for the interrupts


//*****************************************************************************
// The interrupt handler for the for SysTick interrupt.
//*****************************************************************************
void SysTickIntHandler(void)
{
    // Initiate a conversion
    ADCProcessorTrigger(ADC0_BASE, 3); 
    g_ulSampCnt++;
}


//*****************************************************************************
//
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//
//*****************************************************************************
void ADCIntHandler(void)
{
	uint32_t ulValue;
	
	//
	// Get the single sample from ADC0.  ADC_BASE is defined in
	// inc/hw_memmap.h
	ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);
	//
	// Place it in the circular buffer (advancing write index)
	writeCircBuf (&g_inBuffer, ulValue);
	//
	// Clean up, clearing the interrupt
	ADCIntClear(ADC0_BASE, 3);                          
}


//*****************************************************************************
// Initialisation functions for the clock (incl. SysTick), ADC, display
//*****************************************************************************
void initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}


void initADC (void)
{
    //
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    
    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
  
    //
    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    // Set to pin PE4
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE |
                             ADC_CTL_END);
                             
    //
    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 3);
  
    //
    // Register the interrupt handler
    ADCIntRegister (ADC0_BASE, 3, ADCIntHandler);
  
    //
    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);
}


void initDisplay (void)
{
    // intialise the Orbit OLED display
    OLEDInitialise ();
}


//*****************************************************************************
//
// Function to display the mean ADC value (10-bit value, note) and sample count.
//
//*****************************************************************************
void displayMeanVal(int16_t meanVal, int32_t count)
{
	char string[17];  // 16 characters across the display

    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf (string, sizeof(string), "MEAN ADC = %4d", meanVal);
    // Update line on display.
    OLEDStringDraw (string, 0, 0);

    usnprintf (string, sizeof(string), "SAMPLE NO. %5d", count);
    OLEDStringDraw (string, 0, 1);
}


void displayStats(int32_t altitude, int32_t yaw)
{
    char string[17];  // 16 characters across the display

    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf (string, sizeof(string), "ALTITUDE: %4d%%", altitude);
    // Update line on display.
    OLEDStringDraw (string, 0, 0);
    usnprintf (string, sizeof(string), "YAW: %9d%%", yaw);
    // Update line on display.
    OLEDStringDraw (string, 0, 1);
}


int main(void)
{
	uint16_t i;
	int32_t sum;
	int32_t meanVal;
	int32_t helicopter_landed_value;
	int8_t screen_state = SCREEN_ALTITUDE;
	int8_t n = 0;
	int32_t counter = 0;
	int32_t altitude;
	int32_t yaw;

	SysCtlPeripheralReset (LEFT_BUT_PERIPH);
	SysCtlPeripheralReset (UP_BUT_PERIPH);
	initClock ();
	initADC ();
	initDisplay ();
	initButtons ();  // Initialises 4 pushbuttons (UP, DOWN, LEFT, RIGHT)
	initCircBuf (&g_inBuffer, BUF_SIZE);

    // Enable interrupts to the processor.
    IntMasterEnable();

	while (1)
	{
	    updateButtons();

	    if (counter == SysCtlClockGet()/COUNTER_RATE)
            {
	        if (checkButton(LEFT) == PUSHED) {
	                    helicopter_landed_value = meanVal;
	                } else if (checkButton(UP) == PUSHED) {
	                    OrbitOledClear();
	                    switch(screen_state) {
	                        case SCREEN_ALTITUDE:
	                        screen_state = SCREEN_MEAN_ADC;
	                            break;
	                        case SCREEN_MEAN_ADC:
	                        screen_state = SCREEN_BLANK;
	                            break;
	                        case SCREEN_BLANK:
	                        screen_state = SCREEN_ALTITUDE;
	                            break;
	                    }
	                }

                // Background task: calculate the (approximate) mean of the values in the
                // circular buffer and display it, together with the sample number.
                sum = 0;
                for (i = 0; i < BUF_SIZE; i++)
                    sum = sum + readCircBuf (&g_inBuffer);
                meanVal = (2 * sum + BUF_SIZE) / 2 / BUF_SIZE;

                // Creates a delay so there are values in the buffer to use for the landed value
                if (n == 2) {
                    helicopter_landed_value = meanVal;
                    n++;
                } else if (n < 2) {
                    n++;
                }

                if (screen_state == SCREEN_MEAN_ADC) {
                    // Calculates and display the rounded mean of the buffer contents
                    displayMeanVal (meanVal, g_ulSampCnt);
                } else if (screen_state == SCREEN_ALTITUDE) {
                    altitude = (2*(helicopter_landed_value-meanVal) + (VOLTAGE_SENSOR_RANGE/100))/2/(VOLTAGE_SENSOR_RANGE/100);
                    yaw = 0;
                    displayStats(altitude,yaw);
                 // max height v = -0.8v and min height v = 0v
                 // This adds 0.5 so the value is truncated to the right value: (2*x + y)/2/y = x/y + 0.5
                }
                counter = 0;
            }
	    counter++;
	}
}
