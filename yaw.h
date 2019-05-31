#ifndef YAW_H_
#define YAW_H_


// yaw.h - Controls the yaw of the helicopter

// Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Daniel Page
// Last modified: 1.6.2019


#include <stdint.h>


// ISR for quadrature encoding
void
YawIntHandler(void);


// Initialises the GPIO pin register for yaw channels A and B
void
initYawGPIO(void);


// Returns the yaw in degrees with respect to the reference point
int16_t
getYaw(void);


// ISR for finding the yaw reference
void
YawRefIntHandler(void);


// Initialises the GPIO pin register for the yaw reference point
void
initYawRef(void);


#endif /* YAW_H_ */
