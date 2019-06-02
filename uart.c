/*  uart.c - Functions for UART serial output. */

/*  Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Daniel Page
    Last modified: 1.6.2019
    Based on uartDemo.c by by P.J. Bones UCECE */


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "utils/ustdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "yaw.h"
#include "altitude.h"
#include "motors.h"
#include "switch.h"
#include "control.h"


/* Constant */
#define MAX_STR_LEN 16


/* USB Serial comms: UART0, Rx:PA0, Tx:PA1 */
#define BAUD_RATE 9600
#define UART_USB_BASE           UART0_BASE
#define UART_USB_PERIPH_UART    SYSCTL_PERIPH_UART0
#define UART_USB_PERIPH_GPIO    SYSCTL_PERIPH_GPIOA
#define UART_USB_GPIO_BASE      GPIO_PORTA_BASE
#define UART_USB_GPIO_PIN_RX    GPIO_PIN_0
#define UART_USB_GPIO_PIN_TX    GPIO_PIN_1
#define UART_USB_GPIO_PINS      UART_USB_GPIO_PIN_RX | UART_USB_GPIO_PIN_TX
#define SPACED_COUNT 20 // The number of loops between printing to the console


/* Set variable */
char statusStr[MAX_STR_LEN + 1];


/* Initialise USB_UART - 8 bits, 1 stop bit, no parity */
void
initialiseUSB_UART(void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    //
    SysCtlPeripheralEnable(UART_USB_PERIPH_UART);
    SysCtlPeripheralEnable(UART_USB_PERIPH_GPIO);
    //
    // Select the alternate (UART) function for these pins.
    //
    GPIOPinTypeUART(UART_USB_GPIO_BASE, UART_USB_GPIO_PINS);
    GPIOPinConfigure (GPIO_PA0_U0RX);
    GPIOPinConfigure (GPIO_PA1_U0TX);

    UARTConfigSetExpClk(UART_USB_BASE, SysCtlClockGet(), BAUD_RATE,
            UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
            UART_CONFIG_PAR_NONE);
    UARTFIFOEnable(UART_USB_BASE);
    UARTEnable(UART_USB_BASE);
}


/* Transmit a string via UART0 */
void
UARTSend (char *pucBuffer)
{
    // Loop while there are more characters to send.
    while(*pucBuffer)
    {
        // Write the next character to the UART Tx FIFO.
        UARTCharPut(UART_USB_BASE, *pucBuffer);
        pucBuffer++;
    }
}


/* Forms and sends messages to the console */
void
consoleMsg(void)
{
    usprintf (statusStr, "Alt: %d/%d%%\r\n", getAlt(), getSetAlt()); // Prints the current and set altitude [%]
    UARTSend (statusStr);
    usprintf (statusStr, "Yaw: %d/%d deg\r\n", getYaw(), getSetYaw()); // Prints the current and set yaw [%]
    UARTSend (statusStr);
    usprintf (statusStr, "Main: %d%%\r\n", GetMainDuty(), GetTailDuty()); // Prints the current main motor duty cycle [%]
    UARTSend (statusStr);
    usprintf (statusStr, "Tail: %d%%\r\n", GetTailDuty()); // Prints the current tail motor duty cycle [%]
    UARTSend (statusStr);
    usprintf (statusStr, "Mode: %s\r\n", getMode()); // Prints the program state
    UARTSend (statusStr);
}


/* Sends messages to the console periodically */
void
consoleMsgSpaced(void)
{
    static uint8_t n;
    if (n >= SPACED_COUNT) { // Set to approximately (SYS_TICK_RATE / SPACED_COUNT) Hz
        consoleMsg();
        n = 0;
    } else {
        n++;
    }
}
