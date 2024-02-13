#include "firmware.h"

char kAXIS_ALL[kAXIS_COUNT] = {'X','Y', 'Z','E'};
float cfg_homing_feedrate[] = CFG_HOMING_FEEDRATE;

mc_state state;
gcPrintJob job;
gcMotion mc;

extern "C"{
	extern unsigned int __bss_end;
	extern unsigned int __heap_start;
	extern void *__brkval;
	
	int freeMemory() {
		int free_memory;
		
		if((int)__brkval == 0)
			free_memory = ((int)&free_memory) - ((int)&__bss_end);
		else
			free_memory = ((int)&free_memory) - ((int)__brkval);
		
		return free_memory;
	}
}




void setup() {
	// put your setup code here, to run once:
	
	//setup_killpin();
	//setup_powerhold();
	
	
	serial_init();
	serial_println(CFG_FIRMWARE_NAME);
	
	
	/*
	serial_println("echo: INFO|SIG|Simple RAMPS Firmware V1.0");
	serial_printf("echo: INFO|PROTOCOL|%s\n", CFG_PROTOCOL_VERSION);
	serial_printf("echo: INFO|COMPILE_DATE|%s\n", __DATE__);
	serial_printf("echo: INFO|MEMORY|%d\n", freeMemory() );
	serial_printf("echo: INFO|MACHINE|%s\n", CFG_MACHINE_NAME);
	// Check reason of reset

	byte mcu = MCUSR;
	if(mcu & 1) serial_println("echo: DEBUG|STARTUP|Power On");
	if(mcu & 2) serial_println("echo: DEBUG|STARTUP|External reset");
	if(mcu & 4) serial_println("echo: DEBUG|STARTUP|Brown out reset");
	if(mcu & 8) serial_println("echo: DEBUG|STARTUP|Watchdog reset");
	if(mcu & 32) serial_println("echo: DEBUG|STARTUP|Software reset");
	*/
	
	MCUSR=0;
	
	
	
	// loads data from EEPROM if available else uses defaults (and resets step acceleration rate)
	config_RetrieveSettings();
	//config_PrintSettings();
	//config_StoreSettings();
	
	temp_init(); //temperature.cpp
	plan_init();  //planner.cpp;
	st_init(); //stepper.cpp
	

	
	serial_println("ok");
	state.status = kSTATUS_READY;
	while (Serial.available() > 0) { //clear input buffer
		Serial.read();
	}
	Serial.flush();
	
	
}



void loop() {
	job.run();
}
