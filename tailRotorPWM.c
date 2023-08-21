/*
 * tailRotorPWM.c
 *
 *  Last Modified on : 15/05/2023
 *      Author: Thomas Fleming & Jonathan London
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/pin_map.h" //Needed for pin configure
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "utils/ustdlib.h"

#include "tailRotorPWM.h"
#include "mainRotorPWM.h"

//
// External Globals from main module
//
extern int32_t yawValDeg;
extern int32_t yawVal;
extern int32_t tailDuty;
extern int32_t mainDuty;

//
// Globals to module
//
static int32_t previousError; //for derivative control
static int32_t tailRotorError;
static int32_t tailRotorPK = 9;
static int32_t tailRotorPControl;
static int32_t tailRotorIK = 60;
static int32_t tailRotorIControl = 400;
static int32_t tailRotorDK = 1;
static int32_t tailRotorDControl = 0;
static int32_t tailRotorPWMVal;
static int32_t tailRotorOffset = 25;
static int32_t tailRotorScalar = 40;


//**********************************************************************
// Function to control the PWM of the the tail rotor output while flying.
// Uses PID control based on current and desired altitude
// to acheive the desired altitude whith the helicopter
//**********************************************************************
void
tailRotorPWMControl(int32_t desiredYaw)
{
    previousError = tailRotorError; // Sets the previous error to previousError for the derivative control
    tailRotorError = tailRotorScalar * (yawValDeg - desiredYaw); // Calculates error

    tailRotorPControl = (tailRotorError * tailRotorPK)/10; // Calculates P Control

    if (abs(tailRotorPControl) < 10 * tailRotorScalar) {
        tailRotorIControl += tailRotorError / tailRotorIK; // Calculates I control
    }

    if (abs(tailRotorError) > 2 * tailRotorScalar) {
        tailRotorDControl = (previousError - tailRotorError) / tailRotorDK; // Calculates D control
    }


    // Calculates the PWM Value using the P,I and D
    tailRotorPWMVal = tailRotorOffset + (tailRotorPControl + tailRotorIControl + tailRotorDControl)/tailRotorScalar;

    // Sets the PWM Output to the calculated value(or the min and max of the motors)
    if (tailRotorPWMVal > PWM_MAX) {
        setTailPWM (PWM_FREQ_FIXED, PWM_MAX);
        tailDuty = PWM_MAX;
    } else if (tailRotorPWMVal < PWM_MIN) {
        setTailPWM (PWM_FREQ_FIXED, PWM_MIN);
        tailDuty = PWM_MIN;
    } else {
        setTailPWM (PWM_FREQ_FIXED, tailRotorPWMVal);
        tailDuty = tailRotorPWMVal;
    }
}

//*********************************************************
// initialise Yaw callibration signal
//*********************************************************
void
initYawRef (void)
{
    SysCtlPeripheralEnable(CALLIBRATE_PERIPH);
    GPIOPinTypeGPIOInput(CALLIBRATE_PORT_BASE, GPIO_PIN_4);
    GPIOPadConfigSet(CALLIBRATE_PORT_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

//********************************************************
// initialisePWM tail rotor motor
//********************************************************
void
initialiseTailPWM (void)
{
    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_GPIO);
    GPIOPinConfigure(PWM_TAIL_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_TAIL_GPIO_BASE, PWM_TAIL_GPIO_PIN);
    PWMGenConfigure(PWM_TAIL_BASE, PWM_TAIL_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    // Set the initial PWM parameters
    setTailPWM (PWM_FREQ_FIXED, PWM_DUTY_FIXED);
    PWMGenEnable(PWM_TAIL_BASE, PWM_TAIL_GEN);
    // Disable the output.  Repeat this call with 'true' to turn O/P on.
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, false);
}

//*******************************************************
// Function to set the freq and duty cycle of M1PWM5
//*******************************************************
void
setTailPWM (uint32_t ui32Freq, uint32_t ui32Duty)
{
    // Calculate the PWM period corresponding to the freq.
    uint32_t ui32Period =
        SysCtlClockGet() / PWM_DIVIDER / ui32Freq;
    PWMGenPeriodSet(PWM_TAIL_BASE, PWM_TAIL_GEN, ui32Period);

    PWMPulseWidthSet(PWM_TAIL_BASE, PWM_TAIL_OUTNUM,
        ui32Period * ui32Duty / 100);
}
