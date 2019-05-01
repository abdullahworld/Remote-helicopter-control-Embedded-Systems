// switch.c - Makes the switch work.

// Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Daniel Page
// Last modified: 28.4.2019


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "control.h"


// Constants
#define SWITCH_PIN GPIO_PIN_7
#define SWITCH_PORT GPIO_PORTA_BASE


// Set variables
static bool switchState;
static bool butflag = 0;


// Initialises the GPIO port for the switch
void
initSwitch(void)
{
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeGPIOInput (SWITCH_PORT, SWITCH_PIN);
    GPIOPadConfigSet (SWITCH_PORT, SWITCH_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    GPIODirModeSet(SWITCH_PORT, SWITCH_PIN, GPIO_DIR_MODE_IN);
}


// Updates the status of the switch in an ISR at SYS_TICK_RATE
void
updateSwitch(void)
{
    switchState = GPIOPinRead(SWITCH_PORT, SWITCH_PIN);
}


// Switch logic to only return a single state then lock itself until the switch is changed
bool
checkSwitch(void)
{
    if (switchState != 0 && butflag == 0) { // Switch is up
        butflag = 1;
        return true;
    } else if (switchState == 0 && butflag == 1) { // Switch is down
        butflag = 0;
        return false;
    } else {
        return false;
    }
}


// Checks to see if the state of the switch has been changed
// Starts the initialising state when changed
void
switched(void)
{
    if (checkSwitch() == true) { // Need to prevent from activating twice
        findRefStart();
    }
}

