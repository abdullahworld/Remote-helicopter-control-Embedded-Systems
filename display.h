/*  display.h - Controls the OLED display, showing the stats of the helicopter. */

/*  Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Daniel Page
    Last modified: 1.6.2019 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

/* Initialise the Orbit OLED display */
void
initDisplay(void);

/* Increments the timer to show the next line on the OLED screen */
void
incrementDispTimer(void);

/* This display function will display each line depending on the timer that is updated based on sysTick
   The timer will be updated every 10ms and each timer count is equal to 10ms. Thus, the OLED will update every 40 ms. */
void
displayStats(void);

#endif /* DISPLAY_H_ */