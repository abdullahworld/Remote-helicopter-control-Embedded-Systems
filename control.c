#include "control.h"
#include "motors.h"
#include "altitude.h"


#define OUTPUT_MAX 98
#define OUTPUT_MIN 2


enum modes {Initialising, Flying, Landed, Landing};
static enum modes mode = Landed;
static uint32_t count;
static uint8_t setAlt = 0;
static int16_t setYaw = 0;


static int32_t Kp = 4;
static int32_t Ki = 0;
static int32_t Kd = 0;


// Starts routine to find reference
void findRefStart(void) {
    activateMainPWM();
    mode = Initialising;
}


void findRefStop(void) {
    mode = Flying;
    activateTailPWM();
}


void refPulse(void) {
    if (mode == Initialising) {
        if (count == 0) {
            activateTailPWM();
            count++;
        } else if (count == 100) {
            deactivateTailPWM();
            count++;
        } else if (count == 100) {
            count = 0;
        } else {
            count++;
        }
    }
}


// Returns a string of the mode
char* getMode(void) {
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


void incrAlt(void) {
    if (setAlt < 100 && mode != Initialising) {
        setAlt += 10;
    }
}


void decrAlt(void) {
    if (setAlt > 0 && mode != Initialising) {
        setAlt -= 10;
    }

}


void incrYaw(void) {
    if (setYaw < 360 && mode != Initialising) {
        setYaw += 15;
    }

}


void decrYaw(void) {
    if (setYaw > -360 && mode != Initialising) {
        setYaw -= 15;
    }
}


uint8_t getSetAlt(void) {
    return setAlt;
}


int16_t getSetYaw(void) {
    return setYaw;
}


void pidControlUpdate(void) {
    static int32_t setpoint;
    static int32_t measurement;
    static int32_t control;
    static int32_t error;
    static int32_t prev_error;
    static int32_t P;
    static int32_t dI;
    static int32_t I = 0;
    static int32_t D;
    static int32_t T = 0.01;

    if (mode == Flying) {
        setpoint = getSetAlt();
        measurement = getAlt();

        error = setpoint - measurement;


        P = Kp * error;
        dI = Ki * error * T;
        D = (Kd/T)*(error - prev_error);
        control = P + (I + dI) + D;
        prev_error = error;

        // Enforce output limits
        if (control > OUTPUT_MAX) {
            control = OUTPUT_MAX;
        } else if (control < OUTPUT_MIN) {
            control = OUTPUT_MIN;
        } else {
            I += dI;
        }
        setMainPWM(200, control);
        setTailPWM(200, control);
    }
}


