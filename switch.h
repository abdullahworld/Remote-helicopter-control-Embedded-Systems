#ifndef SWITCH_H_
#define SWITCH_H_


// switch.h - Functions for the switch on the ORBIT BoosterPack.

// Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Daniel Page
// Last modified: 1.6.2019


// Initialises the GPIO port for the switch
void
initSwitch(void);


// Updates the status of the switch in an ISR at SYS_TICK_RATE
void
updateSwitch(void);


// Switch logic to only return a single state then lock itself until the switch is changed
bool
checkSwitch(void);


/* Checks to see if the state of the switch has been changed.
   Changes the program to the initialising state when changed */
void
switched(void);


#endif /* SWITCH_H_ */
