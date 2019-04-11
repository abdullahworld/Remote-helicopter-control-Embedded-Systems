#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "yaw.h"


#define NUM_SLOTS 112 // The number of slots in the slotted disk
#define ChAPin GPIO_PIN_0
#define ChBPin GPIO_PIN_1
#define PortB GPIO_PORTB_BASE
#define FULL_ROT 360 // A full rotation in degrees


enum quadrature {Same, Different};
static enum quadrature diskState;
static uint8_t ChanA, ChanB;
static int32_t slots;


void YawIntHandler(void) {
    ChanA = GPIOPinRead(PortB, ChAPin);
    ChanB = GPIOPinRead(PortB, ChBPin) >> 1; // Bit shifted to the right
    if (ChanA == 1 && ChanB == 1) {
        diskState = Same;
    } else if (diskState == Same && ChanA == 1 && ChanB == 0){
        slots++;
        diskState = Different;
    } else if (diskState == Same && ChanA == 0 && ChanB == 1) {
        slots--;
        diskState = Different;
    }
    GPIOIntClear(PortB, ChAPin | ChBPin);
}


void initYawGPIO(void) {
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOInput (PortB, ChAPin | ChBPin);
    GPIOPadConfigSet (PortB, ChAPin | ChBPin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    GPIODirModeSet(PortB, ChAPin | ChBPin, GPIO_DIR_MODE_IN);

    GPIOIntRegister(PortB, YawIntHandler);
    GPIOIntTypeSet(PortB, ChAPin | ChBPin, GPIO_BOTH_EDGES);
    GPIOIntEnable(PortB, ChAPin | ChBPin);
}


int32_t yaw(void) {
    return ((FULL_ROT*slots)/NUM_SLOTS);
}


void setYawRef(void) {
    slots = 0;
}


