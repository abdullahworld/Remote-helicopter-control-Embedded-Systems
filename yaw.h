#ifndef YAW_H_
#define YAW_H_


#include <stdint.h>


void YawIntHandler(void);


void initYawGPIO(void);


int16_t getYaw(void);


void YawRefIntHandler(void);


void initYawRef(void);


void checkYawRef(void);


void findRef(void);


#endif /* YAW_H_ */
