#ifndef YAW_H_
#define YAW_H_


#include <stdint.h>


void
YawIntHandler(void);


void
initYawGPIO(void);


int16_t
getYaw(void);


int16_t
getDispYaw(void);


void
YawRefIntHandler(void);


void
initYawRef(void);


#endif /* YAW_H_ */
