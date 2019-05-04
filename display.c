// display.c - Controls the OLED display, showing the stats of the helicopter.

// Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Daniel Page
// Last modified: 4.5.2019


#include <stdint.h>
#include <stdbool.h>
#include "utils/ustdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "OrbitOLED/lib_orbitOled/OrbitOled.h"
#include "altitude.h"
#include "yaw.h"
#include "display.h"
#include "motors.h"


void
initDisplay(void)
{
    // intialise the Orbit OLED display
    OLEDInitialise();
}


void
displayStats(void)
{
    char string[17];  // 16 characters across the display

    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf (string, sizeof(string), "ALTITUDE: %5d%%", getAlt());
    // Update line on display.
    OLEDStringDraw (string, 0, 0);
    usnprintf (string, sizeof(string), "YAW: %7d DEG", getDispYaw());
    // Update line on display.
    OLEDStringDraw (string, 0, 1);
    usnprintf (string, sizeof(string), "MAIN PWM: %5d%%", GetMainDuty());
    // Update line on display.
    OLEDStringDraw (string, 0, 2);
    usnprintf (string, sizeof(string), "TAIL PWM: %5d%%", GetTailDuty());
    // Update line on display.
    OLEDStringDraw (string, 0, 3);
}
