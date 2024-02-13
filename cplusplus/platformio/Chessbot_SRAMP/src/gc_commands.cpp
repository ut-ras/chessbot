#include "firmware.h"



extern unsigned long starttime;
extern unsigned long stoptime;



//G90: Set to Absolute Positioning
void gcode_G90(){
	state.relative_modes[X_AXIS] = false;
	state.relative_modes[Y_AXIS] = false;
	state.relative_modes[Z_AXIS] = false;

	serial_println("ok");
}
//G91: Set to Relative Positioning
void gcode_G91(){
	state.relative_modes[X_AXIS] = true;
	state.relative_modes[Y_AXIS] = true;
	state.relative_modes[Z_AXIS] = true;

	serial_println("ok");
}
//G92: Set Position
void gcode_G92(){
	char param;
	int found;
	long value=0;
	long p[] = {state.position[0], state.position[1], state.position[2], state.position[3]};

	st_synchronize();
	for(int idx=0; idx < 3; idx++) {
		param = kAXIS_ALL[idx];
		if((found = job.parser.findParam(param)) < 0) continue;

		value = lround(job.parser.parseFloat(found, 0) * axis_steps_per_unit[value]);
		state.position[idx] = value; //lround(x*axis_steps_per_unit[value]);
	}

	plan_set_position(state.position[X_AXIS], state.position[Y_AXIS], state.position[Z_AXIS], state.position[E_AXIS]);

	//st_set_position(state.position[X_AXIS], state.position[Y_AXIS], state.position[Z_AXIS], state.position[E_AXIS]);
	serial_println("ok");
}
//M82: Set extruder to absolute mode
void gcode_M82(){
	state.relative_modes[E_AXIS] = false;

	serial_println("ok");
}
//M83: Set extruder to relative mode
void gcode_M83(){
	state.relative_modes[E_AXIS] = true;

	serial_println("ok");
}


//M81: ATX Power Off
void gcode_M81(){
	temp_heaters_disable();

	st_synchronize();
	disable_e0();
	disable_e1();

	finishAndDisableSteppers();

	state.fanSpeed = 0;

	delay(1000); // Wait a little before to switch off
	st_synchronize();


#if defined(PS_ON_PIN) && PS_ON_PIN > -1
	SET_OUTPUT(PS_ON_PIN);
	WRITE(PS_ON_PIN, HIGH);
#endif

	serial_println("ok");
}


//T0: Set Extruder Temperature ///TODO Implement T0 T1 change extruder
void gcode_T0(){

	state.active_extruder = 0;

	#if EXTRUDERS > 1

	#endif

	serial_println("ok T0");
}
//T1: Set Extruder Temperature ///TODO Implement T0 T1 change extruder
void gcode_T1(){

	state.active_extruder = 0;

	#if EXTRUDERS > 1

	#endif

	serial_println("ok T1");
}

//M104: Set Extruder Temperature
void gcode_M104(){
	int8 ext = state.active_extruder;
	int8 found;

	if((found = job.parser.findParam('T')) >= 0){
		ext = job.parser.parseFloat(found, ext);
	}
	if(ext > EXTRUDERS){
		serial_printi("echo: ERR|M104|Invalid extruder|%d\n", ext);
		ext = state.active_extruder;
	}

	float temp = 195;
	if((found = job.parser.findParam('S')) >= 0){
		temp = job.parser.parseFloat(found, 195);
		setTargetHotend( temp, ext );
	}

	///TODO add support DUAL_X_CARRIAGE

	serial_printi("ok T%d:%f / %f B:%f / %f\n", ext,degHotend(ext), degTargetHotend(ext), degBed(), degTargetBed());
}
//M105: Get Extruder Temperature
void gcode_M105(){
	serial_printi("ok T%d:%f / %f B:%f / %f\n", state.active_extruder, degHotend(state.active_extruder), degTargetHotend(state.active_extruder), degBed(), degTargetBed());
}

