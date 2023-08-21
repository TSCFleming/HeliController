/*
 * switch.c
 *
 *  Last Modified on : 15/05/2023
 *      Author: Thomas Fleming & Jonathan London
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "switch.h"

//*****************************************************************************
//
// Function to intialise the pin for Switch1(SW1 - PA7)
//
//*****************************************************************************
void
initSwitch (void)
{

    // Enable GPIO Port A
	SysCtlPeripheralEnable(SW1_PERIPH);  

    GPIOPadConfigSet(SW1_PORT_BASE, SW1, GPIO_STRENGTH_2MA,
                            GPIO_PIN_TYPE_STD_WPD);
	// Set data direction register as input
	GPIODirModeSet(SW1_PORT_BASE, SW1, GPIO_DIR_MODE_IN);
}

//*****************************************************************************
//
// Function to read the switch pin(PA7) and then return its state as a boolean
//
//*****************************************************************************
bool
checkSwitch (void)
{
    bool switch_1 = GPIOPinRead(SW1_PORT_BASE,  SW1) == SW1;
    return switch_1;
}


