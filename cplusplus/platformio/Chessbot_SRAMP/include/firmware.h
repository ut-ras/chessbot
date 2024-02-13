//
//  firmware.h
//  gc_parser
//
//  Created by JOSE L CUEVAS on 5/11/19.
//  Copyright © 2019 JOSE L CUEVAS. All rights reserved.
//

#ifndef firmware_h
#define firmware_h

#define ENABLE_AUTO_BED_LEVELING  //delete me

#define  FORCE_INLINE __attribute__((always_inline)) inline

#ifndef CRITICAL_SECTION_START
	#define CRITICAL_SECTION_START  unsigned char _sreg = SREG; cli();
	#define CRITICAL_SECTION_END    SREG = _sreg;
#endif //CRITICAL_SECTION_START

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SPI.h>
#include <SD.h>
#include "Arduino.h"
#include "fastio.h"
#include "config.h"

#include "helpers.h"



#include "cfg_store.h"
#include "serial.h"
#include "types.h"
#include "qr_solve.h"

#include "gc_sdcard.h"
#include "gc_parser.h"
#include "gc_printjob.h"
#include "gc_motion.h"
#include "gc_bed_leveling.h"

#include "temperature.h"

#include "planner.h"
#include "stepper.h"

#include "gc_commands.h"

extern char kAXIS_ALL[kAXIS_COUNT];
extern float cfg_homing_feedrate[kAXIS_COUNT];
extern mc_state state;
extern gcPrintJob job;
extern gcMotion mc;

#endif /* firmware_h */
