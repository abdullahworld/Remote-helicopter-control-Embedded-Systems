#ifndef MAINROTOR_H_
#define MAINROTOR_H_


#include <stdint.h>


void setMainPWM(uint32_t ui32Freq, uint32_t ui32Duty);


void initialiseMainPWM(void);


void SetPWMClock(void);


uint32_t MainRotDuty(void);


void increaseMainDuty(void);


#endif /* MAINROTOR_H_ */
