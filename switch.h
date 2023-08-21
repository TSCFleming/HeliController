/*
 * switch.h
 *
 *  Last Modified on : 15/05/2023
 *      Author: Thomas Fleming & Jonathan London
 */
#ifndef SWITCH_H_
#define SWITCH_H_

#include <stdint.h>

//
// Constants
//
#define SW1_PERIPH      SYSCTL_PERIPH_GPIOA
#define SW1_PORT_BASE   GPIO_PORTA_BASE
#define SW1             GPIO_PIN_7


//*****************************************************************************
//
// Function to intialise the pin for Switch1(SW1 - PA7)
//
//*****************************************************************************
void initSwitch (void);


//*****************************************************************************
//
// Function to read the switch pin(PA7) and then return its state as a boolean
//
//*****************************************************************************
bool checkSwitch (void);

#endif /*SWITCH_H_*/
