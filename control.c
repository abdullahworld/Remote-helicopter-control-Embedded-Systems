#include "control.h"
#include "motors.h"
#include "altitude.h"


enum modes {Initialising, Flying, Landed, Landing};
static enum modes mode = Landed;
static uint32_t count;
static uint8_t setAlt = 0;
static int16_t setYaw = 0;


// Starts routine to find reference
void findRefStart(void) {
    activateMainPWM();
    mode = Initialising;
}


void findRefStop(void) {
    mode = Landed;
}


void refPulse(void) {
    if (mode == Initialising) {
        if (count == 0) {
            activateTailPWM();
            count++;
        } else if (count == 10) {
            deactivateTailPWM();
            count++;
        } else if (count == 45) {
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


void setModeFlying(void) {
    mode = Flying;
}


void setModeLanding(void) {
    mode = Landing;
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


