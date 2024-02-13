#ifndef printer_board_h
#define printer_board_h

#define EXTRUDERS 1


//SD Card Support
#define CFG_SDSUPPORT //enable sd card support
#define CFG_SD_DETECTINVERTED

#define INVERT_X_DIR true    // for Mendel set to false, for Orca set to true
#define INVERT_Y_DIR false    // for Mendel set to true, for Orca set to false
#define INVERT_Z_DIR true     // for Mendel set to false, for Orca set to true
#define INVERT_E0_DIR false   // for direct drive extruder v9 set to true, for geared extruder set to false
#define INVERT_E1_DIR false    // for direct drive extruder v9 set to true, for geared extruder set to false
#define INVERT_E2_DIR false   // for direct drive extruder v9 set to true, for geared extruder set to false




// Disables axis when it's not being used.
#define CFG_DISABLE_X false
#define CFG_DISABLE_Y false
#define CFG_DISABLE_Z true    //robo
#define CFG_DISABLE_E false // For all extruders

//On a normal pololu driver the enable pin is inverted (is enabled with LOW and disable with HIGH)
//For Inverting Stepper Enable Pins (Active Low) use 0, Non Inverting (Active High) use 1
#define CFG_X_ENABLE_ON 0
#define CFG_Y_ENABLE_ON 0
#define CFG_Z_ENABLE_ON 0
#define CFG_E_ENABLE_ON 0 // For all extruders

//By default pololu step drivers require an active high signal. However, some high power drivers require an active low signal as step.
#define CFG_INVERT_X_STEP_PIN false
#define CFG_INVERT_Y_STEP_PIN false
#define CFG_INVERT_Z_STEP_PIN false
#define CFG_INVERT_E_STEP_PIN false

#define ENDSTOPPULLUPS
#define ENDSTOPPULLUP_XMAX
#define ENDSTOPPULLUP_YMAX
#define ENDSTOPPULLUP_ZMAX
#define ENDSTOPPULLUP_XMIN
#define ENDSTOPPULLUP_YMIN
#define ENDSTOPPULLUP_ZMIN

// The pullups are needed if you directly connect a mechanical endswitch between the signal and ground pins.
const bool X_MIN_ENDSTOP_INVERTING = false; // set to true to invert the logic of the endstop. //robo
const bool Y_MIN_ENDSTOP_INVERTING = false; // set to true to invert the logic of the endstop. //robo
const bool Z_MIN_ENDSTOP_INVERTING = true; // set to true to invert the logic of the endstop. //robo
const bool X_MAX_ENDSTOP_INVERTING = false; // set to true to invert the logic of the endstop. //robo
const bool Y_MAX_ENDSTOP_INVERTING = false; // set to true to invert the logic of the endstop. //robo
const bool Z_MAX_ENDSTOP_INVERTING = false; // set to true to invert the logic of the endstop. //robo


#include "config_pins_ramps_v13.h"

#endif
