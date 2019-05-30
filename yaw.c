// yaw.c - Controls the yaw of the helicopter

// Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Daniel Page
// Last modified: 9.5.2019


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
                slots--; // Decreases the slot count (anticlockwise)
                break;
            case D:
                slots++; // Increases the slot count (clockwise)
                break;
            }
            break;
        case B:
            switch(nextState)
            {
            case A:
                slots++; // Increases the slot count (clockwise)
                break;
            case C:
                slots--; // Decreases the slot count (anticlockwise)
                break;
            }
            break;
        case C:
            switch(nextState)
            {
            case B:
                slots++; // Increases the slot count (clockwise)
                break;
            case D:
                slots--; // Decreases the slot count (anticlockwise)
                break;
            }
            break;
        case D:
            switch(nextState)
            {
            case A:
                slots--; // Decreases the slot count (anticlockwise)
                break;
            case C:
                slots++; // Increases the slot count (clockwise)
                break;
            }
            break;
    }
    currentState = nextState;
    // Limits the yaw to +-180 deg from the reference point
    if (slots == 224 || slots == -224) {
        slots = slots*-1; // Switches the sign of yaw angle
    }
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


// Returns the yaw in degrees with respect to the reference point
int16_t
getYaw(void)
{
    return (FULL_ROT * slots) / NUM_READINGS;
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



