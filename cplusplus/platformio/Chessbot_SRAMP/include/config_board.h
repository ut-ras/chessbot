//
//  board.hpp
//  marlin
//
//  Created by JOSE L CUEVAS on 9/1/19.
//  Copyright © 2019 JOSE L CUEVAS. All rights reserved.
//

#ifndef board_hpp
#define board_hpp

#define X_MS1_PIN -1
#define X_MS2_PIN -1
#define Y_MS1_PIN -1
#define Y_MS2_PIN -1
#define Z_MS1_PIN -1
#define Z_MS2_PIN -1
#define E0_MS1_PIN -1
#define E0_MS2_PIN -1
#define E1_MS1_PIN -1
#define E1_MS2_PIN -1


/* Include the config for our printer */
#include "config_r1plus_ramps_V13.h"

/* ==== Auto Config Options ==== */

//Macros to enable or disable motors
#if defined(X_ENABLE_PIN) && X_ENABLE_PIN > -1
	#define  enable_x() WRITE(X_ENABLE_PIN, CFG_X_ENABLE_ON)
	#define disable_x() { WRITE(X_ENABLE_PIN,!CFG_X_ENABLE_ON); state.located[X_AXIS] = false; }
#else
	#define enable_x() ;
	#define disable_x() ;
#endif

#if defined(Y_ENABLE_PIN) && Y_ENABLE_PIN > -1
	#define  enable_y() WRITE(Y_ENABLE_PIN, CFG_Y_ENABLE_ON)
	#define disable_y() { WRITE(Y_ENABLE_PIN,!CFG_Y_ENABLE_ON); state.located[Y_AXIS] = false; }
#else
	#define enable_y() ;
	#define disable_y() ;
#endif

#if defined(Z_ENABLE_PIN) && Z_ENABLE_PIN > -1
	#define  enable_z() WRITE(Z_ENABLE_PIN, CFG_Z_ENABLE_ON)
	#define disable_z() { WRITE(Z_ENABLE_PIN,!CFG_Z_ENABLE_ON); state.located[Z_AXIS] = false; }
#else
	#define enable_z() ;
	#define disable_z() ;
#endif

#if defined(E0_ENABLE_PIN) && (E0_ENABLE_PIN > -1)
	#define enable_e0() WRITE(E0_ENABLE_PIN, CFG_E_ENABLE_ON)
	#define disable_e0() WRITE(E0_ENABLE_PIN,!CFG_E_ENABLE_ON)
#else
	#define enable_e0()  /* nothing */
	#define disable_e0() /* nothing */
#endif

#if (EXTRUDERS > 1) && defined(E1_ENABLE_PIN) && (E1_ENABLE_PIN > -1)
	#define enable_e1() WRITE(E1_ENABLE_PIN, CFG_E_ENABLE_ON)
	#define disable_e1() WRITE(E1_ENABLE_PIN,!CFG_E_ENABLE_ON)
#else
	#define enable_e1()  /* nothing */
	#define disable_e1() /* nothing */
#endif

#endif /* board_hpp */
