// yaw.c - Controls the yaw of the helicopter

// Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Daniel Page
// Last modified: 28.4.2019


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "yaw.h"
#include "motors.h"
#include "control.h"


// Constants
#define NUM_READINGS 448 // The number of slots in the slotted disk (4 * 112 slots in a rev)
#define CHA_PIN GPIO_PIN_0
#define CHB_PIN GPIO_PIN_1
#define PORTB GPIO_PORTB_BASE
#define FULL_ROT 360 // A full rotation in degrees
#define YAW_REF_PIN GPIO_PIN_4
#define YAW_REF_PORT GPIO_PORTC_BASE


// Sets variables
enum quadrature {A=0, B=1, C=3, D=2};
int32_t currentState;
static int32_t slots;


// ISR for quadrature encoding
void
YawIntHandler(void)
{
    int32_t nextState;
    nextState = GPIOPinRead(PORTB, CHA_PIN | CHB_PIN);
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
int16_t
getYaw(void)
{
    return (2 * FULL_ROT * slots + NUM_READINGS) / (2 * NUM_READINGS);
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



