# ENCE361 - The Helicopter Project 

## Description
This program controls the motion of a toy helicopter. It starts of by
calibrating the yaw with respect to a reference point and then uses
feedback control to maintain stable flight. The buttons are used to change
the height and yaw.

It can be run on an [online simulator](http://eng-labshare.canterbury.ac.nz) which takes a .out file.

Note: This program was designed on CCS8.3.0.00009 to work with the compiler TI v 18.1.4.LTS.

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
* [Hassan Alhujhoj](https://github.com/hassan-alhujhoj)
* [Abdullah Naeem](https://github.com/ana104-collab)
* [Daniel Page](https://github.com/Daniel-Page)
