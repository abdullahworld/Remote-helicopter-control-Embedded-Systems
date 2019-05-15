#ifndef CONTROL_H_
#define CONTROL_H_


#include <stdint.h>


void
findRefStart(void);


void
findRefStop(void);


void
modeLanding(void);


void
landingSet(void);


void
landedCheck(void);


void
refPulse(void);


char*
getMode(void);


void
incrAlt(void);


void
decrAlt(void);


void
incrYaw(void);


void
decrYaw(void);


uint8_t
getSetAlt(void);


int16_t
getSetYaw(void);


void
piMainUpdate(void);


void
piTailUpdate(void);


#endif /* CONTROL_H_ */
