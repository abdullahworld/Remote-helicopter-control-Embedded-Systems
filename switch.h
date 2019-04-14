#ifndef SWITCH_H_
#define SWITCH_H_

#include <stdint.h>
#include <stdbool.h>


void initSwitch(void);


void updateSwitch(void);


bool checkSwitch(void);


char* getMode(void);


#endif /* SWITCH_H_ */
