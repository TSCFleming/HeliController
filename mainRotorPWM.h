#ifndef MAIN_ROTOR_PWM_H_
#define MAIN_ROTOR_PWM_H_

/*
 * mainRotorPWM.h
 * 
 *
 *  Created on: 15/05/2023
 *      Author: Thomas Fleming & Jonathan London
 */

#include <stdint.h>
#include <stdbool.h>

//
// Constants
//   Values, for both rotors
#define PWM_FREQ_FIXED    250
#define PWM_DUTY_FIXED    0 // Starting Duty Cycle
#define PWM_DIVIDER  4
#define PWM_MAX      70 
#define PWM_MIN      5

//  ---Main Rotor PWM: PC5, J4-05
#define PWM_MAIN_BASE	     PWM0_BASE
#define PWM_MAIN_GEN         PWM_GEN_3
#define PWM_MAIN_OUTNUM      PWM_OUT_7
#define PWM_MAIN_OUTBIT      PWM_OUT_7_BIT
#define PWM_MAIN_PERIPH_PWM	 SYSCTL_PERIPH_PWM0
#define PWM_MAIN_PERIPH_GPIO SYSCTL_PERIPH_GPIOC
#define PWM_MAIN_GPIO_BASE   GPIO_PORTC_BASE
#define PWM_MAIN_GPIO_CONFIG GPIO_PC5_M0PWM7
#define PWM_MAIN_GPIO_PIN    GPIO_PIN_5


//*****************************************************************************
// Function to control main rotor during landing Rountine.
//  When reference has been reached, setMainPWM a low value for a smooth landing
//*****************************************************************************
bool mainRotorLandingRoutine(void);

//*****************************************************************************
// Function to control main rotor while in FLYING mode
//  Uses PI control based on error to aproach and maintain desired altitude.
//*****************************************************************************
void mainRotorPWMControl(int32_t desiredAltVal);

// *********************************************************
// initialisePWM main rotor motor
// *********************************************************
void initialiseMainPWM(void);

// ********************************************************
// Function to set the freq, duty cycle of M0PWM7
// ********************************************************
void setMainPWM (uint32_t ui32Freq, uint32_t ui32Duty);

#endif /* MAIN_ROTOR_PWM_H_ */
