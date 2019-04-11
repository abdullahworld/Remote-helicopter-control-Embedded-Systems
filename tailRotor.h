#ifndef TAILROTOR_H_
#define TAILROTOR_H_


#include <stdint.h>


void setTailPWM(uint32_t ui32Freq, uint32_t ui32Duty);


void initialiseTailPWM(void);


uint32_t TailRotFreq(void);


#endif /* TAILROTOR_H_ */
