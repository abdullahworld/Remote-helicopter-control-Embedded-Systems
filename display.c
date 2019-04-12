#include <stdint.h>
#include <stdbool.h>
#include "utils/ustdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "OrbitOLED/lib_orbitOled/OrbitOled.h"
#include "altitude.h"
#include "yaw.h"
#include "display.h"
#include "motors.h"


void initDisplay (void) {
    // intialise the Orbit OLED display
    OLEDInitialise();
}


//*****************************************************************************
//
// Function to display the mean ADC value (10-bit value, note) and sample count.
//
//*****************************************************************************
void displayMeanVal(int16_t meanVal, int32_t count) {
    char string[17];  // 16 characters across the display

    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf(string, sizeof(string), "MEAN ADC = %4d", meanVal);
    // Update line on display.
    OLEDStringDraw (string, 0, 0);

    usnprintf(string, sizeof(string), "SAMPLE NO. %5d", count);
    OLEDStringDraw (string, 0, 1);
}


void displayStats(void) {
    char string[17];  // 16 characters across the display

    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf (string, sizeof(string), "ALTITUDE: %5d%%", getAlt());
    // Update line on display.
    OLEDStringDraw (string, 0, 0);
    usnprintf (string, sizeof(string), "YAW: %7d DEG", yaw());
    // Update line on display.
    OLEDStringDraw (string, 0, 1);
    usnprintf (string, sizeof(string), "MAIN PWM: %5d%%", GetMainDuty());
    // Update line on display.
    OLEDStringDraw (string, 0, 2);
    usnprintf (string, sizeof(string), "TAIL PWM: %5d%%", GetTailDuty());
    // Update line on display.
    OLEDStringDraw (string, 0, 3);
}
