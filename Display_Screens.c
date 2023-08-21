/*
 * Display_Screens.c
 *
 *  Last Modified on : 16/05/2023
 *      Author: Thomas Fleming & Jonathan London
 */

#include <stdint.h>
#include <stdbool.h>
#include "utils/ustdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"

#include "switch.h"
#include "Display_Screens.h"
#include "UART.h"

//
// External Globals from main module
//
extern int32_t zeroVal;
extern int32_t meanAltVal;
extern int32_t yawVal;
extern int32_t yawValDeg;
extern int32_t desiredYawVal;
extern int32_t desiredAltVal;
extern int32_t mainDuty;
extern int32_t tailDuty;
extern int8_t mode;

//
// Globals to module
//
static const char *HELI_STATE[] = {
   "Landed", "Calibration", "Flying", "Landing"
};

//*****************************************************************************
//
// Function to intialise the OLED Display using the OLEDInitialise() function
//
//*****************************************************************************
void
initDisplay (void)
{
    // intialise the Orbit OLED display
    OLEDInitialise ();
}


//*****************************************************************************
//
// Function to display the actual and disired Yaw degree and Alt percentages,
// along with the duty cycles of the PWM sent to each rotor and the mode of the
// helicopter
// This information is also printed to the serial output using the UART Module
//
//*****************************************************************************
void
displayYawAltVal()
{
    char string[17];  // 16 characters across the display
    int32_t altPercentVal = (100 * (zeroVal - meanAltVal) + oneV_ADC/2)/(oneV_ADC);
    int32_t desiredAltPer = (100 * (zeroVal - desiredAltVal) + oneV_ADC/2)/(oneV_ADC);
    yawBoundary();
    int32_t degreeVal2DP = (10*yawVal*360)/(4 * NO_SLOTS);
    yawValDeg = degreeVal2DP / 10;
    int32_t degreeDecimal = abs(degreeVal2DP) % 10;

    //alternative string for first line of display that doesnt use the special degree symbol
    //usnprintf (string, sizeof(string), "Yaw %3d.%1d(%3d)De", yawValDeg, degreeDecimal, desiredYawVal); // UNCOMMENT IF WANTED, then comment out next line
    usnprintf (string, sizeof(string), "Yaw %3d.%1d(%4d)*", yawValDeg, degreeDecimal, desiredYawVal); //special charater for own OLED Module used for degree symbol
    OLEDStringDraw (string, 0, 0);
    usnprintf (string, sizeof(string), "Alt %3d  (%4d)%%", altPercentVal, desiredAltPer);
    OLEDStringDraw (string, 0, 1);
    usnprintf (string, sizeof(string), "Main %2d Tail %2d  ", mainDuty, tailDuty);
    OLEDStringDraw (string, 0, 2);
    usnprintf (string, sizeof(string), "Mode %s            ", HELI_STATE[mode]);
    OLEDStringDraw (string, 0, 3);

    char string2[100];
    usnprintf (string2, sizeof(string2), "----------------\r\nAlt = %3d(%3d)%%\r\nYaw = %3d(%3d)deg\r\nMain %3d Tail %3d\r\nMode = %s\r\n",
               altPercentVal, desiredAltPer, yawValDeg, desiredYawVal, mainDuty, tailDuty, HELI_STATE[mode]);
    UARTSend(string2);
}



//*****************************************************************************
//
// Function to display a blank screen
// NOTE: Currently not used, but might have use if program ever changes.
//
//*****************************************************************************
void
blankDisplay (void)
{
    uint16_t i;
    for (i = 0; i < 4; i++)
        OLEDStringDraw ("                ", 0, i);
}


//*****************************************************************************
//
// Function to keep yaw within the 180 and -180 boundarys
//
//*****************************************************************************
void
yawBoundary (void)
{
    if (yawVal > 224)
    {
        yawVal -= 448;
        desiredYawVal -= 360;
    } else if (yawVal < -224) {
        yawVal += 448;
        desiredYawVal += 360;
    }
}
