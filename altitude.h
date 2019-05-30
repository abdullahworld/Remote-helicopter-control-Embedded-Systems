#ifndef ALTITUDE_H_
#define ALTITUDE_H_


#include <stdint.h>


/* The handler for the ADC conversion complete interrupt.
   Writes to the circular buffer */
void
ADCIntHandler(void);


// Enables and configures ADC
void
initADC (void);


// Calculates the average altitude reading from the circular buffer and sets the landed value
void
ProcessAltData(void);


// Initalises the circular buffer for altitude readings
void 
initADCCircBuf(void);


// Returns the rounded current altitude
int16_t 
getAlt(void);


#endif /* ALTITUDE_H_ */