//M109: Set Extruder Temperature and Wait
void gcode_M109(){
	int found = -1;
	float temp = 210; //celcius
	bool dir;
	bool flgWait;
	if((found = job.parser.findParam('S')) >= 0){
		temp = job.parser.parseFloat(found, temp);
		setTargetHotend(temp, state.active_extruder);

		///TODO #ifdef DUAL_X_CARRIAGE

		dir = isHeatingHotend(state.active_extruder); // 1 if heating, 0 if cooling
		if(dir){
			flgWait = true;
		}
	}else if((found = job.parser.findParam('R')) >= 0){
		temp = job.parser.parseFloat(found, temp);
		setTargetHotend(temp, state.active_extruder);

		dir = isHeatingHotend(state.active_extruder); // 1 if heating, 0 if cooling
		flgWait = true;
	}

	unsigned long t = millis();
	while (flgWait){


		//Print Temp Reading every 1 second while heating up.
		if(( millis() - t) > 1000 ){
			t = millis();

			serial_printi("T:%f E:%d B:%f\n", degHotend(state.active_extruder), state.active_extruder, degBed());
		}

		job.pollJob();

		flgWait = dir ? isHeatingHotend(state.active_extruder) : isCoolingHotend(state.active_extruder);
	}

	updateLastActivity
	serial_println("ok");
}

//M190: Wait for bed temperature to reach target temp
void gcode_M190(){
	int8 found;
	float temp = 50;
	int8 dir;
	bool flgWait;
	if((found = job.parser.findParam('S')) >= 0){
		temp = job.parser.parseFloat(found, temp);
		setTargetBed(temp);

		dir = isHeatingBed(); // 1 if heating, 0 if cooling
		if(dir){
			flgWait = true;
		}
	}else if((found = job.parser.findParam('R')) >= 0){
		temp = job.parser.parseFloat(found, temp);
		setTargetBed(temp);

		dir = isHeatingBed(); // 1 if heating, 0 if cooling
		flgWait = true;
	}
	serial_printi("BED B:%f/%f DIR=%d\n",  degBed(), degTargetBed(), dir);
	unsigned long t = millis();
	while (flgWait){


		//Print Temp Reading every 1 second while heating up.
		if(( millis() - t) > 1000 ){
			t = millis();

			serial_printi("T:%f E:%d B:%f /%f\n", degHotend(state.active_extruder), state.active_extruder, degBed(), degTargetBed());
		}

		job.pollJob();

		flgWait = dir ? isHeatingBed() : isCoolingBed();
	}

	 //previous_millis_cmd = millis();  ///TODO code for inactivity
	serial_println("ok");
}

//M106: Fan On
void gcode_M106(){
	int8 found;
	if((found = job.parser.findParam('S')) >= 0){
		state.fanSpeed = job.parser.parseFloat(found, 255);
	}else{
		state.fanSpeed = 255;
	}
	serial_printi("echo: INFO|FANSPEED|%d\nok\n", state.fanSpeed);

}
//M107: Fan Off //deprecated in favor of M106 S0
void gcode_M107(){
	state.fanSpeed = 0;
	serial_println("echo: INFO|FANSPEED|0");
	serial_println("ok");
}


//M84: Stop idle hold
void gcode_M84(){
	int8 found;
	if((found = job.parser.findParam('S')) >= 0){
		state.max_inactive_time = job.parser.parseFloat(found, 255) * 1000; //seconds to millis
	}

	st_synchronize();
	disable_e0();
	disable_e1();

	finishAndDisableSteppers();

	serial_println("ok");
}

//M114: Get Current Position
void gcode_M114(){
	serial_printi("ok X:%f Y:%f Z:%f E%f\n", state.position[X_AXIS], state.position[Y_AXIS], state.position[Z_AXIS], state.position[E_AXIS]);
}

