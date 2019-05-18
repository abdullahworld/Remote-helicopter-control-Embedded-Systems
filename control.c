// control.c - Controls the different states of the program and the positioning of the helicopter.

// Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Daniel Page
// Last modified: 9.5.2019


#include <stdint.h>
#include <stdbool.h>
#include "control.h"
#include "motors.h"
#include "altitude.h"
#include "yaw.h"


// Constants
#define OUTPUT_MAX        95
#define OUTPUT_MIN        5
#define PWM_FIXED_RATE_HZ 200
#define M_KP              0.55 // Proportional gain for main motor Kp
#define M_KI              0.18 // Integral gain for main motor Ki
#define T_KP              0.25 // Proportional gain for tail motor Kp
#define T_KI              0.15
#define T_KD              0 // Integral gain for tail motor Ki
#define T_DELTA           0.01 // dt


// Sets variables
enum modes {Initialising, Flying, Landed, Landing};
static enum modes mode = Landed;
static uint32_t count;
static uint8_t setAlt = 0;
static int16_t setYaw = 0;


// Starts routine to find reference
void
findRefStart(void)
{
    activateMainPWM();
    mode = Initialising;
}


// Changes the mode to flying once the reference point has been found
void
findRefStop(void)
{
    setTailPWM(PWM_FIXED_RATE_HZ, OUTPUT_MIN);
    activateTailPWM();
    mode = Flying;
}


void
modeLanding(void)
{
    mode = Landing;
}


void
landingSet(void) {
    static bool LandingFlag = 0;
    if (mode == Landing && LandingFlag == 0) {
        LandingFlag = 1;
        setAlt = 0;
        setYaw = 0;
    }
}


void
landedCheck(void) {
    if (mode == Landing && getAlt() <= 0) {
        deactivateMainPWM();
        deactivateTailPWM();
        mode = Landed;
    }
}


// Pules the PWM of the tail rotor to find the reference point
void
refPulse(void)
{
    if (mode == Initialising) {
        if (count == 0) {
            activateTailPWM();
            count++;
        } else if (count == 80) {
            deactivateTailPWM();
            count++;
        } else if (count == 350) {
            count = 0;
        } else {
            count++;
        }
    }
}


// Returns a string of the mode
char*
getMode(void)
{
static char charInitialising[] = "Initialising";
static char charLanded[] = "Landed";
static char charFlying[] = "Flying";
static char charLanding[] = "Landing";

    if (mode == Flying) {
        return charFlying;
    } else if (mode == Landing) {
        return charLanding;
    } else if (mode == Initialising) {
        return charInitialising;
    } else {
        return charLanded;
    }
}


// Increases the desired altitude by 10%
void
incrAlt(void)
{
    if (setAlt < 100 && mode != Initialising) {
        setAlt += 10;
    }
}


// Reduces the desire altitude by 10%
void
decrAlt(void)
{
    if (setAlt > 0 && mode != Initialising) {
        setAlt -= 10;
    }

}


// Increases the desired yaw by 15 degrees
void
incrYaw(void)
{
    if (setYaw < 180 && mode != Initialising) {
        setYaw += 15;
    }

}


// Decreases the desired yaw by 15 degrees
void
decrYaw(void)
{
    if (setYaw > -180 && mode != Initialising) {
        setYaw -= 15;
    }
}


// Returns the desired altitude
uint8_t
getSetAlt(void)
{
    return setAlt;
}


// Returns the desired yaw
int16_t
getSetYaw(void)
{
    return setYaw;
}


// Updates the PI controller for the main rotor based of the desired position and the current position
void
piMainUpdate(void)
{
    if ((mode == Flying || mode == Landing) && setAlt >= 10) {
        static double I;
        double P;
        double control;
        double error;
        double dI;

        error = setAlt - getAlt(); // error = set Altitude value - actual Altitude value
        P = M_KP * error;
        dI = M_KI * error * T_DELTA;
        control = P + (I + dI); // The controller output

        // Enforces output limits
        if (control > OUTPUT_MAX) {
            control = OUTPUT_MAX;
        } else if (control < OUTPUT_MIN) {
            control = OUTPUT_MIN;
        } else {
            I += dI;
        }
        setMainPWM(PWM_FIXED_RATE_HZ, control);
    }
}


// Updates the PI controller for the tail rotor based of the desired position and the current position
void
piTailUpdate(void)
{
    if ((mode == Flying || mode == Landing) && setAlt >= 10) {
       double error;
       double P;
       double dI;
       double D;
       double control; // The controller output
       static double I;
       static double prev_error;

       error = setYaw - getYaw(); // error = set YAW value - actual YAW value
       P = T_KP * error;
       dI = T_KI * error * T_DELTA;
       D = (T_KD/T_DELTA)*(error-prev_error);

       control = P + (I + dI) + D;
       prev_error = error;
       // Enforces output limits
       if (control > OUTPUT_MAX) {
           control = OUTPUT_MAX;
       } else if (control < OUTPUT_MIN) {
           control = OUTPUT_MIN;
       } else {
           I += dI;
       }
       setTailPWM(PWM_FIXED_RATE_HZ, control);
    }
}
