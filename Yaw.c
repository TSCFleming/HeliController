/*
 * Yaw.c
 *
 *  Last modified on: 02/05/2023
 *      Authors: Jonathan london, Thomas Fleming
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "utils/ustdlib.h"
#include "stdio.h"
#include "stdlib.h"

#include "Yaw.h"

//
// Globals from main
//
extern int32_t yawVal;
//
// Globals to module
//
static int32_t yawCount = 0;   //test #of interrupts
static int32_t yawAB = 0;
static int8_t STATE = 0;  //Yaw previous AB State

//*****************************************************************************
//
// The handler for the YAW interrupt.
//  Reads the two pins connected to the yaw sensors and uses a simple FSM to
//  perform quadrature decoding.
//
//*****************************************************************************
void
YawIntHandler(void)
{
    yawCount++;

    yawAB = GPIOPinRead(GPIO_PORTB_BASE, (GPIO_INT_PIN_0 | GPIO_INT_PIN_1));

    switch (yawAB) {
    case 0:
        //compare with previous state
        if (STATE == 10) {
            yawVal++;
        } else if (STATE == 01) {
            yawVal--;
        }
        //update state
        STATE = 00;
        break;
    case 1: //01
        if (STATE == 00) {
            yawVal++;
        } else if (STATE == 11) {
            yawVal--;
        }
        STATE = 01;
        break;
    case 2:  //10
        if (STATE == 11) {
            yawVal++;
        } else if (STATE == 00) {
            yawVal--;
        }
        STATE = 10;
        break;
    case 3:  //11
        if (STATE == 01) {
            yawVal++;
        } else if (STATE == 10) {
            yawVal--;
        }
        STATE = 11;
        break;
    }

    GPIOIntClear(GPIO_PORTB_BASE, (GPIO_INT_PIN_0 | GPIO_INT_PIN_1));
}


//*****************************************************************************
//
// The initialiser for the YAW interrupt.
//  Register the interrupt handler
//  Enable and configure the GPIO associated with yaw sensors (PB0 | PB1)
//
//*****************************************************************************
void
initYaw (void)
{
    //
    // The peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, (GPIO_INT_PIN_0 | GPIO_INT_PIN_1));
    GPIOIntTypeSet(GPIO_PORTB_BASE, (GPIO_INT_PIN_0 | GPIO_INT_PIN_1), GPIO_BOTH_EDGES);
    GPIOPadConfigSet(GPIO_PORTB_BASE, (GPIO_INT_PIN_0 | GPIO_INT_PIN_1), GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    //Register the interrupt handler
    GPIOIntRegister (GPIO_PORTB_BASE, YawIntHandler);
    GPIOIntEnable(GPIO_PORTB_BASE, (GPIO_INT_PIN_0 | GPIO_INT_PIN_1));

}

