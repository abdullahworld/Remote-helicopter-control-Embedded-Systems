// yaw.c - Controls the yaw of the helicopter

// Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Daniel Page
// Last modified: 5.5.2019


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "yaw.h"
#include "motors.h"
#include "control.h"


// Constants
#define NUM_READINGS 448 // The number of readings in the slotted disk (4 * (112 slots) in a rev)
#define CHA_PIN GPIO_PIN_0
#define CHB_PIN GPIO_PIN_1
#define PORTB GPIO_PORTB_BASE
#define FULL_ROT 360 // A full rotation in degrees
#define YAW_REF_PIN GPIO_PIN_4
#define YAW_REF_PORT GPIO_PORTC_BASE


// Sets variables
enum quadrature {A=0, B=1, C=3, D=2}; // Sets the values for the finite state machine
int32_t currentState;
static int32_t slots;


// ISR for quadrature encoding
void
YawIntHandler(void)
{
    int32_t nextState;
    nextState = GPIOPinRead(PORTB, CHA_PIN | CHB_PIN);
    /* A Finite State Machine that changes from one state to another.
       There are four states: A, B, C, and D, where A = 00, B = 01, C = 11, and D = 10. */
    switch(currentState)
    {
        case A:
            switch(nextState)
            {
            case B:
                slots--;
                break;
            case D:
                slots++;
                break;
            }
            break;
        case B:
            switch(nextState)
            {
            case A:
                slots++;
                break;
            case C:
                slots--;
                break;
            }
            break;
        case C:
            switch(nextState)
            {
            case B:
                slots++;
                break;
            case D:
                slots--;
                break;
            }
            break;
        case D:
            switch(nextState)
            {
            case A:
                slots--;
                break;
            case C:
                slots++;
                break;
            }
            break;
    }
    currentState = nextState;
    GPIOIntClear(PORTB, CHA_PIN | CHB_PIN);
}


// Initialises the GPIO pin register for yaw channels A and B
void
initYawGPIO(void)
{
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOInput (PORTB, CHA_PIN | CHB_PIN);
    GPIOPadConfigSet (PORTB, CHA_PIN | CHB_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    GPIODirModeSet(PORTB, CHA_PIN | CHB_PIN, GPIO_DIR_MODE_IN);

    GPIOIntRegister(PORTB, YawIntHandler);
    GPIOIntTypeSet(PORTB, CHA_PIN | CHB_PIN, GPIO_BOTH_EDGES);
    GPIOIntEnable(PORTB, CHA_PIN | CHB_PIN);
}


// Returns the calculated and rounded yaw in degrees with respect to the reference point
// This function will be use internally throughout the code and not for the dispaly.c
int16_t
getYaw(void)
{
    return (2 * FULL_ROT * slots + NUM_READINGS) / (2 * NUM_READINGS);
}

// Returns the yaw within the range -360 to 360 for the display
// We use this function only to make it readable for the user. It is easier to read the YAW angle to be -30 deg rather than -390deg or -750deg
// We will only use this for the OLED display and on display.c
int16_t
getDispYaw(void)
{
    enum directions {clockwise, anticlockwise};
    static enum directions dir;
    static int8_t rev = 0;

    if ((getYaw() / 360) >= (rev + 1))  { // If true, increment the number of revolutions and sign the direction to be CW
            dir = clockwise;
            rev++;
    } else if ((getYaw() / 360) <= (rev - 1)) { // If true, decrement the number of revolutions and sign the direction to be CCW
            dir = anticlockwise;
            rev--;
    }

    if (dir == clockwise) {
        return (getYaw() - rev * 360); // If the directions is CW reset the YAW to 0 deg after each full CW rotations
    } else if (dir == anticlockwise) { // same idea here.
        return (getYaw() - rev * 360);
    } else {
        return getYaw();
    }
}



// ISR for finding the yaw reference
void
YawRefIntHandler(void)
{
    if (GPIOPinRead(YAW_REF_PORT, YAW_REF_PIN) == 0) {
        GPIOIntDisable(YAW_REF_PORT, YAW_REF_PIN);
        GPIOIntClear(YAW_REF_PORT, YAW_REF_PIN);
        slots = 0;
        findRefStop();
        deactivateTailPWM();
    }
}


// Initialises the GPIO pin register for the yaw reference point
void
initYawRef(void)
{
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOC);
    GPIOPinTypeGPIOInput (YAW_REF_PORT, YAW_REF_PIN);
    GPIOPadConfigSet (YAW_REF_PORT, YAW_REF_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIODirModeSet(YAW_REF_PORT, YAW_REF_PIN, GPIO_DIR_MODE_IN);

    GPIOIntRegister(YAW_REF_PORT, YawRefIntHandler);
    GPIOIntTypeSet(YAW_REF_PORT, YAW_REF_PIN, GPIO_FALLING_EDGE);
    GPIOIntEnable(YAW_REF_PORT, YAW_REF_PIN);
}



