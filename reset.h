/*
 * reset.h
 *
 *  Last Modified on : 15/05/2023
 *      Author: Thomas Fleming & Jonathan London
 */
#ifndef RESET_H_
#define RESET_H_

#include <stdint.h>

//
// Constants
//
#define RESET_PERIPH      SYSCTL_PERIPH_GPIOA
#define RESET_PORT_BASE   GPIO_PORTA_BASE
#define RESET_PIN         GPIO_PIN_6

//*****************************************************************************
//
// Function to intialise the reset pin (PA6)
//
//*****************************************************************************
void initReset (void);


//*****************************************************************************
//
// Function to read the reset pin (PA6), and then trigger SystemCtlReset() function
//
//*****************************************************************************
void updateReset (void);


#endif /*RESET_H_*/
