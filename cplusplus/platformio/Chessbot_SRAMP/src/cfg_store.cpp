#include "firmware.h"


void _EEPROM_writeData(int &pos, uint8_t* value, uint8_t size){
    do{
        eeprom_write_byte((unsigned char*)pos, *value);
        pos++;
        value++;
    }while(--size);
}
#define EEPROM_WRITE_VAR(pos, value) _EEPROM_writeData(pos, (uint8_t*)&value, sizeof(value))
void _EEPROM_readData(int &pos, uint8_t* value, uint8_t size){
    do{
        *value = eeprom_read_byte((unsigned char*)pos);
        pos++;
        value++;
    }while(--size);
}
#define EEPROM_READ_VAR(pos, value) _EEPROM_readData(pos, (uint8_t*)&value, sizeof(value))
//======================================================================================




#define EEPROM_OFFSET 100


// IMPORTANT:  Whenever there are changes made to the variables stored in EEPROM
// in the functions below, also increment the version number. This makes sure that
// the default values are used whenever there is a change to the data, to prevent
// wrong data being written to the variables.
// ALSO:  always make sure the variables in the Store and retrieve sections are in the same order.

#define EEPROM_VERSION "V50"



void config_StoreSettings(){
	char ver[4]= "000";
	int i=EEPROM_OFFSET;
	
	EEPROM_WRITE_VAR(i,ver); // invalidate data first
	EEPROM_WRITE_VAR(i,axis_steps_per_unit);
	EEPROM_WRITE_VAR(i,max_feedrate);
	EEPROM_WRITE_VAR(i,max_acceleration_units_per_sq_second);
	EEPROM_WRITE_VAR(i,acceleration);
	EEPROM_WRITE_VAR(i,retract_acceleration);
	EEPROM_WRITE_VAR(i,minimumfeedrate);
	EEPROM_WRITE_VAR(i,mintravelfeedrate);
	EEPROM_WRITE_VAR(i,minsegmenttime);
	EEPROM_WRITE_VAR(i,max_xy_jerk);
	EEPROM_WRITE_VAR(i,max_z_jerk);
	EEPROM_WRITE_VAR(i,max_e_jerk);
	EEPROM_WRITE_VAR(i,state.offsets);
	EEPROM_WRITE_VAR(i,state.zoffset);
	
	EEPROM_WRITE_VAR(i,Kp);
    EEPROM_WRITE_VAR(i,Ki);
    EEPROM_WRITE_VAR(i,Kd);
	
	char ver2[4]=EEPROM_VERSION;
  	i=EEPROM_OFFSET;
  	EEPROM_WRITE_VAR(i,ver2); // validate data
	
	serial_print("echo: DEBUG|CONFIG|saved to EEPROM");
}




void config_PrintSettings(){  // Always have this function, even with EEPROM_SETTINGS disabled, the current values will be shown
	serial_println("echo: INFO|EEPROM VALUES|START");
	serial_printi("echo: M92 X%f Y%f Z%f E%f\n", axis_steps_per_unit[0], axis_steps_per_unit[1], axis_steps_per_unit[2], axis_steps_per_unit[3]);
	serial_printi("echo: M203 X%f Y%f Z%f E%f\n", max_feedrate[0], max_feedrate[1], max_feedrate[2], max_feedrate[3]);
	serial_printi("echo: M201 X%f Y%f Z%f E%f\n", max_acceleration_units_per_sq_second[0], max_acceleration_units_per_sq_second[1], max_acceleration_units_per_sq_second[2], max_acceleration_units_per_sq_second[3]);
	serial_printi("echo: M204 S%f T%f\n", acceleration, retract_acceleration);
	serial_printi("echo: M205 S%f T%f B%f X%f Z%f E%f\n",minimumfeedrate, mintravelfeedrate, minsegmenttime,max_xy_jerk,max_z_jerk,max_e_jerk);

	serial_printi("echo: M206 X%d Y%d Z%d\n", state.offsets[0], state.offsets[1], state.offsets[2]);
	serial_println("echo: INFO|EEPROM VALUES|END");
	
} 



void config_RetrieveSettings(){
	int i=EEPROM_OFFSET;
	char stored_ver[4];
	char ver[4]=EEPROM_VERSION;
	EEPROM_READ_VAR(i,stored_ver); //read stored version
	//  SERIAL_ECHOLN("Version: [" << ver << "] Stored version: [" << stored_ver << "]");
	
	
	if (strncmp(ver,stored_ver,3) != 0){
		serial_printf("echo: DEBUG|CONFIG|INSTALLING DEFAULTS|%s\n", stored_ver);
		config_ResetDefault();
		return;
	}
	// version number match
	EEPROM_READ_VAR(i,axis_steps_per_unit);
	EEPROM_READ_VAR(i,max_feedrate);
	EEPROM_READ_VAR(i,max_acceleration_units_per_sq_second);

	EEPROM_READ_VAR(i,acceleration);
	EEPROM_READ_VAR(i,retract_acceleration);
	EEPROM_READ_VAR(i,minimumfeedrate);
	EEPROM_READ_VAR(i,mintravelfeedrate);
	EEPROM_READ_VAR(i,minsegmenttime);
	EEPROM_READ_VAR(i,max_xy_jerk);
	EEPROM_READ_VAR(i,max_z_jerk);
	EEPROM_READ_VAR(i,max_e_jerk);
	EEPROM_READ_VAR(i,state.offsets);
	EEPROM_READ_VAR(i,state.zoffset);

	// do not need to scale PID values as the values in EEPROM are already scaled
	EEPROM_READ_VAR(i,Kp);
	EEPROM_READ_VAR(i,Ki);
	EEPROM_READ_VAR(i,Kd);


	job.settingsChaged();
	
}


void config_ResetDefault(){
	
    float tmp1[]=DEFAULT_AXIS_STEPS_PER_UNIT;
    float tmp2[]=DEFAULT_MAX_FEEDRATE;
    long tmp3[]=DEFAULT_MAX_ACCELERATION;
	
    for (short i=0;i<4;i++){
		axis_steps_per_unit[i]=tmp1[i];     //defined in planner.cpp
        max_feedrate[i]=tmp2[i];  //defined in planner.cpp
        max_acceleration_units_per_sq_second[i]=tmp3[i]; //defined in planner.cpp
    }
	
	minimumfeedrate = DEFAULT_MINIMUMFEEDRATE; //defined in planner.cpp
	mintravelfeedrate = DEFAULT_MINTRAVELFEEDRATE; //defined in planner.cpp
	max_xy_jerk = DEFAULT_XYJERK; //defined in planner.cpp
	max_z_jerk = DEFAULT_ZJERK; //defined in planner.cpp
	max_e_jerk = DEFAULT_EJERK; //defined in planner.cpp
	minsegmenttime = DEFAULT_MINSEGMENTTIME; //defined in planner.cpp
	acceleration = DEFAULT_ACCELERATION; //defined in planner.cpp
	retract_acceleration = DEFAULT_RETRACT_ACCELERATION; //defined in planner.cpp
	
    
	
	state.zoffset = -CFG_Z_PROBE_OFFSET_FROM_EXTRUDER; //defined gc_bed_leveling.cpp


#ifdef temperature_h
    Kp = DEFAULT_Kp;
    Ki = scalePID_i(DEFAULT_Ki);
    Kd = scalePID_d(DEFAULT_Kd);
    
    // call updatePID (similar to when we have processed M301)
    temp_updatePID();
	
	Kc = DEFAULT_Kc;

#endif //temperature_h

	job.settingsChaged();
	serial_println("echo: DEBUG|CONFIG|RESET TO DEFAULTS");

}
