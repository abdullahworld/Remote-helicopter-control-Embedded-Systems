// motors.c - Controls the PWM output to the motors

// Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Daniel Page
// Last modified: 1.6.2019

// Based on pwmGen.c by by P.J. Bones UCECE

// Outputs: PC5 (PWM Main), PF1 (PWM Tail)


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"


// PWM configuration
#define PWM_FIXED_RATE_HZ       200
#define PWM_MAIN_FIXED_DUTY     5
#define PWM_TAIL_FIXED_DUTY     35
#define PWM_DIVIDER_CODE        SYSCTL_PWMDIV_4
#define PWM_DIVIDER             4


// Main Rotor PWM: PC5, J4-05
#define PWM_MAIN_BASE        PWM0_BASE
#define PWM_MAIN_GEN         PWM_GEN_3
#define PWM_MAIN_OUTNUM      PWM_OUT_7
#define PWM_MAIN_OUTBIT      PWM_OUT_7_BIT
#define PWM_MAIN_PERIPH_PWM  SYSCTL_PERIPH_PWM0
#define PWM_MAIN_PERIPH_GPIO SYSCTL_PERIPH_GPIOC
#define PWM_MAIN_GPIO_BASE   GPIO_PORTC_BASE
#define PWM_MAIN_GPIO_CONFIG GPIO_PC5_M0PWM7
#define PWM_MAIN_GPIO_PIN    GPIO_PIN_5


// Tail Rotor PWM: PF1, J3-10
#define PWM_TAIL_BASE        PWM1_BASE
#define PWM_TAIL_GEN         PWM_GEN_2
#define PWM_TAIL_OUTNUM      PWM_OUT_5
#define PWM_TAIL_OUTBIT      PWM_OUT_5_BIT
#define PWM_TAIL_PERIPH_PWM  SYSCTL_PERIPH_PWM1
#define PWM_TAIL_PERIPH_GPIO SYSCTL_PERIPH_GPIOF
#define PWM_TAIL_GPIO_BASE   GPIO_PORTF_BASE
#define PWM_TAIL_GPIO_CONFIG GPIO_PF1_M1PWM5
#define PWM_TAIL_GPIO_PIN    GPIO_PIN_1 // Causes the red LED to activate on the TIVA board


// Sets the frequency and duty cycle of the PWM for the main motor
void
setMainPWM(uint32_t ui32Freq, uint32_t ui32Duty)
{
    // Calculate the PWM period corresponding to the freq.
    uint32_t ui32Period =
        SysCtlClockGet() / PWM_DIVIDER / ui32Freq;

    PWMGenPeriodSet(PWM_MAIN_BASE, PWM_MAIN_GEN, ui32Period);
    PWMPulseWidthSet(PWM_MAIN_BASE, PWM_MAIN_OUTNUM,
                     ui32Period * ui32Duty / 100);
}


// Sets the frequency and duty cycle of the PWM for the tail motor
void
setTailPWM(uint32_t ui32Freq, uint32_t ui32Duty)
{
    // Calculate the PWM period corresponding to the freq.
    uint32_t ui32Period = SysCtlClockGet() / PWM_DIVIDER / ui32Freq;

    PWMGenPeriodSet(PWM_TAIL_BASE, PWM_TAIL_GEN, ui32Period);
    PWMPulseWidthSet(PWM_TAIL_BASE, PWM_TAIL_OUTNUM,
                     ui32Period * ui32Duty / 100);
}


// Returns the rounded duty cycle of the main motor
uint16_t
GetMainDuty(void)
{
    return (2*100*PWMPulseWidthGet(PWM_MAIN_BASE, PWM_MAIN_OUTNUM)
            + PWMGenPeriodGet(PWM_MAIN_BASE, PWM_MAIN_GEN))
            / (2*PWMGenPeriodGet(PWM_MAIN_BASE, PWM_MAIN_GEN));
}


// Returns the rounded duty cycle of the tail motor
uint16_t
GetTailDuty(void)
{
    return (2*100*PWMPulseWidthGet(PWM_TAIL_BASE, PWM_TAIL_OUTNUM)
            + PWMGenPeriodGet(PWM_TAIL_BASE, PWM_TAIL_GEN))
            / (2*PWMGenPeriodGet(PWM_TAIL_BASE, PWM_TAIL_GEN));
}


// Initialises the PWM for the main motor
void
initialiseMainPWM(void)
{
    SysCtlPWMClockSet(PWM_DIVIDER_CODE);

    // As a precaution, make sure that the peripherals used are reset
    SysCtlPeripheralReset(PWM_MAIN_PERIPH_GPIO); // Used for PWM output
    SysCtlPeripheralReset(PWM_MAIN_PERIPH_PWM);  // Main Rotor PWM

    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_GPIO);

    GPIOPinConfigure(PWM_MAIN_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_MAIN_GPIO_BASE, PWM_MAIN_GPIO_PIN);

    PWMGenConfigure(PWM_MAIN_BASE, PWM_MAIN_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    // Set the initial PWM parameters
    setMainPWM(PWM_FIXED_RATE_HZ, PWM_MAIN_FIXED_DUTY);
    PWMGenEnable(PWM_MAIN_BASE, PWM_MAIN_GEN);
    // Disable the output.  Repeat this call with 'true' to turn O/P on.
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, false);
}


// Initialises the PWM for the tail motor
void
initialiseTailPWM(void)
{
    SysCtlPeripheralReset(PWM_TAIL_PERIPH_GPIO); // Used for PWM output
    SysCtlPeripheralReset(PWM_TAIL_PERIPH_PWM);  // Main Rotor PWM

    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_GPIO);

    GPIOPinConfigure(PWM_TAIL_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_TAIL_GPIO_BASE, PWM_TAIL_GPIO_PIN);

    PWMGenConfigure(PWM_TAIL_BASE, PWM_TAIL_GEN,
                       PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    // Set the initial PWM parameters
    setTailPWM (PWM_FIXED_RATE_HZ, PWM_TAIL_FIXED_DUTY);

    PWMGenEnable(PWM_TAIL_BASE, PWM_TAIL_GEN);

    // Disable the output.  Repeat this call with 'true' to turn O/P on.
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, false);
}


// Enables the PWM output for the main motor
void
activateMainPWM(void)
{
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, true);
}


// Enables the PWM output for the tail motor
void
activateTailPWM(void)
{
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, true);
}


// Disables the PWM output for the main motor
void
deactivateMainPWM(void)
{
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, false);
}


// Disables the PWM output for the tail rotor
void
deactivateTailPWM(void)
{
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, false);
}
