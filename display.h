#ifndef DISPLAY_H_
#define DISPLAY_H_


#include <stdint.h>


// Initialise the Orbit OLED display
void
initDisplay(void);


void
displayStats(void);


int32_t
get_timer(void);


void
set_timer(void);


#endif /* DISPLAY_H_ */
