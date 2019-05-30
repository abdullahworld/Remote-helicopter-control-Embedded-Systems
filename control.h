#ifndef CONTROL_H_
#define CONTROL_H_


#include <stdint.h>


// Starts a routine to find the reference location where the helicopter faces the front
void
findRefStart(void);


// Changes the mode to flying once the reference point has been found
void
findRefStop(void);


// Changes the program to the landing state
void
modeLanding(void);


// Reset set values for altitude and yaw when the program is changed to the landing state
void
landingSet(void);


// Checks to see if the helicopter has landed. It turns of the motors and performs a reset when it is landed
void
landedCheck(void);


// Pulses the PWM of the tail motor to find the reference point
void
refPulse(void);


// Returns a string of the current mode
char*
getMode(void);


// Increases the set altitude by 10%
void
incrAlt(void);


// Decreases the set altitude by 10%
void
decrAlt(void);


// Increases the set yaw by 15 degrees
void
incrYaw(void);


// Decreases the set yaw by 15 degrees
void
decrYaw(void);


// Returns the current set altitude
uint8_t
getSetAlt(void);


// Returns the current set yaw
int16_t
getSetYaw(void);


// Updates the PI controller for the main motor based of the set position and the current position
void
piMainUpdate(void);


// Updates the PI controller for the tail motor based on the set position and the current position
void
piTailUpdate(void);


#endif /* CONTROL_H_ */
