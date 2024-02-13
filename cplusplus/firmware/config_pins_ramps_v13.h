#define X_STEP_PIN         54
#define X_DIR_PIN          55
#define X_ENABLE_PIN       38
#define X_MIN_PIN           3
#define X_MAX_PIN           -1 //Ramps Pin 2

#define Y_STEP_PIN         60
#define Y_DIR_PIN          61
#define Y_ENABLE_PIN       56
#define Y_MIN_PIN          14
#define Y_MAX_PIN          -1 //Ramps Pin 15

#define Z_STEP_PIN         46
#define Z_DIR_PIN          48
#define Z_ENABLE_PIN       62
#define Z_MIN_PIN          18
#define Z_MAX_PIN          -1 //Ramps Pin 19

//extruder 1
#define E0_STEP_PIN        26
#define E0_DIR_PIN         28
#define E0_ENABLE_PIN      24

//extruder 2
#define E1_STEP_PIN        36
#define E1_DIR_PIN         34
#define E1_ENABLE_PIN      30



#define Y2_STEP_PIN        36
#define Y2_DIR_PIN         34
#define Y2_ENABLE_PIN      30

#define Z2_STEP_PIN        36
#define Z2_DIR_PIN         34
#define Z2_ENABLE_PIN      30


//SD CARD
//Uses standard Arduino Built-in SD Card library,
//Tested with SD Library Version 1.2.3

#define SDPOWER            	-1
//ChipSelect, Hardware SS Pin on Mega, 10 for Arduino Boards, alwats kept as output
#define SDCS_PIN           	53
#define SD_DETECT_PIN 		-1 //currently not implemented

#define LED_PIN            	13


//ROBO 3D R1 uses a RAMPS 1.4.3 board with different connectors to a
//traditional RAMPS 1.4 board


//On R1+ 1.4.3 board the fan connects to D10, which is a connector labeled "FAN0" next to the leds
//This connector is actually connected to Arduino's PIN 9 like of regular RAMPS
#define FAN_PIN            9  //Cooling Fan, controlled with gcode M106


#define PS_ON_PIN          12   //ATX , awake=LOW, SLEEP=High
#define KILL_PIN           -1

//On R1+ 1.4.3 board the extruder is connected to connector labeled D9.
//This connector behaves like the original D10 of RAMPS and still responds to Arduino's PIN 10
#define HEATER_0_PIN       10 // Extruder Heater
#define HEATER_1_PIN       -1 //none
#define HEATER_BED_PIN 		8 // BED

#define TEMP_0_PIN          13   // ANALOG NUMBERING
#define TEMP_BED_PIN		14   // ANALOG NUMBERING



#ifdef DUAL_X_CARRIAGE

	#define X2_ENABLE_PIN 29
	#define X2_STEP_PIN 25
	#define X2_DIR_PIN 23

#endif //DUAL_X_CARRIAGE
