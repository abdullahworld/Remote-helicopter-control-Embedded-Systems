// altitude.c - Reads the altitude using an ADC conversion and the average of a circular buffer.

// Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Daniel Page
// Last modified: 1.6.2019

// Based on ADCdemo1.c by P.J. Bones UCECE


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "circBufT.h"
#include "altitude.h"


// Constants
#define BUF_SIZE 25 // Matches the number of samples per period of jitter, ensuring it will not significantly deviate
#define SAMPLE_RATE_HZ 100 // The sampling rate for altitude readings (well over the jitter of 4Hz)
#define VOLTAGE_SENSOR_RANGE 800 // The voltage range for the height sensor [mV]


// Sets variables
static uint16_t i;
static int8_t n = 0;
static int32_t sum;
static int32_t meanVal;
static int32_t helicopter_landed_value;
static circBuf_t g_inBuffer;
static int16_t altitude;


/* The handler for the ADC conversion complete interrupt.
   Writes to the circular buffer */
void ADCIntHandler(void) {
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


// Enables and configures ADC
void initADC (void) {
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

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

    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 3);

    // Register the interrupt handler
    ADCIntRegister (ADC0_BASE, 3, ADCIntHandler);

    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);
}


// Calculates the average altitude reading from the circular buffer and sets the landed value
void ProcessAltData(void) {
    // Background task: calculate the (approximate) mean of the values in the
    // circular buffer and display it, together with the sample number.
    sum = 0;
    for (i = 0; i < BUF_SIZE; i++) {
        sum = sum + readCircBuf (&g_inBuffer);
        meanVal = (2 * sum + BUF_SIZE) / 2 / BUF_SIZE;
    }

    // Creates a delay so there are values in the buffer to use for the landed value
    if (n == BUF_SIZE) {
        helicopter_landed_value = meanVal;
        n++;
    } else if (n < BUF_SIZE) {
        n++;
    }
}


// Initalises the circular buffer for altitude readings
void initADCCircBuf(void) {
    initCircBuf(&g_inBuffer, BUF_SIZE);
}


// Returns the rounded current altitude as an integer
int16_t getAlt(void) {
    altitude = ((100 * 2 * (helicopter_landed_value - meanVal) + VOLTAGE_SENSOR_RANGE)) / (2 * VOLTAGE_SENSOR_RANGE);
    // This adds 0.5 so truncation from floating point division is corrected
    // Based on the formula: (2*x + y)/2/y = x/y + 0.5
    return altitude;
}