//M115: Get Firmware Version and Capabilities
void gcode_M115(){
	serial_printf("FIRMWARE_NAME:%s\tFIRMWARE_VERSION: 1.0\tPROTOCOL_VERSION:%s\tMACHINE_TYPE:Mendel\tEXTRUDER_COUNT:%d\tUUID:%s\nok\n", CFG_FIRMWARE_NAME,CFG_PROTOCOL_VERSION ,EXTRUDERS, CFG_MACHINE_UUID);
}
//M119: Get all endstop states
void gcode_M119(){
	serial_printf("ok Reporting endstop status\n");
	#if defined(X_MIN_PIN) && X_MIN_PIN > -1
			serial_printf("x_min: %s\n", (READ(X_MIN_PIN)^X_MIN_ENDSTOP_INVERTING) ? "TRIGGERED": "OPEN" );
	#endif
	#if defined(X_MAX_PIN) && X_MAX_PIN > -1
			serial_printf("x_max: %s\n", (READ(X_MAX_PIN)^X_MAX_ENDSTOP_INVERTING) ? "TRIGGERED": "OPEN" );
	#endif

	#if defined(Y_MAX_PIN) && Y_MAX_PIN > -1
			serial_printf("y_max: %s\n", (READ(Y_MAX_PIN)^Y_MAX_ENDSTOP_INVERTING) ? "TRIGGERED": "OPEN" );
	#endif
	#if defined(Y_MIN_PIN) && Y_MIN_PIN > -1
			serial_printf("y_min: %s\n", (READ(Y_MIN_PIN)^Y_MIN_ENDSTOP_INVERTING) ? "TRIGGERED": "OPEN" );
	#endif

	#if defined(Z_MIN_PIN) && Z_MIN_PIN > -1
			serial_printf("z_min: %s\n", (READ(Z_MIN_PIN)^Z_MIN_ENDSTOP_INVERTING) ? "TRIGGERED": "OPEN" );
	#endif
	#if defined(Z_MAX_PIN) && Z_MAX_PIN > -1
			serial_printf("z_max: %s\n", (READ(Z_MAX_PIN)^Z_MAX_ENDSTOP_INVERTING) ? "TRIGGERED": "OPEN" );
	#endif
}
//M140: Set Bed Temperature (Fast)
void gcode_M140(){
	int8 found;
	if((found = job.parser.findParam('S')) >= 0){
		setTargetBed( job.parser.parseFloat(found, 50) );
	}
	serial_println("ok");
}

// M203 max feedrate mm/sec
void gcode_M203(){
	char param;
	int found;
	float value=0;

	for(int idx=0; idx < 3; idx++) {
		param = kAXIS_ALL[idx];
		if((found = job.parser.findParam(param)) < 0) continue;

		value = job.parser.parseFloat(found, 0);
		state.max_feedrate[idx] = value;
	}
	serial_println("ok");
}

// M204: Set default acceleration
void gcode_M204(){

	//acceleration from planner.cpp
	acceleration = job.parser.getNumeric('S', acceleration);
	//retract_acceleration from planner.cpp
	retract_acceleration = job.parser.getNumeric('T', retract_acceleration);
	serial_println("ok");
}
// M205: Set Advanced Settings
void gcode_M205(){
	//advanced settings:  minimum travel speed S=while printing T=travel only,  B=minimum segment time X= maximum xy jerk, Z=maximum Z jerk

	//minimumfeedrate from planner.cpp
	minimumfeedrate = job.parser.getNumeric('S', minimumfeedrate);
	//mintravelfeedrate from planner.cpp
	mintravelfeedrate = job.parser.getNumeric('T', mintravelfeedrate);
	//minsegmenttime from planner.cpp
	minsegmenttime = job.parser.getNumeric('B', minsegmenttime);
	//max_xy_jerk from planner.cpp
	max_xy_jerk = job.parser.getNumeric('X', max_xy_jerk);
	//max_z_jerk from planner.cpp
	max_z_jerk = job.parser.getNumeric('Z', max_z_jerk);
	//max_e_jerk from planner.cpp
	max_e_jerk = job.parser.getNumeric('E', max_e_jerk);
	serial_println("ok");
}


// M206 additional homeing offset
void gcode_M206(){
	char param;
	int found;
	float value=0;

	for(int idx=0; idx < 3; idx++) {
		param = kAXIS_ALL[idx];
		if((found = job.parser.findParam(param)) < 0) continue;

		value = job.parser.parseFloat(found, 0);
		state.offsets[idx] = value;
	}
	serial_println("ok");
}

