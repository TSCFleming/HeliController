/*
 * Display_Screens.h
 *
 *  Last Modified on : 12/05/2023
 *      Author: Thomas Fleming & Jonathan London
 */

#ifndef DISPLAY_SCREENS_H_
#define DISPLAY_SCREENS_H_

#include <stdint.h>
#include <stdbool.h>

//
// Constants
//
#define oneV_ADC 1250
#define NO_SLOTS 112


//*****************************************************************************
//
// Function to intialise the OLED Display
//
//*****************************************************************************
void initDisplay (void);


//*****************************************************************************
//
// Function to display the actual and disired Yaw degree and Alt percentages,
// along with the duty cycles of the PWM sent to each rotor and the mode of the
// helicopter
// This information is also printed to the serial output using the UART Module
//
//*****************************************************************************
void displayYawAltVal();


//*****************************************************************************
//
// Function to display a blank screen
// NOTE: Currently not used, but might have use if program ever changes.
//
//***************************************************************************
void blankDisplay (void);


//*****************************************************************************
//
// Function to keep yaw within the 180 and -180 boundarys
//
//*****************************************************************************
void yawBoundary(void);

#endif /* DISPLAY_SCREENS_H_ */
