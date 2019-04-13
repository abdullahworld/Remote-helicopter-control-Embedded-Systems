#include "control.h"
#include "motors.h"
#include "altitude.h"


static double error_previous;
static double error_integrated;


// Proportional control
double pcontrol_update(double error, double K_P) {
    return error * K_P;
}


// Integral control
double icontrol_update(double error, double K_I, double delta_t) {
    error_integrated += error * delta_t;
    return error_integrated * K_I;
}


// Derivative control
double dcontrol_update(double error, double K_D, double delta_t) {
    double control;
    double error_derivative;

    error_derivative = (error - error_previous) / delta_t;
    control = error_derivative * K_D;

    error_previous = error;
    return control;
}


void feedbackControl(double setpoint, double K_P, double K_I, double K_D, double delta_t) {
    double error;
    double measurement;
    double control;

    measurement = getAlt();

    error = setpoint - measurement;
    control = pcontrol_update(error, K_P) + icontrol_update(error, K_P, delta_t) + dcontrol_update(error, K_D, delta_t);

    setMainPWM(200,control);
}


