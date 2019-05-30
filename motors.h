#ifndef MOTORS_H_
#define MOTORS_H_


#include <stdint.h>


// Sets the frequency and duty cycle of the PWM for the main motor
void
setMainPWM(uint32_t ui32Freq, uint32_t ui32Duty);


// Sets the frequency and duty cycle of the PWM for the tail motor
void
setTailPWM(uint32_t ui32Freq, uint32_t ui32Duty);


// Returns the rounded duty cycle of the main motor
uint16_t
GetMainDuty(void);


// Returns the rounded duty cycle of the tail motor
uint16_t
GetTailDuty(void);


// Initialises the PWM for the main motor
void
initialiseMainPWM(void);


// Initialises the PWM for the tail motor
void
initialiseTailPWM(void);


// Enables the PWM output for the main motor
void
activateMainPWM(void);


// Enables the PWM output for the tail motor
void
activateTailPWM(void);


// Disables the PWM output for the main motor
void
deactivateMainPWM(void);


// Disables the PWM output for the tail rotor
void
deactivateTailPWM(void);


#endif /* MOTORS_H_ */
