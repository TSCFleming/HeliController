/*
 * Yaw.h
 *
 *  Last modified on: 02/05/2023
 *      Authors: Jonathan London, Thomas Fleming
 */

#ifndef YAW_H_
#define YAW_H_

#include <stdint.h>
#include <stdbool.h>

//*****************************************************************************
// The initialiser for the YAW interrupt.
//  Register the interrupt handler
//  Enable and configure the GPIO associated with yaw sensors
//*****************************************************************************
void initYaw (void); 


//*****************************************************************************
// The handler for the YAW interrupt.
//  Reads the two yaw sensors and uses a simple FSM to perform
//  quadrature decoding.
//*****************************************************************************
void YawIntHandler(void);


#endif /* YAW_H_ */
