/*
 * Internal configurations used by the Firmware
 * No user configurable settings here, edit at your own risk
 */

#define CFG_PROTOCOL_VERSION "1.0"
#define CFG_FIRMWARE_NAME "Marlin V1 like, SRAMP V1.0"

#define CFG_MAX_CMD_SIZE 96 //gcode line size per standard
#define CFG_BUFSIZE 4 //gcode command buffer

#define CFG_BAUDRATE 250000


// THE BLOCK_BUFFER_SIZE NEEDS TO BE A POWER OF 2, i.g. 8,16,32 because shifts and ors are used to do the ring-buffering.
#define BLOCK_BUFFER_SIZE 16

// Minimum planner junction speed. Sets the default minimum speed the planner plans for at the end
// of the buffer and all stops. This should not be much greater than zero and should only be changed
// if unwanted behavior is observed on a user's machine when running at very slow speeds.
#define MINIMUM_PLANNER_SPEED 0.05// (mm/sec)

#define CFG_MAX_STEP_FREQUENCY 40000 // Max step frequency for Ultimaker (5000 pps / half step)

// MS1 MS2 Stepper Driver Microstepping mode table
#define MICROSTEP1 LOW,LOW
#define MICROSTEP2 HIGH,LOW
#define MICROSTEP4 LOW,HIGH
#define MICROSTEP8 HIGH,HIGH
#define MICROSTEP16 HIGH,HIGH


//default stepper release if idle
#define CFG_MAX_IDLE_TIME 60

// Arc interpretation settings:
#define MM_PER_ARC_SEGMENT 1
#define N_ARC_CORRECTION 25