// M210 - Set homing feedrates
//Set the feedrates used for homing to the values specified in mm per minute
void gcode_M210(){
	/*float feedValues[3];

	 if( job.parser.getNParams("XYZ", feedValues) ){
	 if(feedValues[0]!=NULL) cfg_homing_feedrate[X_AXIS] = feedValues[0];
	 if(feedValues[1]!=NULL) cfg_homing_feedrate[Y_AXIS] = feedValues[0];
	 if(feedValues[2]!=NULL) cfg_homing_feedrate[Z_AXIS] = feedValues[0];
	 }
	 SERIAL_ECHOLN("ok");
	 */
	serial_println("ok");
}

//M220: Set speed factor override percentage
void gcode_M220(){
	//S<factor in percent>- Extrude factor override percentage (0..100 or higher), default 100%
	//percent as 0...100 or higher

	state.extrudemultiply = job.parser.getNumeric('S', state.extrudemultiply);
	serial_println("ok");
}

//M221: Set extrude factor override percentage
void gcode_M221(){
	//S<factor in percent>- set speed factor override percentage,
	//percent as 0...100 or higher

	state.feedmultiply = job.parser.getNumeric('S', state.feedmultiply);
	serial_println("ok");
}


///TODO Implement M218
///M218: Set Hotend Offset, RepRap uses G10 instead
///Sets hotend offset (in mm): T<extruder_number> X<offset_on_X> Y<offset_on_Y>.
/// M218 T1 X50 Y0.5

///TODO Implement 226
///M226 P<pin number> S<pin state>- Wait until the specified pin reaches the state required

///TODO Implement M240  Triggers a camera by emulating a Canon RC-1 : http://www.doc-diy.net/photo/rc-1_hacked/


//M301: Set PID parameters
void gcode_M301(){
	int8 found = 0;

	if((found = job.parser.findParam('P')) >= 0){
		Kp = job.parser.parseFloat(found, Kp);
	}

	if((found = job.parser.findParam('I')) >= 0){
		Ki = scalePID_i(job.parser.parseFloat(found, 0));
	}

	if((found = job.parser.findParam('D')) >= 0){
		Kd = scalePID_d(job.parser.parseFloat(found, 0));
	}

	if((found = job.parser.findParam('C')) >= 0){
		Kc = job.parser.parseFloat(found, Kc);
	}


	temp_updatePID();
	serial_println("ok");
}

//M303: Run PID tuning
void gcode_M303(){
	//This command generates Proportional (Kp), Integral (Ki), and Derivative (Kd) values for the hotend or bed (E-1).
	//Send the appropriate code and wait for the output to update the firmware.
	int8 found = 0;
	float temp = 150.0;
	int e=0;
	int c=5;
	if((found = job.parser.findParam('E')) >= 0){
		e = job.parser.parseFloat(found, e);
	}

	if (e<0) temp=70;

	if((found = job.parser.findParam('S')) >= 0){
		temp = job.parser.parseFloat(found, temp);
	}
	if((found = job.parser.findParam('C')) >= 0){
		c = job.parser.parseFloat(found, c);
	}

	///TODO implement PID_autotune
	//PID_autotune(temp, e, c);
	serial_println("ok");
}

//M400: Wait for current moves to finish
void gcode_M400(){
	st_synchronize();
	serial_println("ok");
}


// M500 Store settings in EEPROM
void gcode_M500(){
	config_StoreSettings();
	serial_println("ok");
}

// M501 Read settings from EEPROM
void gcode_M501(){
	config_RetrieveSettings();
	serial_println("ok");
}

// M502 Revert to default settings
void gcode_M502(){
	config_ResetDefault();
	serial_println("ok");
}
// M503 print settings currently in memory
void gcode_M503(){
	config_PrintSettings();
	serial_println("ok");
}

// M565 Set Z probe offset
void gcode_M565(){ //new marlin doesnt have this one
	int8 found = 0;
	if((found = job.parser.findParam('Z')) >= 0){
		state.zoffset = job.parser.parseFloat(found, state.zoffset);
	}else{
		serial_printi("echo: INFO|Z-OFFSET|%f\n", state.zoffset);
	}
	serial_println("ok");
}

