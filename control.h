#ifndef CONTROL_H_
#define CONTROL_H_


double pcontrol_update(double error, double K_P);


double icontrol_update(double error, double K_I, double delta_t);


double dcontrol_update(double error, double K_D, double delta_t);


void feedbackControl(double setpoint, double K_P, double K_I, double K_D, double delta_t);


#endif /* CONTROL_H_ */
