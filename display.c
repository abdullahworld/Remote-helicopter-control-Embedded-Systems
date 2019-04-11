#include <stdint.h>
#include <stdbool.h>
#include "utils/ustdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "OrbitOLED/lib_orbitOled/OrbitOled.h"
#include "altitude.h"
#include "yaw.h"
#include "display.h"

enum screen {stats, mean_adc, blank};
static enum screen screen_state = stats;


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


void displayStats(int32_t altitude, int32_t yaw) {
    char string[17];  // 16 characters across the display

    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf (string, sizeof(string), "ALTITUDE: %5d%%", altitude);
    // Update line on display.
    OLEDStringDraw (string, 0, 0);
    usnprintf (string, sizeof(string), "YAW: %7d DEG", yaw);
    // Update line on display.
    OLEDStringDraw (string, 0, 1);
}


void SetDispValues(uint32_t g_ulSampCnt) {
    if (screen_state == mean_adc) {
        // Calculates and display the rounded mean of the buffer contents
        displayMeanVal(RetMeanVal(), g_ulSampCnt);
    } else if (screen_state == stats) {
        displayStats(altitude(),yaw());
        // max height v = -0.8v and min height v = 0v
        // This adds 0.5 so the value is truncated to the right value: (2*100*x + y)/2y = x/y + 0.5
    }
}


void changeDispState(void) {
    OrbitOledClear();
    switch(screen_state) {
        case stats:
            screen_state = mean_adc;
            break;
        case mean_adc:
            screen_state = blank;
            break;
        case blank:
            screen_state = stats;
            break;
    }
}
