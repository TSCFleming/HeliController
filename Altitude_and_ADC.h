#ifndef ALTITUDE_AND_ADC_H_
#define ALTITUDE_AND_ADC_H_

/*
 * Altitude_and_ADC.h
 *
 *
 *  Last modified on: 12/05/2023
 *      Authors: Thomas Fleming & Jonathan London
 */


#include <stdint.h>
#include <stdbool.h>

//
// Constants
//
#define BUF_SIZE 8


//*****************************************************************************
// Function to calculate the mean adc value
//*****************************************************************************
void meanAltValue (void);

//*****************************************************************************
// Function to set the global variable zeroVal to the current ADC value.
//*****************************************************************************
void zeroPercentFunc (void);

//*****************************************************************************
// The handler for the ADC_conversion_complete interrupt.
// Writes to the circular buffer.
//*****************************************************************************
void ADCIntHandler(void);

//*****************************************************************************
// The initialiser for the Altitude/ADC interupt from ADC0
//*****************************************************************************
void initADC (void);





#endif /* ALTITUDE_AND_ADC_H_ */
