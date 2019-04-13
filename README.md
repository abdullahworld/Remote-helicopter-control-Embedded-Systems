# ENCE361 - The Helicopter Project 

## Description
This program controls the motion of a toy helicopter. It starts of by
calibrating the yaw with respect to a reference point and then uses
PID control to maintain stable flight. The buttons are used to change
the height and yaw.

## Inputs

> #### Altitude sensor
- PE4

> #### Yaw sensor
- PB0 (channel A)
- PB1 (channel B)
- PC4 (reference)

## Outputs

> #### Main Rotor PWM
- PC5

> #### Tail Rotor PWM
- PF1

## Buttons

> #### Up
- Increases the duty cycle of the main rotor PWM by 10%

> #### Down
- Decreases the duty cycle of the main rotor PWM by 10%

> #### Left
- Increases the duty cycle of the tail rotor PWM by 15%

> #### Right
- Decreases the duty cycle of the tail rotor PWM by 15%

## Contributers
> - Hassan Ali Alhujhoj
> - Abdullah Naeem 
> - Daniel Page