#ifndef ALTITUDE_H_
#define ALTITUDE_H_


#include <stdint.h>


void ADCIntHandler(void);


void initADC (void);


void ProcessAltData(void);


void initADCCircBuf(void);


void SetLandedVal(void);


int32_t RetMeanVal(void);


int8_t getAlt(void);


#endif /* ALTITUDE_H_ */
