#include <stdint.h>
#include "control.h"
#include "motors.h"
#include "altitude.h"
#include "yaw.h"


#define OUTPUT_MAX 95
#define OUTPUT_MIN 5


enum modes {Initialising, Flying, Landed, Landing};
static enum modes mode = Landed;
static uint32_t count;
static uint8_t setAlt = 0;
static int16_t setYaw = 0;


static int32_t MKp = 6;
static int32_t MKi = 0;
static int32_t MKd = 100;

static int32_t TKp = 3;
static int32_t TKi = 0;
static int32_t TKd = 100;


// Starts routine to find reference
void findRefStart(void) {
    activateMainPWM();
    mode = Initialising;
}


// Occurs once the reference point has been found
void findRefStop(void) {
    mode = Flying;
}


void refPulse(void) {
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


void pidMainUpdate(void) {
    static int32_t control;
    static int32_t error;
    static int32_t prev_error;
    static int32_t P;
    static int32_t dI;
    static int32_t I = 0;
    int32_t D;
    int32_t T = 0.01;

    if (mode == Flying && setAlt >= 10) {
        error = setAlt - getAlt();

        P = MKp * error;
        dI = MKi * error * T;
        D = (MKd/T)*(error - prev_error);
        control = P + (I + dI) + D;
        if (error > 0) {
            control = P + (I + dI) + D;
        } else if (error < 0) {
           control = 10;
        }
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
    }
}


void pidTailUpdate(void) {
    static int32_t control;
    static int32_t error;
    static int32_t prev_error;
    static int32_t P;
    static int32_t dI;
    static int32_t I = 0;
    int32_t D;
    int32_t T = 0.01;

    if (mode == Flying && setAlt >= 10) {
        activateTailPWM();

        error = setYaw - getYaw();

        P = TKp * error;
        dI = TKi * error * T;
        D = (TKd/T)*(error - prev_error);
        if (error > 0) {
            control = P + (I + dI) + D;
        } else if (error < 0) {
            control = 10;
        }

        prev_error = error;

        // Enforce output limits
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
