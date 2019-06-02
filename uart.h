/*  uart.h - Functions for UART serial output. */

/*  Contributers: Hassan Ali Alhujhoj, Abdullah Naeem and Daniel Page
    Last modified: 1.6.2019
    Based on uartDemo.c by by P.J. Bones UCECE */

#ifndef UART_H_
#define UART_H_

/* Initialise USB_UART - 8 bits, 1 stop bit, no parity */
void
initialiseUSB_UART(void);

/* Transmit a string via UART0 */
void
UARTSend (char *pucBuffer);

/* Forms and sends messages to the console */
void
consoleMsg(void);

/* Sends messages to the console periodically */
void
consoleMsgSpaced(void);

#endif /* UART_H_ */