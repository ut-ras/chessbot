

#ifndef temperature_h
#define temperature_h

#include "thermistor_table_100K.h"

//===========================================================================
//=============================public variables============================
//===========================================================================
extern int target_temperature[EXTRUDERS];
extern float hot_temperature[EXTRUDERS];

extern int current_temperature_raw[EXTRUDERS];
extern float current_temperature[EXTRUDERS];
extern int current_temperature_bed_raw;

extern int target_temperature_bed;
extern float current_temperature_bed;


extern unsigned char soft_pwm_bed;
extern float Kp,Ki,Kd,Kc;


float scalePID_i(float i);
float scalePID_d(float d);
float unscalePID_i(float i);
float unscalePID_d(float d);

void temp_init();
void temp_checkrange();
void temp_error_trigger(int err);
void temp_heaters_disable();
void temp_bed_disable();
void temp_checkrange();
void temp_checkheaters(); //manage_heater()
void temp_updatePID();

//high level conversion routines, for use outside of temperature.cpp
//inline so that there is no performance decrease.
//deg=degreeCelsius

FORCE_INLINE float degHotend(uint8_t extruder) {
	return current_temperature[extruder];
};

FORCE_INLINE float degBed() {
	return current_temperature_bed;
};

FORCE_INLINE float degTargetHotend(uint8_t extruder) {
	return target_temperature[extruder];
};

FORCE_INLINE float degTargetBed() {
	return target_temperature_bed;
};

FORCE_INLINE void setTargetHotend(const float &celsius, uint8_t extruder) {
	target_temperature[extruder] = celsius;
};

FORCE_INLINE void setTargetBed(const float &celsius) {
	target_temperature_bed = celsius;
};

FORCE_INLINE bool isHeatingHotend(uint8_t extruder){
	return target_temperature[extruder] > current_temperature[extruder];
};

FORCE_INLINE bool isHeatingBed() {
	return target_temperature_bed > current_temperature_bed;
};

FORCE_INLINE bool isCoolingHotend(uint8_t extruder) {
	return target_temperature[extruder] < current_temperature[extruder];
};

FORCE_INLINE bool isCoolingBed() {
	return target_temperature_bed < current_temperature_bed;
};

#define degHotend0() degHotend(0)
#define degTargetHotend0() degTargetHotend(0)
#define setTargetHotend0(_celsius) setTargetHotend((_celsius), 0)
#define isHeatingHotend0() isHeatingHotend(0)
#define isCoolingHotend0() isCoolingHotend(0)

#if EXTRUDERS > 1
	#define degHotend1() degHotend(1)
	#define degTargetHotend1() degTargetHotend(1)
	#define setTargetHotend1(_celsius) setTargetHotend((_celsius), 1)
	#define isHeatingHotend1() isHeatingHotend(1)
	#define isCoolingHotend1() isCoolingHotend(1)
#else
	#define setTargetHotend1(_celsius) do{}while(0)
#endif



void temp_updatePID();

#endif  //temperature_h