//Move to Origin (Home)
void gcode_G28(){
	//serial_println("echo: exec G28");

	int8 found;
	float feedrate=0;
	float v=0;
	bool flgHomeAll = false;
	bool flgHomeX = false;
	bool flgHomeY = false;
	bool flgHomeZ = false;

	flgHomeX = (job.parser.findParam('X') >= 0);
	flgHomeY = (job.parser.findParam('Y') >= 0);
	flgHomeZ = (job.parser.findParam('Z') >= 0);

	flgHomeAll = !(flgHomeX || flgHomeY || flgHomeZ);


	//Reset the plane ("erase" all leveling data)
	plan_bed_level_matrix.set_to_identity();

	updateLastActivity

	job.setFeedRateWithMultiplier(0.0, 100);

	for(int i=0; i < CFG_AXIS_COUNT; i++) {
		state.destination[i] = state.position[i];
	}

	enable_endstops(true);

	if( flgHomeAll || flgHomeX ){
		mc_homeaxis(X_AXIS);
	}

	if( flgHomeAll || flgHomeY ){
		mc_homeaxis(Y_AXIS);
	}

	if(flgHomeX){
		v = job.parser.getNumeric('X', 0);
		if(v != 0) {
			state.position[X_AXIS] = v + state.offsets[X_AXIS];
		}
	}
	if(flgHomeY){
		v = job.parser.getNumeric('Y', 0);
		if(v != 0) {
			state.position[Y_AXIS] = v + state.offsets[Y_AXIS];
		}
	}


	if(flgHomeAll || flgHomeZ ){
		#if defined(CFG_Z_HOME_RAISE) && (CFG_Z_HOME_RAISE > 0)
			state.destination[Z_AXIS] = CFG_Z_HOME_RAISE * CFG_Z_HOME_DIR * (-1);
			feedrate = state.max_feedrate[Z_AXIS];
			plan_buffer_line(state.destination[X_AXIS], state.destination[Y_AXIS], state.destination[Z_AXIS], state.destination[E_AXIS], feedrate, state.active_extruder);
			st_synchronize();
		#endif

		mc_homeaxis(Z_AXIS);
	}


	if(flgHomeZ){
		v = job.parser.getNumeric('Z', 0);
		if(v != 0) {
			state.position[Z_AXIS] = v + state.offsets[Z_AXIS];
		}
	}

	if(flgHomeAll || flgHomeZ) {
		if (state.zoffset < 0){
			state.position[Z_AXIS] += state.zoffset; // in the lsq we reach here after raising the extruder due to the loop structure
		}else{
			state.position[Z_AXIS] += -state.zoffset; // in the lsq we reach here after raising the extruder due to the loop structure
		}
	}


	plan_set_position(state.position[X_AXIS], state.position[Y_AXIS], state.position[Z_AXIS], state.position[E_AXIS]);



	enable_endstops(false);
	job.restoreFeedRate();


	endstops_hit_on_purpose();  //reset endstop trigger stepper.cpp

	updateLastActivity

	//serial_printi("echo: DEBUG|HOME COMPLETED|X=%f|Y=%f|Z=%f\n", state.position[X_AXIS], state.position[Y_AXIS], state.position[Z_AXIS]);
	serial_println("ok");
}

