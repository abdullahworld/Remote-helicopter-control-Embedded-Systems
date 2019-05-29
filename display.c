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

// This display function will display each line on the oled display at time instead of displaying all four lines all togather 
// at the same time. This will improve the update rate on the oled display and make it faster.
void
displayStats(int16_t line)
{
    char string[17];  // 16 characters across the display
    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    if(line == 0){
        usnprintf (string, sizeof(string), "ALTITUDE: %5d%%", getAlt());
        OLEDStringDraw (string, 0, 0);
        line++;
    } else if(line == 1){// Update line on display.
        usnprintf (string, sizeof(string), "YAW: %7d DEG", getDispYaw());
        OLEDStringDraw (string, 0, 1);
        line++;
    } else if(line == 2){// Update line on display.
        usnprintf (string, sizeof(string), "MAIN PWM: %5d%%", GetMainDuty());
        OLEDStringDraw (string, 0, 2);
        line++;
    } else if(line == 3){// Update line on display.
        usnprintf (string, sizeof(string), "TAIL PWM: %5d%%", GetTailDuty());
        OLEDStringDraw (string, 0, 3);
        line = 0;
    }
}
