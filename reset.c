/*
 * reset.c
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
#include "reset.h"

//*****************************************************************************
//
// Function to intialise the reset pin (PA6)
//
//*****************************************************************************
void
initReset (void)
{
    // Enable GPIO Port A
	SysCtlPeripheralEnable(RESET_PERIPH);  

    GPIOPadConfigSet(RESET_PORT_BASE, RESET_PIN, GPIO_STRENGTH_2MA,
                            GPIO_PIN_TYPE_STD_WPD);
	// Set data direction register as input
	GPIODirModeSet(RESET_PORT_BASE, RESET_PIN, GPIO_DIR_MODE_IN);
}

//*****************************************************************************
//
// Function to read the reset pin (PA6), and then trigger SystemCtlReset() function
//
//*****************************************************************************
void
updateReset (void)
{
    bool reset = GPIOPinRead(RESET_PORT_BASE,  RESET_PIN) == RESET_PIN; // Read pin PA6
    if (reset == false)
    {
        SysCtlReset();
    }
}


