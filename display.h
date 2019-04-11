#ifndef DISPLAY_H_
#define DISPLAY_H_


#include <stdint.h>


void initDisplay(void);


void displayMeanVal(int16_t meanVal, int32_t count);


void displayStats(int32_t altitude, int32_t yaw);


void SetDispValues(uint32_t g_ulSampCnt);


void changeDispState(void);


#endif /* DISPLAY_H_ */
