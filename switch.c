/*  switch.c - Functions for the switch on the ORBIT BoosterPack. */

/*  Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Daniel Page
    Last modified: 1.6.2019 */


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "switch.h"
#include "control.h"


/* Constants */
#define SWITCH_PIN  GPIO_PIN_7
#define SWITCH_PORT GPIO_PORTA_BASE


/* Sets variables */
static bool switchState;
static bool flyingFlag = false;


/* Initialises the GPIO port for the switch */
void
initSwitch(void)
{
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeGPIOInput (SWITCH_PORT, SWITCH_PIN);
    GPIOPadConfigSet (SWITCH_PORT, SWITCH_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    GPIODirModeSet(SWITCH_PORT, SWITCH_PIN, GPIO_DIR_MODE_IN);
}


/* Updates the status of the switch in an ISR at SYS_TICK_RATE */
void
updateSwitch(void)
{
    switchState = GPIOPinRead(SWITCH_PORT, SWITCH_PIN);
}


/* Switch logic to only return a single state then lock itself until the switch is changed */
bool
checkSwitch(void)
{
    if (switchState != 0) { // Switch is up
        return true;
    } else { // Switch is down
        return false;
    }
}


/* Checks to see if the state of the switch has been changed.
   Changes the program to the initialising state when changed */
void
switched(void)
{
    if (checkSwitch() == true && flyingFlag == false) {
        findRefStart();
        flyingFlag = true;
    } else if (checkSwitch() == false && flyingFlag == true) {
        modeLanding();
        flyingFlag = false;
    }
}

