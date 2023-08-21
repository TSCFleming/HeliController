# Helicopter Project

Kernal.c is the main control file for the project

## OrbitOLED
ChrFont0.c is slightly modified from the original to get a degree symbol on the display.
    To use the original OrbitOLED files, in the Display_Screens.c file, do these changes...

Line 70:
```c
usnprintf (string, sizeof(string), "Yaw %3d.%1d(%3d)De", yawValDeg, degreeDecimal, desiredYawVal); //Needs to be uncommented
```


Line 71:
```c 
usnprintf (string, sizeof(string), "Yaw %3d.%1d(%4d)*", yawValDeg, degreeDecimal, desiredYawVal); //Needs to be commented out
```


## Authors
    Jonathan London - jlo107
    Thomas Fleming - tfl33