//G29: Detailed Z-Probe, probes the bed at 3 or more points.
void gcode_G29(){

	int8 found;
	float feedrate=0;

	// Prevent user from running a G29 without first homing in X and Y
	if (! (state.located[X_AXIS] && state.located[Y_AXIS]) ){
		serial_println("echo: ERR|G29|Issue G28 first to home axis");
		serial_println("ok");
		return;
	}

	st_synchronize();

	// make sure the bed_level_rotation_matrix is identity or the planner will get it incorectly
	//vector_3 corrected_position = plan_get_position_mm();
	//corrected_position.debug("position before G29");
	plan_bed_level_matrix.set_to_identity();
	vector_3 uncorrected_position = plan_get_position();
	//uncorrected_position.debug("position durring G29");
	state.position[X_AXIS] = uncorrected_position.x;
	state.position[Y_AXIS] = uncorrected_position.y;
	state.position[Z_AXIS] = uncorrected_position.z;
	plan_set_position(state.position[X_AXIS], state.position[Y_AXIS], state.position[Z_AXIS], state.position[E_AXIS]);

	job.setFeedRateWithMultiplier(state.feedrate, 100);
	enable_endstops(true);

	feedrate = cfg_homing_feedrate[Z_AXIS];


	// probe at the points of a lattice grid
	int xGridSpacing = (RIGHT_PROBE_BED_POSITION - LEFT_PROBE_BED_POSITION) / (AUTO_BED_LEVELING_GRID_POINTS-1);
	int yGridSpacing = (BACK_PROBE_BED_POSITION - FRONT_PROBE_BED_POSITION) / (AUTO_BED_LEVELING_GRID_POINTS-1);

	// solve the plane equation ax + by + d = z
	// A is the matrix with rows [x y 1] for all the probed points
	// B is the vector of the Z positions
	// the normal vector to the plane is formed by the coefficients of the plane equation in the standard form, which is Vx*x+Vy*y+Vz*z+d = 0
	// so Vx = -a Vy = -b Vz = 1 (we want the vector facing towards positive Z

	// "A" matrix of the linear system of equations
	double eqnAMatrix[AUTO_BED_LEVELING_GRID_POINTS*AUTO_BED_LEVELING_GRID_POINTS*3];
	// "B" vector of Z points
	double eqnBVector[AUTO_BED_LEVELING_GRID_POINTS*AUTO_BED_LEVELING_GRID_POINTS];

	int probePointCounter = 0;
	bool zig = true;

	for (int yProbe=FRONT_PROBE_BED_POSITION; yProbe <= BACK_PROBE_BED_POSITION; yProbe += yGridSpacing){
		int xProbe, xInc;
		if (zig){
			xProbe = LEFT_PROBE_BED_POSITION;
			//xEnd = RIGHT_PROBE_BED_POSITION;
			xInc = xGridSpacing;
			zig = false;
		} else { // zag
			xProbe = RIGHT_PROBE_BED_POSITION;
			//xEnd = LEFT_PROBE_BED_POSITION;
			xInc = -xGridSpacing;
			zig = true;
		}

		for (int xCount=0; xCount < AUTO_BED_LEVELING_GRID_POINTS; xCount++){
			float z_before;
			if (probePointCounter == 0){
				// raise before probing
				z_before = CFG_Z_RAISE_BEFORE_PROBING;
			} else {
				// raise extruder
				z_before = state.position[Z_AXIS] + CFG_Z_RAISE_BETWEEN_PROBINGS;
			}

			float measured_z = mc_probe_pt(xProbe, yProbe, z_before);

			eqnBVector[probePointCounter] = measured_z;

			eqnAMatrix[probePointCounter + 0*AUTO_BED_LEVELING_GRID_POINTS*AUTO_BED_LEVELING_GRID_POINTS] = xProbe;
			eqnAMatrix[probePointCounter + 1*AUTO_BED_LEVELING_GRID_POINTS*AUTO_BED_LEVELING_GRID_POINTS] = yProbe;
			eqnAMatrix[probePointCounter + 2*AUTO_BED_LEVELING_GRID_POINTS*AUTO_BED_LEVELING_GRID_POINTS] = 1;
			probePointCounter++;
			xProbe += xInc;
		}// end for xCount
	} //end for yGridSpacing

	enable_endstops(false);
	job.restoreFeedRate();


	// solve lsq problem
	double *plane_equation_coefficients = qr_solve(AUTO_BED_LEVELING_GRID_POINTS*AUTO_BED_LEVELING_GRID_POINTS, 3, eqnAMatrix, eqnBVector);

	vector_3 planeNormal = vector_3(-plane_equation_coefficients[0], -plane_equation_coefficients[1], 1);
	planeNormal.debug("planeNormal");
	plan_bed_level_matrix = matrix_3x3::create_look_at(planeNormal);

	vector_3 corrected_position = plan_get_position();
	//    corrected_position.debug("position after");
	state.position[X_AXIS] = corrected_position.x;
	state.position[Y_AXIS] = corrected_position.y;
	state.position[Z_AXIS] = corrected_position.z;

	// put the bed at 0 so we don't go below it.
	if (state.zoffset < 0){
		state.position[Z_AXIS] = state.zoffset; // in the lsq we reach here after raising the extruder due to the loop structure
	}else{
		state.position[Z_AXIS] = -state.zoffset; // in the lsq we reach here after raising the extruder due to the loop structure
	}


	plan_set_position(state.position[X_AXIS], state.position[Y_AXIS], state.position[Z_AXIS], state.position[E_AXIS]);

	free(plane_equation_coefficients);

	// The following code correct the Z height difference from z-probe position and hotend tip position.
	// The Z height on homing is measured by Z-Probe, but the probe is quite far from the hotend.
	// When the bed is uneven, this height must be corrected.
	float real_z = float(st_get_position(Z_AXIS))/axis_steps_per_unit[Z_AXIS];  //get the real Z (since the auto bed leveling is already correcting the plane)
	float z_tmp = state.position[Z_AXIS];
	float x_tmp = state.position[X_AXIS] + CFG_X_PROBE_OFFSET_FROM_EXTRUDER;
	float y_tmp = state.position[Y_AXIS] + CFG_Y_PROBE_OFFSET_FROM_EXTRUDER;

	apply_rotation_xyz(plan_bed_level_matrix, x_tmp, y_tmp, z_tmp);	//Apply the correction sending the probe offset
	state.position[Z_AXIS] = z_tmp - real_z + state.position[Z_AXIS]; //The difference is added to current position and sent to planner.
	plan_set_position(state.position[X_AXIS], state.position[Y_AXIS], state.position[Z_AXIS], state.position[E_AXIS]);

	serial_printi("echo: INFO|POSITION|X:%f|Y:%f|Z:%f|E:%f\n", state.position[X_AXIS], state.position[Y_AXIS], state.position[Z_AXIS], state.position[E_AXIS]);

	serial_printi("echo: INFO|POSITION-STEPS|X:%f|Y:%f|Z:%f|\n",
		float(st_get_position(X_AXIS))/axis_steps_per_unit[X_AXIS],
		float(st_get_position(Y_AXIS))/axis_steps_per_unit[Y_AXIS],
		float(st_get_position(Z_AXIS))/axis_steps_per_unit[Z_AXIS]
	);

	updateLastActivity
	serial_println("ok");
}
//G30: Single Z Probe
void gcode_G30(){
	st_synchronize();

	job.setFeedRateWithMultiplier(cfg_homing_feedrate[Z_AXIS], 100);
	enable_endstops(true);


	mc_run_z_probe();

	enable_endstops(false);
	job.restoreFeedRate();

	updateLastActivity

	serial_println("ok");
}


