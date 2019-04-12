#include <stdint.h>
#include <stdbool.h>
#include "yaw.h"
#include "motors.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "control.h"


#define YAW_REF_PIN GPIO_PIN_4
#define YAW_REF_PORT GPIO_PORTC_BASE

static bool YawRefState;

void initYawRef(void) {
    // Port C already init from PWM gen
    GPIOPinTypeGPIOInput (YAW_REF_PORT, YAW_REF_PIN);
    GPIOPadConfigSet (YAW_REF_PORT, YAW_REF_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    GPIODirModeSet(YAW_REF_PORT, YAW_REF_PIN, GPIO_DIR_MODE_IN);
}


void updateYawRef(void) {
    YawRefState = GPIOPinRead(YAW_REF_PORT, YAW_REF_PIN);
}


void YawRef(void) {
    if (YawRefState == 0) {
       setYawRef();
       deactivateMainPWM();
       deactivateTailPWM();
    }
}


void findRef(void) {
    activateMainPWM();
    activateTailPWM();

}
