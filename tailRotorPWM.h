/*
 * tailRotorPWM.h
 *
 *  Last Modified on : 15/05/2023
 *      Author: Thomas Fleming & Jonathan London
 */
#ifndef TAIL_ROTOR_PWM_H_
#define TAIL_ROTOR_PWM_H_

#include <stdint.h>
#include <stdbool.h>

//
// Constants
//  ---Tail Rotor PWM: PF1, -11
#define PWM_TAIL_BASE        PWM1_BASE
#define PWM_TAIL_GEN         PWM_GEN_2
#define PWM_TAIL_OUTNUM      PWM_OUT_5
#define PWM_TAIL_OUTBIT      PWM_OUT_5_BIT
#define PWM_TAIL_PERIPH_PWM  SYSCTL_PERIPH_PWM1
#define PWM_TAIL_PERIPH_GPIO SYSCTL_PERIPH_GPIOF
#define PWM_TAIL_GPIO_BASE   GPIO_PORTF_BASE
#define PWM_TAIL_GPIO_CONFIG GPIO_PF1_M1PWM5
#define PWM_TAIL_GPIO_PIN    GPIO_PIN_1
//  ---Yaw callibration signal
#define CALLIBRATE_PERIPH  SYSCTL_PERIPH_GPIOC
#define CALLIBRATE_PORT_BASE  GPIO_PORTC_BASE
#define CALLIBRATE_PIN  GPIO_PIN_4


//***********************************************************************
// Function to control the PWM of the the tail rotor output while flying.
// Uses PID control based on current and desired altitude
// to acheive the desired altitude whith the helicopter
//***********************************************************************
void tailRotorPWMControl(int32_t desiredYawVal);


//*********************************************************
// initialise Yaw callibration signal
//*********************************************************
void initYawRef (void);


//********************************************************
// initialisePWM tail rotor motor
//********************************************************
void initialiseTailPWM(void);


//*******************************************************
// Function to set the freq and duty cycle of M1PWM5
//*******************************************************
void setTailPWM (uint32_t ui32Freq, uint32_t ui32Duty);

#endif /* TAIL_ROTOR_PWM_H_ */
