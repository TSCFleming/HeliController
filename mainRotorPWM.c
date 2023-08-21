/*
 * mainRotorPWM.c
 *  Based on pwmGen [from week3lab on Learn]
 *
 *  Last modified on: 16/05/2023
 *      Authors: Thomas Fleming & Jonathan London
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "utils/ustdlib.h"

#include "mainRotorPWM.h"
//
// Globals from main
//
extern int32_t meanAltVal;
extern int32_t mainDuty;
extern int32_t yawValDeg;
extern int32_t zeroVal;
extern bool hovering;
extern int8_t mode;
//
//Globals to module
//
static int32_t mainRotorError;
static int32_t mainRotorPK = 5;
static int32_t mainRotorPControl;
static int32_t mainRotorIK = 250;
static int32_t mainRotorIControl = 1000;
static int32_t mainRotorPWMVal;
static int32_t mainRotorOffset = 25;
static int32_t mainRotorScalar = 50;

//*****************************************************************************
//
// Function to control main rotor during landing Rountine.
//  When reference has been reached, setMainPWM a low value for a smooth landing
//
//*****************************************************************************
bool
mainRotorLandingRoutine(void)
{
    if (abs(yawValDeg) < 3) { //don't lower heli-copter until yaw is ~0 degrees
        setMainPWM(PWM_FREQ_FIXED , 20); // tell heli-copter to approach ground.
    }
    if (meanAltVal >= (zeroVal - 50)) { //when close to ground, turn off main rotor
        return true;
    }
    return false;
}

//*****************************************************************************
//
// Function to control main rotor while in FLYING mode
//  Uses PI control based on error to aproach and maintain desired altitude.
//
//*****************************************************************************
void
mainRotorPWMControl(int32_t desiredAltVal)
{
    if (abs(meanAltVal - desiredAltVal) < 30) {
        hovering = true;
    } else {
        hovering = false;
    }

    mainRotorError = mainRotorScalar * (meanAltVal - desiredAltVal);  //calculate error, use scalar to allow larger IK values (smaller integral increments)

    mainRotorPControl = mainRotorError/ mainRotorPK;  //calculate PControl

    if (abs(mainRotorError) < (125 * mainRotorScalar)){  //if the helicopter is within 10% of the desired altitude, continue updating integral control
        mainRotorIControl += mainRotorError / mainRotorIK;
    }

    mainRotorPWMVal = mainRotorOffset + (mainRotorPControl + mainRotorIControl)/mainRotorScalar; //combine P and I control

    if (mainRotorPWMVal > PWM_MAX) {  // If PWM is beyond allowable limits, set it to the limit value. Or set it to the PWM from PI control
        setMainPWM (PWM_FREQ_FIXED, PWM_MAX);
        mainDuty = PWM_MAX;
    } else if (mainRotorPWMVal < PWM_MIN) {
        setMainPWM (PWM_FREQ_FIXED, PWM_MIN);
        mainDuty = PWM_MIN;
    } else {
        setMainPWM (PWM_FREQ_FIXED, mainRotorPWMVal);
        mainDuty = mainRotorPWMVal;
    }
}

// *********************************************************
// initialisePWM main rotor motor
// *********************************************************
void
initialiseMainPWM (void)
{
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_GPIO);

    GPIOPinConfigure(PWM_MAIN_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_MAIN_GPIO_BASE, PWM_MAIN_GPIO_PIN);

    PWMGenConfigure(PWM_MAIN_BASE, PWM_MAIN_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    // Set the initial PWM parameters
    setMainPWM (PWM_FREQ_FIXED, PWM_DUTY_FIXED);

    PWMGenEnable(PWM_MAIN_BASE, PWM_MAIN_GEN);

    // Disable the output.  Repeat this call with 'true' to turn O/P on.
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, false);
}

// ********************************************************
// Function to set the freq, duty cycle of M0PWM7
// ********************************************************
void
setMainPWM (uint32_t ui32Freq, uint32_t ui32Duty)
{
    // Calculate the PWM period corresponding to the freq.
    uint32_t ui32Period =
        SysCtlClockGet() / PWM_DIVIDER / ui32Freq;

    PWMGenPeriodSet(PWM_MAIN_BASE, PWM_MAIN_GEN, ui32Period);
    PWMPulseWidthSet(PWM_MAIN_BASE, PWM_MAIN_OUTNUM, 
        ui32Period * ui32Duty / 100);
}
