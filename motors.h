#ifndef MOTORS_H_
#define MOTORS_H_


#include <stdint.h>


void initialiseMainPWM(void);


void initialiseTailPWM(void);


void setMainPWM(uint32_t ui32Freq, uint32_t ui32Duty);


void setTailPWM(uint32_t ui32Freq, uint32_t ui32Duty);


void activateMainPWM(void);


void activateTailPWM(void);


void deactivateMainPWM(void);


void deactivateTailPWM(void);


#endif /* MOTORS_H_ */
