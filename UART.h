/*
 * UART.h
 *
 * Based on uartDemo.c - Example code for ENCE361 By P.J. Bones  UCECE
 *
 *  Last Modified on : 15/05/2023
 *      Author: Thomas Fleming & Jonathan London
 */
#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdbool.h>

//
// Constants
//
#define BAUD_RATE 9600
#define UART_USB_BASE           UART0_BASE
#define UART_USB_PERIPH_UART    SYSCTL_PERIPH_UART0
#define UART_USB_PERIPH_GPIO    SYSCTL_PERIPH_GPIOA
#define UART_USB_GPIO_BASE      GPIO_PORTA_BASE
#define UART_USB_GPIO_PIN_RX    GPIO_PIN_0
#define UART_USB_GPIO_PIN_TX    GPIO_PIN_1
#define UART_USB_GPIO_PINS      UART_USB_GPIO_PIN_RX | UART_USB_GPIO_PIN_TX


//********************************************************
// initialiseUSB_UART - 8 bits, 1 stop bit, no parity
//********************************************************
void initialiseUSB_UART (void);

//**********************************************************************
// Transmit a string via UART0
//**********************************************************************
void UARTSend (char *pucBuffer);


#endif /* UART_H_ */