//G0-rapid move
//G1-linear mover
void gcode_G0G1(){
	float feedValues[3];
	int found = 0;
	float value = 0;
	char param;


	for(int idx=0; idx < kAXIS_COUNT; idx++) {
		param = kAXIS_ALL[idx];
		if((found = job.parser.findParam(param)) < 0) continue;

		value = job.parser.parseFloat(found, 0);
		state.destination[idx] = value;

		if(state.relative_modes[idx]) state.destination[idx] += state.position[idx];
	}

	value = job.parser.getNumeric('F', 0);
	if(value > 0) state.feedrate = value;
	mc.pushMove();
	serial_println("ok");
}

//G2: Controlled Arc Move (Clockwise Arc)
void gcode_G2(){
	float feedValues[3];
	int found = 0;
	float value = 0;
	char param;


	for(int idx=0; idx < kAXIS_COUNT; idx++) {
		param = kAXIS_ALL[idx];
		if((found = job.parser.findParam(param)) < 0) continue;

		value = job.parser.parseFloat(found, 0);
		state.destination[idx] = value;

		if(state.relative_modes[idx]) state.destination[idx] += state.position[idx];
	}

	value = job.parser.getNumeric('I', 0);
	if(value > 0) state.destOffset[0] = value;

	value = job.parser.getNumeric('J', 0);
	if(value > 0) state.destOffset[1] = value;

	mc.pushArcMove(true);
	serial_println("ok");
}

//G3: Controlled Arc Move (Counter-Clockwise Arc)
void gcode_G3(){
	float feedValues[3];
	int found = 0;
	float value = 0;
	char param;


	for(int idx=0; idx < kAXIS_COUNT; idx++) {
		param = kAXIS_ALL[idx];
		if((found = job.parser.findParam(param)) < 0) continue;

		value = job.parser.parseFloat(found, 0);
		state.destination[idx] = value;

		if(state.relative_modes[idx]) state.destination[idx] += state.position[idx];
	}

	value = job.parser.getNumeric('I', 0);
	if(value > 0) state.destOffset[0] = value;

	value = job.parser.getNumeric('J', 0);
	if(value > 0) state.destOffset[1] = value;

	mc.pushArcMove(false);
	serial_println("ok");
}


