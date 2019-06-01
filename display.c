// display.c - Controls the OLED display, showing the stats of the helicopter.

// Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Daniel Page
// Last modified: 1.6.2019


#include <stdint.h>
#include <stdbool.h>
#include "utils/ustdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "OrbitOLED/lib_orbitOled/OrbitOled.h"
#include "altitude.h"
#include "yaw.h"
#include "display.h"
#include "motors.h"


// Constant
#define MAX_OLED_LINES 4


// Initalises global variable
int32_t OLEDDisplaytTimer = 0;


// Initialise the Orbit OLED display
void
initDisplay(void)
{
    OLEDInitialise();
}


// Increments the timer to show the next line on the OLED screen
void incrementDispTimer(void)
{
    OLEDDisplaytTimer++;
}


/* This display function will display each line depending on the timer that is updated based on sysTick
   The timer will be updated every 10ms and each timer count is equal to 10ms. Thus, the OLED will update every 40 ms. */
void
displayStats(void)
{
    char string[17];  // 16 characters across the display
    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    if(OLEDDisplaytTimer <= 1){
        usnprintf(string, sizeof(string), "ALTITUDE: %5d%%", getAlt());
        OLEDStringDraw (string, 0, 0);
    } else if(OLEDDisplaytTimer > 1 && OLEDDisplaytTimer <= 2) { // Update line on display.
        usnprintf(string, sizeof(string), "YAW: %7d DEG", getYaw());
        OLEDStringDraw (string, 0, 1);
    } else if(OLEDDisplaytTimer > 2 && OLEDDisplaytTimer <= 3) { // Update line on display.
        usnprintf(string, sizeof(string), "MAIN PWM: %5d%%", GetMainDuty());
        OLEDStringDraw (string, 0, 2);
    } else if(OLEDDisplaytTimer > 3 && OLEDDisplaytTimer <= 4) { // Update line on display.
        usnprintf(string, sizeof(string), "TAIL PWM: %5d%%", GetTailDuty());
        OLEDStringDraw (string, 0, 3);
    }

    if(OLEDDisplaytTimer > MAX_OLED_LINES) {
        OLEDDisplaytTimer = 0;
    }
}
