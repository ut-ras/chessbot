/*
 * Config temperature related values
 *
 */


// Incrementing this by 1 will double the software PWM frequency,
// affecting heaters, and the fan if FAN_SOFT_PWM is enabled.
// However, control resolution will be halved for each increment;
// at zero value, there are 128 effective control positions.
#define CFG_TEMP_SOFT_PWM_SCALE 0


#define TEMP_SENSOR_0 1    	// 1 is 100k thermistor - best choice for EPCOS 100k (4.7k pullup)
#define TEMP_SENSOR_1 0 	// 0 is not used
#define TEMP_SENSOR_2 0		// 0 is not used



// This makes temp sensor 1 a redundant sensor for sensor 0. If the temperatures difference between these sensors is to high the print will be aborted.
//#define TEMP_SENSOR_1_AS_REDUNDANT
#define MAX_REDUNDANT_TEMP_SENSOR_DIFF 10

//Check for thermistor temerature freefall during the print,
//This usually indicates that the thermistor has been removed from the head.
#define CFG_TEMP_SENSOR_DISCONNECT_CHECK 2
#define CFG_TEMP_SENSOR_DISCONNECT_LIMIT 20

// Actual temperature must be close to target for this long before M109 returns success
#define TEMP_RESIDENCY_TIME 2  // (seconds) //CTK
#define TEMP_HYSTERESIS 3       // (degC) range of +/- temperatures considered "close" to the target one
#define TEMP_WINDOW     5       // (degC) Window around target to start the residency timer x degC early. //CTK 1

// The minimal temperature defines the temperature below which the heater will not be enabled It is used
// to check that the wiring to the thermistor is not broken.
// Otherwise this would lead to the heater being powered on all the time.
#define HEATER_0_MINTEMP 5
#define HEATER_1_MINTEMP 5
#define HEATER_2_MINTEMP 5


#define HEATER_0_MAXTEMP 290    //robo
#define HEATER_1_MAXTEMP 290
#define HEATER_2_MAXTEMP 290


#define PIDTEMP
#define CFG_BANG_MAX 255 // limits current to nozzle while in bang-bang mode; 255=full current
#define CFG_PID_MAX 255 // limits current to nozzle while PID is active (see PID_FUNCTIONAL_RANGE below); 255=full current

#define CFG_PID_FUNCTIONAL_RANGE 10 // If the temperature difference between the target temperature and the actual temperature
// is more then PID_FUNCTIONAL_RANGE then the PID will be shut off and the heater will be set to min/max.
#define PID_INTEGRAL_DRIVE_MAX 255  //limit for the integral term
#define K1 0.95 //smoothing factor within the PID
#define PID_dT ((OVERSAMPLENR * 8.0)/(F_CPU / 64.0 / 256.0)) //sampling period of the temperature routine


// this adds an experimental additional term to the heating power, proportional to the extrusion speed.
// if Kc is chosen well, the additional required power due to increased melting should be compensated.
#define PID_ADD_EXTRUSION_RATE
#define  DEFAULT_Kc (1) //heating power=Kc*(e_speed)


// If you are using a pre-configured hotend then you can use one of the value sets by uncommenting it
// Ultimaker
#define  DEFAULT_Kp 22.2
#define  DEFAULT_Ki 1.08
#define  DEFAULT_Kd 114

// MakerGear
//    #define  DEFAULT_Kp 7.0
//    #define  DEFAULT_Ki 0.1
//    #define  DEFAULT_Kd 12

// Mendel Parts V9 on 12V
//    #define  DEFAULT_Kp 63.0
//    #define  DEFAULT_Ki 2.25
//    #define  DEFAULT_Kd 440



/*
* BED
*/

#define TEMP_SENSOR_BED 1 // 1 is 100k thermistor - best choice for EPCOS 100k (4.7k pullup)

// This sets the max power delivered to the bed, and replaces the HEATER_BED_DUTY_CYCLE_DIVIDER option.
// all forms of bed control obey this (PID, bang-bang, bang-bang with hysteresis)
// setting this to anything other than 255 enables a form of PWM to the bed just like HEATER_BED_DUTY_CYCLE_DIVIDER did,
// so you shouldn't use it unless you are OK with PWM on your bed.  (see the comment on enabling PIDTEMPBED)
#define MAX_BED_POWER 255 // limits duty cycle to bed; 255=full current
#define CFG_BED_MINTEMP 5
#define CFG_BED_MAXTEMP 110

#define CFG_BED_CHECK_INTERVAL 5000 //ms between checks in bang-bang control


//These defines help to calibrate the AD595 sensor in case you get wrong temperature measurements.
//The measured temperature is defined as "actualTemp = (measuredTemp * TEMP_SENSOR_AD595_GAIN) + TEMP_SENSOR_AD595_OFFSET"
#define TEMP_SENSOR_AD595_OFFSET 0.0
#define TEMP_SENSOR_AD595_GAIN   1.0


//End of user options

// AUTO CONFIG OPTIONS

#if TEMP_SENSOR_0 > 0
	#define THERMISTORHEATER_0 TEMP_SENSOR_0
	#define HEATER_0_USES_THERMISTOR
#endif
#if TEMP_SENSOR_1 > 0
	#define THERMISTORHEATER_1 TEMP_SENSOR_1
	#define HEATER_1_USES_THERMISTOR
#endif
#if TEMP_SENSOR_2 > 0
	#define THERMISTORHEATER_2 TEMP_SENSOR_2
	#define HEATER_2_USES_THERMISTOR
#endif
#if TEMP_SENSOR_BED > 0
	#define THERMISTORBED TEMP_SENSOR_BED
	#define BED_USES_THERMISTOR
#endif
#if TEMP_SENSOR_0 == -1
	#define HEATER_0_USES_AD595
#endif
#if TEMP_SENSOR_1 == -1
	#define HEATER_1_USES_AD595
#endif
#if TEMP_SENSOR_2 == -1
	#define HEATER_2_USES_AD595
#endif
#if TEMP_SENSOR_BED == -1
	#define BED_USES_AD595
#endif
#if TEMP_SENSOR_0 == -2
	#define HEATER_0_USES_MAX6675
#endif
#if TEMP_SENSOR_0 == 0
	#undef HEATER_0_MINTEMP
	#undef HEATER_0_MAXTEMP
#endif
#if TEMP_SENSOR_1 == 0
	#undef HEATER_1_MINTEMP
	#undef HEATER_1_MAXTEMP
#endif
#if TEMP_SENSOR_2 == 0
	#undef HEATER_2_MINTEMP
	#undef HEATER_2_MAXTEMP
#endif
#if TEMP_SENSOR_BED == 0
	#undef BED_MINTEMP
	#undef BED_MAXTEMP
#endif