void gcode_initialize_methods(){


	job.registerGCODE('G',0, &gcode_G0G1);
	job.registerGCODE('G',1, &gcode_G0G1);
	job.registerGCODE('G',2, &gcode_G2); //G2: Controlled Arc Move (Clockwise Arc)
	job.registerGCODE('G',3, &gcode_G3); //G3: Controlled Arc Move (Counter-Clockwise Arc)

	job.registerGCODE('G',90, &gcode_G90);
	job.registerGCODE('G',92, &gcode_G92); //set position

	job.registerGCODE('M',21, &gcode_M21); // M21 - init SD card
	job.registerGCODE('M',20, &gcode_M20); //M20 - list SD card
	job.registerGCODE('M',23, &gcode_M23); //M23: Select SD file
	job.registerGCODE('M',24, &gcode_M24); //M24 - Start SD print

	job.registerGCODE('T',0, &gcode_T0); //set active extruder to ext 0
	job.registerGCODE('T',1, &gcode_T1); //set active extruder to ext 1 //TODO


	job.registerGCODE('M',81, &gcode_M81); //M81: ATX Power Off

	//absolute relative pos
	job.registerGCODE('M',82, &gcode_M82);
	job.registerGCODE('M',83, &gcode_M83);
	job.registerGCODE('G',90, &gcode_G90);
	job.registerGCODE('G',91, &gcode_G91);


	job.registerGCODE('M',84, &gcode_M84); //M84: Stop idle hold

	job.registerGCODE('M',114, &gcode_M114); //M114: Get Current Position
	job.registerGCODE('M',115, &gcode_M115); //M115: Get Firmware Version and Capabilities

	//Temperature
	job.registerGCODE('M',105, &gcode_M105); //M105: Get Extruder Temperature
	job.registerGCODE('M',104, &gcode_M104); //M104: Set Extruder Temperature
	job.registerGCODE('M',109, &gcode_M109); //M109: Set Extruder Temperature and Wait
	job.registerGCODE('M',140, &gcode_M140); //M140: Set Bed Temperature (Fast)
	job.registerGCODE('M',190, &gcode_M190); //M190: Wait for bed temperature to reach target temp

	//Fan
	job.registerGCODE('M',106, &gcode_M106); //M106: Fan On
	job.registerGCODE('M',107, &gcode_M107); //M107: Fan Off //deprecated in favor of M106 S0

	//Homeing & Auto Zero Bed
	job.registerGCODE('G',28, &gcode_G28); //G28: Move to Origin (Home), Home all axes
	job.registerGCODE('G',29, &gcode_G29); //G29: Detailed Z-Probe, probes the bed at 3 or more points.

	//motion settings
	job.registerGCODE('M',203, &gcode_M203); //M203: Set maximum feedrate
	job.registerGCODE('M',204, &gcode_M204); //M204: Set default acceleration
	job.registerGCODE('M',205, &gcode_M205); //M205: Set Advanced Settings
	job.registerGCODE('M',206, &gcode_M206); //M206: additional homeing offset
	job.registerGCODE('M',220, &gcode_M220); //M220: Set speed factor override percentage
	job.registerGCODE('M',221, &gcode_M221); //M221: Set extrude factor override percentage
	job.registerGCODE('M',565, &gcode_M565); //M565: Set Z probe offset

	job.registerGCODE('M',301, &gcode_M301); //M301: Set PID parameters

	job.registerGCODE('M',400, &gcode_M400); //M400: Wait for current moves to finish
	job.registerGCODE('M',500, &gcode_M500); // M500 Store settings in EEPROM
	job.registerGCODE('M',501, &gcode_M501); // M501 Read settings from EEPROM
	job.registerGCODE('M',502, &gcode_M502); // M502 Revert to default settings
	job.registerGCODE('M',503, &gcode_M503); // M503 print settings currently in memory

	job.registerGCODE('M',119, &gcode_M119); //M119: Get all endstop states

}
