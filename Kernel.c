/******************************************************************************
 * Kernel.c
 *
 *  Last modified on: 16/05/2023
 *      Author: Thomas Fleming & Jonathan London
 *  
 *  Kernel for the helicopter. After the init cycle, uses a round robin scheduler 
 *  to perform background tasks:
 *   -meanAltValue()
 *   -updateButtons ()
 *   -displayYawAltVal()
 *   -Check integrated FSM to determine and perform movement routines(PI control).
 *
 ***************************************************************************//*
 * 
 * sysTickIntHandler and clockInit() based on:
 * ADCdemo1.c - Simple interrupt driven program which samples with AIN0
 *
 *Author:  P.J. Bones  UCECE
 * Last modified:   8.2.2018
 *
 * *************************************************************************//*
 * Based on the 'convert' series from 2016
 *****************************************************************************/

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
#include "circBufT.h"
#include "stdio.h"
#include "stdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "driverlib/uart.h"
#include "buttons4.h"

#include "switch.h"                   //including our own (project specific) modules
#include "Altitude_and_ADC.h"               
#include "Yaw.h"
#include "Display_Screens.h"
#include "mainRotorPWM.h"
#include "tailRotorPWM.h"
#include "UART.h"

//*****************************************************************************
// Constants
//*****************************************************************************

//Altitude sensor
#define SAMPLE_RATE_HZ 48

//PWM
#define PWM_DIVIDER_CODE   SYSCTL_PWMDIV_4

//*****************************************************************************
// Global variables
//*****************************************************************************

// Altitude_and_ADC.c
circBuf_t g_inBuffer;        // Buffer of size BUF_SIZE integers (sample values)
static uint32_t g_ulSampCnt;    // Counter for the interrupts
int32_t meanAltVal;
int32_t zeroVal;

// Yaw.c
int32_t yawValDeg = 0;
int32_t yawVal = 0;

// MainRotorPWM.c and tailRotorPWM.c
int32_t desiredAltVal = 2500;
int32_t desiredYawVal = 0;

int32_t mainDuty = 0; //Copy PWM signals to monitor during debugging
int32_t tailDuty = 0;

//Finite State machine across kernel.c and others
enum switchStates {LANDED = 0, CALIBRATION, FLYING, LANDING};
int8_t mode = LANDED;  //stores FSM mode, initialises in LANDED state
bool hovering = false; //CALIBRATION
bool Calibrated = false;  //CALIBRATION
bool yawRefTrigger = false; //CALIBRATION
bool landed = true; //LANDING
bool landing_acknowledged = false; //LANDED
bool landing_switch = 0; //FLYING | LANDED

//*****************************************************************************
// The interrupt handler for the for SysTick interrupt.
//*****************************************************************************
void
SysTickIntHandler(void)
{
    //
    // Initiate a conversion
    //
    ADCProcessorTrigger(ADC0_BASE, 3);
    g_ulSampCnt++;
}

//*****************************************************************************
// Initialisation functions for the clock (incl. SysTick), ADC, display
//*****************************************************************************
void
initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);

    // Set the PWM clock rate (using the prescaler)
    SysCtlPWMClockSet(PWM_DIVIDER_CODE);

    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}


//*****************************************************************************
//
// Main Function
//
//*****************************************************************************
int
main(void)
  {
    //Initialisation cycle
    initClock ();
    initADC ();
    initYaw ();
    initDisplay ();
    initButtons ();
    initCircBuf (&g_inBuffer, BUF_SIZE);
    initialiseMainPWM ();
    initialiseTailPWM ();
    initYawRef ();
    initialiseUSB_UART ();
    initSwitch ();
    IntMasterEnable(); // Enable interrupts
    SysCtlDelay (SysCtlClockGet() / 6); // needs time to fill ADC buffer
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, true);
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, true);

    zeroPercentFunc ();

    while (1)
    {
            meanAltValue(); // Read ADC
            updateButtons (); // Poll buttons
            displayYawAltVal(); // Refresh OLED display

            if (mode == LANDED) { 
                landing_switch = checkSwitch();
                if (landing_switch == 0) {
                    landing_acknowledged = true; //landing is acknowledged by returning landing switch to "land" after landing is complete
                }
                if (landing_switch == 1 && landing_acknowledged) { //if landing is not acknowledged, helicopter will not takeoff. [prevents immediate takeoff if landing switch was changed mid landing]
                    if (!Calibrated){
                    mode = CALIBRATION; //Only run calibration on the first takeoff
                    } else {
                    mode = FLYING;
                    }
                    landed = false;
                    landing_acknowledged = false;
                }
            }

            if (mode == CALIBRATION) {
                mainRotorPWMControl(zeroVal - 500); //set arbitrary hover point
                if (hovering) { //when stable hovering is acheived, rotate slowly until ref is found
                    yawRefTrigger = GPIOPinRead(CALLIBRATE_PORT_BASE, GPIO_INT_PIN_4); //poll the yaw ref pin
                    if (yawRefTrigger) {
                        setTailPWM (PWM_FREQ_FIXED, 50);
                        tailDuty = 50;
                    } else {  //when triggered, set yaw, desiredYaw to 0, end calibration cycle
                        yawVal = 0;
                        Calibrated = true;
                        desiredYawVal = 0;
                    }
                } else {
                    tailRotorPWMControl(0); //stay at the current yaw while ascending
                }
                if (Calibrated) {
                    mode = FLYING;
                }
            }

            if (mode == FLYING) {
                landing_switch = checkSwitch(); //check landing switch
                if ((checkButton (UP) == PUSHED) && (desiredAltVal > 1250)) {  //update desired position [Altitude and yaw] based on buttons
                    desiredAltVal -= 125;
                }
                if ((checkButton (DOWN) == PUSHED) && (desiredAltVal < 2500)) {
                    desiredAltVal += 125;
                }
                if (checkButton (RIGHT) == PUSHED) {
                    desiredYawVal += 15;
                }
                if (checkButton (LEFT) == PUSHED) {
                    desiredYawVal -= 15;
                }

                mainRotorPWMControl(desiredAltVal); //update PI control for main rotor
                tailRotorPWMControl(desiredYawVal); //and tail rotor

                if (landing_switch == 0) {
                    mode = LANDING;
                }
            }

            if (mode == LANDING) {
                tailRotorPWMControl(0);
                landed = mainRotorLandingRoutine(); //landing routine defined in mainRotorPWM.c
                if (landed) { //when landing is complete, turn off rotors.
                    mode = LANDED;
                    setMainPWM (PWM_FREQ_FIXED, 0);
                    setTailPWM (PWM_FREQ_FIXED, 0);
                    mainDuty = 0; //update watch expression variable
                    tailDuty = 0;
                }
            }
    }
}
