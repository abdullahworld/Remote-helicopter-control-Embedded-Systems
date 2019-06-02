
/* control.c - Controls the different states of the program and the positioning of the helicopter.
    Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Daniel Page
    Last modified: 1.6.2019 */


#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "control.h"
#include "motors.h"
#include "altitude.h"
#include "yaw.h"


// Constants
#define OUTPUT_MAX        95     // Largest possible duty cycle signal for the motors
#define OUTPUT_MIN        5      // Smallest possible duty cycle signal for the motors
#define OUTPUT_MAIN_MIN   10     // The duty cycle at which the main motor begins to move
#define PWM_FIXED_RATE_HZ 200    // Fixed rate of the PWM signal for the motors
#define M_KP              0.55   // Proportional gain for main motor
#define M_KP_LANDING      0.0005 // Proportional gain for main motor
#define M_KI              0.18   // Integral gain for main motor
#define T_KP              0.22   // Proportional gain for tail motor
#define T_KI              0.14   // Integral gain for the tail motor
#define T_DELTA           0.01   // Time increment for integral control
#define ON_PULSE_CNT      50     // The number of loops that the pulse is on for
#define OFF_PULSE_CNT     400    // The number of loops tha tthe pulse is off for


/* Sets variables */
enum modes {Initialising, Flying, Landed, Landing}; // Program states
static enum modes mode = Landed; // Initial state
static uint32_t count; // Count for the pulsing of the motors in the reference state
static uint8_t setAlt = 0; // Initial altitude the helicopter is set to
static int16_t setYaw = 0; // Initial yaw angle the helicopter is set to
static bool takeOff;


/* Starts a routine to find the reference location where the helicopter faces the front */
void
findRefStart(void)
{
    activateMainPWM(); // Turns on the main motor
    setMainPWM(PWM_FIXED_RATE_HZ, OUTPUT_MAIN_MIN);
    mode = Initialising; // Changes program state
}


/* Changes the mode to flying once the reference point has been found */
void
findRefStop(void)
{
    setTailPWM(PWM_FIXED_RATE_HZ, OUTPUT_MIN);
    activateTailPWM();
    mode = Flying;
}


/* Changes the program to the landing state */
void
modeLanding(void)
{
    mode = Landing;
}


/* Reset set values for altitude and yaw when the program is changed to the landing state */
void
landingSet(void) {
    static bool LandingFlag = 0;
    if (mode == Landing && LandingFlag == 0) {
        LandingFlag = 1;
        setAlt = 0;
        setYaw = 0;
    }
}


/* Checks to see if the helicopter has landed. It turns of the motors and performs a reset when it is landed */
void
landedCheck(void) {
    if (mode == Landing && getAlt() <= 0) { // Checks to see if the helicopter has landed
        deactivateMainPWM(); // Turns off main motor
        deactivateTailPWM(); // Turns off tail motor
        mode = Landed; // Changes the program state
        SysCtlReset(); // Soft reset
    }
}


/* Pulses the PWM of the tail motor to find the reference point */
void
refPulse(void)
{
    if (mode == Initialising) {
        if (count == 0) {
            activateTailPWM();
            count++;
        } else if (count == ON_PULSE_CNT) {
            deactivateTailPWM();
            count++;
        } else if (count == OFF_PULSE_CNT) {
            count = 0;
        } else {
            count++;
        }
    }
}


/* Returns a string of the current mode */
char*
getMode(void)
{
    if (mode == Flying) {
        return "Flying";
    } else if (mode == Landing) {
        return "Landing";
    } else if (mode == Initialising) {
        return "Initialising";
    } else {
        return "Landed";
    }
}


/* Increases the set altitude by 10% */
void
incrAlt(void)
{
    // Limits the set altitude to <=100% and can only be changed while landed or flying

    if (setAlt < 100 && mode == Flying) {
        setAlt += 10;
    }
}


/* Decreases the set altitude by 10% */
void
decrAlt(void)
{
    // Limits the set altitude to >=0% and can only be changed while landed or flying
    if (setAlt > 0 && mode == Flying) {
        setAlt -= 10;
    }

}


/* Increases the set yaw by 15 degrees */
void
incrYaw(void)
{
    // Limits the set yaw to <=180 degrees and can only be changed while landed or flying
    if (setYaw < 180 && mode == Flying) {
        setYaw += 15;
    }
}


/* Decreases the set yaw by 15 degrees */
void
decrYaw(void)
{
    // Limits the set yaw to >=-180 degrees and can only be changed while landed or flying
    if (setYaw > -180 && mode == Flying) {
        setYaw -= 15;
    }
}


/* Returns the current set altitude */
uint8_t
getSetAlt(void)
{
    return setAlt;
}


/* Returns the current set yaw */
int16_t
getSetYaw(void)
{
    return setYaw;
}


/* Updates the PI controller for the main motor based of the set position and the current position */
void
piMainUpdate(void)
{
    if ((mode == Flying || mode == Landing) && takeOff == 1) { // Control only occurs at these states
        static double I;
        double P;
        double control;
        double error;
        double dI;

        error = setAlt - getAlt(); // Error between the set altitude and the actual altitude

        if (mode != Landing) {
            P = M_KP * error; // Proportional control
        } else {
            P = M_KP_LANDING * error; // Proportional control for landing
        }

        dI = M_KI * error * T_DELTA; // Integral control

        control = P + (I + dI); // The combined elements of PI control

        // Enforces output limits
        if (control > OUTPUT_MAX) {
            control = OUTPUT_MAX;
        } else if (control < OUTPUT_MIN) {
            control = OUTPUT_MIN;
        } else {
            I += dI; // Accumulates the a history of the error in the integral
        }
        setMainPWM(PWM_FIXED_RATE_HZ, control); // Updates the PWM duty cycle
    } else if (setAlt >= 10) {
        takeOff = 1;
    }
}


/* Updates the PI controller for the tail motor based on the set position and the current position */
void
piTailUpdate(void)
{
    if ((mode == Flying || mode == Landing) && takeOff == 1) // PI control only occurs at these states after take off
    {
       double error;
       double P;
       double dI;
       double control; // The controller output
       static double I;

       error = setYaw - getYaw(); // Error between the set yaw and the actual yaw

       P = T_KP * error; // Proportional control
       dI = T_KI * error * T_DELTA; // Integral control
       control = P + (I + dI); // The combined elements of PI control

       // Enforces output limits
       if (control > OUTPUT_MAX) {
           control = OUTPUT_MAX;
       } else if (control < OUTPUT_MIN) {
           control = OUTPUT_MIN;
       } else {
           I += dI; // Accumulates the a history of the error in the integral
       }
       setTailPWM(PWM_FIXED_RATE_HZ, control); // Updates the PWM duty cycle
    }
}
