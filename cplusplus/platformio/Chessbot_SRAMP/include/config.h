
/*
 * User Config
 * 
 */


// Define this to set a unique identifier for this printer, (Used by some programs to differentiate between machines)
// You can use an online service to generate a random UUID. (eg http://www.uuidgenerator.net/version4)
#define CFG_MACHINE_UUID "00000000-0000-0000-0000-000000000000"


#define CFG_AXIS_COUNT 2

#define CFG_MIN_SOFTWARE_ENDSTOPS true // If true, axis won't move to coordinates less than HOME_POS.  //robo
#define CFG_MAX_SOFTWARE_ENDSTOPS true  // If true, axis won't move to coordinates greater than the defined lengths below.


#define CFG_X_MAX_FEEDRATE  500
#define CFG_Y_MAX_FEEDRATE  500
#define CFG_Z_MAX_FEEDRATE  5
#define CFG_E_MAX_FEEDRATE  25

// Travel limits after homing
#define CFG_X_MAX_POS 240
#define CFG_X_MIN_POS 0
#define CFG_Y_MAX_POS 254
#define CFG_Y_MIN_POS 0
#define CFG_Z_MAX_POS 240
#define CFG_Z_MIN_POS 0

//BED LEVELING and ENDSTOPS

//Endstop locations
#define CFG_X_HOME_POS 0
#define CFG_Y_HOME_POS 0
#define CFG_Z_HOME_POS 0

// Sets direction of endstops when homing; 1=MAX, -1=MIN
#define CFG_X_HOME_DIR -1 //-1
#define CFG_Y_HOME_DIR -1 //
#define CFG_Z_HOME_DIR -1 //-1 = toward bed, 1 away from bed

// set the homing speeds (mm/min) //robo
#define CFG_HOMING_FEEDRATE {50*60, 50*60, 10*60, 0}  // set the homing speeds (mm/min) //robo


//homing hits the endstop, then retracts by this distance, before it tries to slowly bump again:
#define CFG_X_HOME_RETRACT_MM 5
#define CFG_Y_HOME_RETRACT_MM 5
#define CFG_Z_HOME_RETRACT_MM 1

//Set a probe clearance. Raise Z by 0 or n mm before homing (G28).
#define CFG_Z_HOME_RAISE 4

#define CFG_Z_RAISE_BEFORE_PROBING 15    //How much the extruder will be raised before traveling to the first probing point.
#define CFG_Z_RAISE_BETWEEN_PROBINGS 5  //How much the extruder will be raised when traveling from between next probing points


#define AUTO_BED_LEVELING_GRID
// with AUTO_BED_LEVELING_GRID, the bed is sampled in a
// AUTO_BED_LEVELING_GRID_POINTSxAUTO_BED_LEVELING_GRID_POINTS grid
// and least squares solution is calculated
// Note: this feature occupies 10'206 byte
#ifdef AUTO_BED_LEVELING_GRID

// set the rectangle in which to probe
#define LEFT_PROBE_BED_POSITION 15
#define RIGHT_PROBE_BED_POSITION 205
#define BACK_PROBE_BED_POSITION 230
#define FRONT_PROBE_BED_POSITION 20

// set the number of grid points per dimension
// I wouldn't see a reason to go above 3 (=9 probing points on the bed)
#define AUTO_BED_LEVELING_GRID_POINTS 3


// #else  // not AUTO_BED_LEVELING_GRID
// with no grid, just probe 3 arbitrary points.  A simple cross-product
// is used to esimate the plane of the print bed

#define ABL_PROBE_PT_1_X 15
#define ABL_PROBE_PT_1_Y 15
#define ABL_PROBE_PT_2_X 220
#define ABL_PROBE_PT_2_Y 15
#define ABL_PROBE_PT_3_X 115
#define ABL_PROBE_PT_3_Y 240

#endif // AUTO_BED_LEVELING_GRID

// these are the offsets to the probe relative to the extruder tip (Hotend - Probe)
#define CFG_X_PROBE_OFFSET_FROM_EXTRUDER 0
#define CFG_Y_PROBE_OFFSET_FROM_EXTRUDER 0
#define CFG_Z_PROBE_OFFSET_FROM_EXTRUDER 1

#define CFG_X_MAX_LENGTH (CFG_X_MAX_POS - CFG_X_MIN_POS)
#define CFG_Y_MAX_LENGTH (CFG_Y_MAX_POS - CFG_Y_MIN_POS)
#define CFG_Z_MAX_LENGTH (CFG_Z_MAX_POS - CFG_Z_MIN_POS)


//this feature is meant to avoid Z homing with probe outside the bed area.
#define CFG_Z_SAFE_HOMING_X_POINT (CFG_X_MAX_LENGTH/2)    // X point for Z homing when homing all axis (G28)
#define CFG_Z_SAFE_HOMING_Y_POINT (CFG_Y_MAX_LENGTH/2)    // Y point for Z homing when homing all axis (G28)

// X and Y axis travel speed between probes, in mm/min
#define CFG_XY_TRAVEL_SPEED 8000

//endstops will only be used for homing
#define CFG_ENDSTOPS_ONLY_FOR_HOMING





/*
 * Define initial values for variable that may be modified by GCODE
 * some of these values are stored in eeprom
 */
#define DEFAULT_AXIS_STEPS_PER_UNIT   {80,80,800,723.38}  // default steps per unit for RoBo 3D R1
#define DEFAULT_MAX_FEEDRATE          {500, 500, 5, 25}    // (mm/sec)
#define DEFAULT_MAX_ACCELERATION      {9000,9000,100,10000}    // X, Y, Z, E maximum start speed for accelerated moves. E default values are good for Skeinforge 40+, for older versions raise them a lot.

#define DEFAULT_ACCELERATION          1300    // X, Y, Z and E max acceleration in mm/s^2 for printing moves  /robo
#define DEFAULT_RETRACT_ACCELERATION  3000   // X, Y, Z and E max acceleration in mm/s^2 for retracts

#define DEFAULT_MINIMUMFEEDRATE       0.0     // minimum feedrate
#define DEFAULT_MINTRAVELFEEDRATE     0.0

// The speed change that does not require acceleration (i.e. the software might assume it can be done instantaneously)
#define DEFAULT_XYJERK                17.0    // (mm/sec)
#define DEFAULT_ZJERK                 0.4     // (mm/sec)
#define DEFAULT_EJERK                 5.0    // (mm/sec)

// minimum time in microseconds that a movement needs to take if the buffer is emptied.
#define DEFAULT_MINSEGMENTTIME        20000


/*
 * Temperature & Heater Configuration
 */
#include "config_temp.h"


/*
 * Board Configuration
 */
#include "config_board.h"

/*
 * LCD Configuration
 */
#include "config_lcd.h"


/*
 * Framework specific values and config
 */
#include "config_internals.h"
