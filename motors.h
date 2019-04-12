#ifndef MOTORS_H_
#define MOTORS_H_


#include <stdint.h>


void initialiseMainPWM(void);


void initialiseTailPWM(void);


void setMainPWM(uint32_t ui32Freq, uint32_t ui32Duty);


void setTailPWM(uint32_t ui32Freq, uint32_t ui32Duty);


uint32_t GetMainDuty(void);


uint32_t GetTailDuty(void);


void activateMainPWM(void);


void activateTailPWM(void);


void deactivateMainPWM(void);


void deactivateTailPWM(void);


void incrMainPWM(void);


void decrMainPWM(void);


void incrTailPWM(void);


void decrTailPWM(void);


#endif /* MOTORS_H_ */
