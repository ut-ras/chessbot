//
//  gc_bed_leveleing.cpp
//  marlin
//
//  Created by JOSE L CUEVAS on 9/1/19.
//  Copyright © 2019 JOSE L CUEVAS. All rights reserved.
//
#include "firmware.h"


void mc_homeaxis(int axis) {

	int axis_home_dir = -1;
	int axis_pin = -1;
	float feedrate = 0;
	float zfeedrate = 0;
	float max_length = 0;
	float retract_mm = 0;
	
	feedrate = cfg_homing_feedrate[axis];
	if (axis == X_AXIS){
		max_length = CFG_X_MAX_LENGTH;
		retract_mm = CFG_X_HOME_RETRACT_MM;
		//axis_home_dir = CFG_X_HOME_DIR(state.active_extruder); //DUAL_X_CARRIAGE
	}else if (axis == Y_AXIS){
		max_length = CFG_Y_MAX_LENGTH;
		retract_mm = CFG_Y_HOME_RETRACT_MM;
		
	}else if (axis == Z_AXIS){
		max_length = CFG_Z_MAX_LENGTH;
		retract_mm = CFG_Z_HOME_RETRACT_MM;
	}else{
		return;
	}
	
	zfeedrate = feedrate;
	
	
	state.position[axis] = 0;
	plan_set_position(state.position[X_AXIS], state.position[Y_AXIS], state.position[Z_AXIS], state.position[E_AXIS]);
	
	
	state.destination[axis] = (1.5 * max_length) * axis_home_dir; //half-way
	
	
	plan_buffer_line(state.destination[X_AXIS], state.destination[Y_AXIS], state.destination[Z_AXIS], state.destination[E_AXIS], feedrate/60, state.active_extruder);
	st_synchronize(); //wait for all moves
		
	state.position[axis] = 0;
	
	plan_set_position(state.position[X_AXIS], state.position[Y_AXIS], state.position[Z_AXIS], state.position[E_AXIS]);
	
	state.destination[axis] = (-1 * retract_mm) * axis_home_dir;
	
	plan_buffer_line(state.destination[X_AXIS], state.destination[Y_AXIS], state.destination[Z_AXIS], state.destination[E_AXIS], feedrate/60, state.active_extruder);
	st_synchronize();
		
	state.destination[axis] = (2*retract_mm) * axis_home_dir;
	
	zfeedrate = feedrate/2;

	plan_buffer_line(state.destination[X_AXIS], state.destination[Y_AXIS], state.destination[Z_AXIS], state.destination[E_AXIS], zfeedrate/60, state.active_extruder);
	st_synchronize();

	mc.setHomePositionForAxis(job, axis);
	
	state.destination[axis] = state.position[axis];
	state.feedrate = 0.0;
	
	endstops_hit_on_purpose(); //reset endstop trigger stepper.cpp
	
	state.located[axis] = true;

}
static void do_blocking_move_to(float x, float y, float z) {
	job.setFeedRate(CFG_XY_TRAVEL_SPEED);
	
	state.position[X_AXIS] = x;
	state.position[Y_AXIS] = y;
	state.position[Z_AXIS] = z;
	
	plan_buffer_line(state.position[X_AXIS], state.position[Y_AXIS], state.position[Z_AXIS], state.position[E_AXIS], state.feedrate/60, state.active_extruder);
	st_synchronize();
	
	job.restoreFeedRate();
}
static void do_blocking_move_relative(float offset_x, float offset_y, float offset_z) {
	do_blocking_move_to(state.position[X_AXIS] + offset_x, state.position[Y_AXIS] + offset_y, state.position[Z_AXIS] + offset_z);
}

/// Probe bed height at position (x,y), returns the measured z value
float mc_probe_pt(float x, float y, float z_before) {
	// move to right place
	do_blocking_move_to(state.position[X_AXIS], state.position[Y_AXIS], z_before);
	do_blocking_move_to(x - CFG_X_PROBE_OFFSET_FROM_EXTRUDER, y - CFG_Y_PROBE_OFFSET_FROM_EXTRUDER, state.position[Z_AXIS]);
	
	
	mc_run_z_probe();
	float measured_z = state.position[Z_AXIS];
	
	
	serial_printi("echo: INFO|BEDPROBE|X:%f|Y:%F|Z:%f\n",x,y, measured_z);
	return measured_z;
}

void mc_run_z_probe(){
	plan_bed_level_matrix.set_to_identity();
	float feedrate = cfg_homing_feedrate[Z_AXIS];
	
	// move down until you find the bed
	float zPosition = -10;
	plan_buffer_line(state.position[X_AXIS], state.position[Y_AXIS], zPosition, state.position[E_AXIS], feedrate/60, state.active_extruder);
	st_synchronize();
	
	// we have to let the planner know where we are right now as it is not where we said to go.
	zPosition = st_get_position_mm(Z_AXIS);
	plan_set_position(state.position[X_AXIS], state.position[Y_AXIS], zPosition, state.position[E_AXIS]);
	
	// move up the retract distance
	zPosition += CFG_Z_HOME_RETRACT_MM;
	plan_buffer_line(state.position[X_AXIS], state.position[Y_AXIS], zPosition, state.position[E_AXIS], feedrate/60, state.active_extruder);
	st_synchronize();
	
	// move back down slowly to find bed
	feedrate = cfg_homing_feedrate[Z_AXIS]/4;
	zPosition -= CFG_Z_HOME_RETRACT_MM * 2;
	plan_buffer_line(state.position[X_AXIS], state.position[Y_AXIS], zPosition, state.position[E_AXIS], feedrate/60, state.active_extruder);
	st_synchronize();
	
	state.position[Z_AXIS] = st_get_position_mm(Z_AXIS);
	// make sure the planner knows where we are as it may be a bit different than we last said to move to
	plan_set_position(state.position[X_AXIS], state.position[Y_AXIS], state.position[Z_AXIS], state.position[E_AXIS]);
}
