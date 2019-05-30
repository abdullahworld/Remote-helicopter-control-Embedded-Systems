#ifndef YAW_H_
#define YAW_H_


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
