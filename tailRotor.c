/**********************************************************
 *
 * pwmGen.c - Example code which generates a single PWM
 *    output on J4-05 (M0PWM7) with duty cycle fixed and
 *    the frequency controlled by UP and DOWN buttons in
 *    the range 50 Hz to 400 Hz.
 * 2017: Modified for Tiva and using straightforward, polled
 *    button debouncing implemented in 'buttons4' module.
 *
 * P.J. Bones   UCECE
 * Last modified:  7.2.2018
 **********************************************************/


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h" //Needed for pin configure
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"


/**********************************************************
 * Generates a single PWM signal on Tiva board pin J4-05 =
 * PC5 (M0PWM7).  This is the same PWM output as the
 * helicopter main rotor.
 **********************************************************/

/**********************************************************
 * Constants
 **********************************************************/
// PWM configuration
#define PWM_START_RATE_HZ       20
#define PWM_TAIL_START_RATE_HZ  200
#define PWM_RATE_STEP_HZ        50
#define PWM_DUTY_CYCLE_STEP     5
#define PWM_RATE_MIN_HZ         50
#define PWM_RATE_MAX_HZ         400
#define PWM_FIXED_DUTY          50
#define PWM_TAIL_FIXED_DUTY     10
#define PWM_DIVIDER_CODE        SYSCTL_PWMDIV_4
#define PWM_DIVIDER             4


// PF1, J3-10
#define PWM_TAIL_BASE        PWM1_BASE
#define PWM_TAIL_GEN         PWM_GEN_2
#define PWM_TAIL_OUTNUM      PWM_OUT_5
#define PWM_TAIL_OUTBIT      PWM_OUT_5_BIT
#define PWM_TAIL_PERIPH_PWM  SYSCTL_PERIPH_PWM1
#define PWM_TAIL_PERIPH_GPIO SYSCTL_PERIPH_GPIOF
#define PWM_TAIL_GPIO_BASE   GPIO_PORTF_BASE
#define PWM_TAIL_GPIO_CONFIG GPIO_PF1_M1PWM5
#define PWM_TAIL_GPIO_PIN    GPIO_PIN_1


static uint32_t ui32Freq = PWM_START_RATE_HZ;


void setTailPWM(uint32_t ui32Freq, uint32_t ui32Duty) {
    // Calculate the PWM period corresponding to the freq.
    uint32_t ui32Period =
        SysCtlClockGet() / PWM_DIVIDER / ui32Freq;

    PWMGenPeriodSet(PWM_TAIL_BASE, PWM_TAIL_GEN, ui32Period);
    PWMPulseWidthSet(PWM_TAIL_BASE, PWM_TAIL_OUTNUM,
        ui32Period * ui32Duty / 100);
}


/*********************************************************
 * initialisePWM
 * M0PWM7 (J4-05, PC5) is used for the main rotor motor
 *********************************************************/
void initialiseTailPWM(void) {
    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_GPIO);

    GPIOPinConfigure(PWM_TAIL_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_TAIL_GPIO_BASE, PWM_TAIL_GPIO_PIN);

    PWMGenConfigure(PWM_TAIL_BASE, PWM_TAIL_GEN,
                        PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    // Set the initial PWM parameters
    setTailPWM(PWM_TAIL_START_RATE_HZ, PWM_TAIL_FIXED_DUTY);

    PWMGenEnable(PWM_TAIL_BASE, PWM_TAIL_GEN);

    // Disable the output.  Repeat this call with 'true' to turn O/P on.
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, false);

    // As a precaution, make sure that the peripherals used are reset
    SysCtlPeripheralReset (PWM_TAIL_PERIPH_GPIO); // Used for PWM output
    SysCtlPeripheralReset (PWM_TAIL_PERIPH_PWM);  // Main Rotor PWM

    // Initialisation is complete, so turn on the output.
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, true);
}


uint32_t TailRotFreq(void) {
    return ui32Freq;
}
