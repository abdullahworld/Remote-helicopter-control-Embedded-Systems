// control.c - Controls the different states and the positioning of the helicopter

// Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Daniel Page
// Last modified: 28.4.2019


#include <stdint.h>
#include "control.h"
#include "motors.h"
#include "altitude.h"
#include "yaw.h"


// Constants
#define OUTPUT_MAX 95
#define OUTPUT_MIN 5


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
    mode = Flying;
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


void
incrAlt(void)
{
    if (setAlt < 100 && mode != Initialising) {
        setAlt += 10;
    }
}


void
decrAlt(void)
{
    if (setAlt > 0 && mode != Initialising) {
        setAlt -= 10;
    }

}


void
incrYaw(void)
{
    if (setYaw < 360 && mode != Initialising) {
        setYaw += 15;
    }

}


void
decrYaw(void)
{
    if (setYaw > -360 && mode != Initialising) {
        setYaw -= 15;
    }
}


uint8_t
getSetAlt(void)
{
    return setAlt;
}


int16_t
getSetYaw(void)
{
    return setYaw;
}


void
piMainUpdate(void)
{
    if (mode == Flying && setAlt >= 10) {
        static double I;
        double P;
        double control;
        double error;
        double dI;
        double T = 0.005;
        double MKp = 1;
        double MKi = 0.1;

        error = setAlt - getAlt();
        P = MKp * error;
        dI = MKi * error * T;
        control = P + (I + dI);

        // Enforces output limits
        if (control > OUTPUT_MAX) {
            control = OUTPUT_MAX;
        } else if (control < OUTPUT_MIN) {
            control = OUTPUT_MIN;
        } else {
            I += dI;
        }
        setMainPWM(200, control);
    }
}


void
piTailUpdate(void)
{
    if (mode == Flying && setAlt >= 10) {
       activateTailPWM(); // is this necessary?

       static double I;
       double P;
       double control;
       double error;
       double dI;
       double T = 0.005;
       double TKp = 0.1;
       double TKi = 0.05;

       error = setYaw - getYaw();
       P = TKp * error;
       dI = TKi * error * T;
       control = P + (I + dI);

       // Enforces output limits
       if (control > OUTPUT_MAX) {
           control = OUTPUT_MAX;
       } else if (control < OUTPUT_MIN) {
           control = OUTPUT_MIN;
       } else {
           I += dI;
       }
       setTailPWM(200, control);
    }
}
