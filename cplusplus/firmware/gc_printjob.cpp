
#include "firmware.h"


gc_method* gc_methods = NULL;
unsigned long lastActivityTime = 0;

gcPrintJob::gcPrintJob() {
	
	state.status = kSTATUS_NOTREADY;
	
	state.min_pos[0] = CFG_X_MIN_POS;
	state.min_pos[1] = CFG_Y_MIN_POS;
	state.min_pos[2] = CFG_Z_MIN_POS;
	
	state.max_pos[0] = CFG_X_MAX_POS;
	state.max_pos[1] = CFG_Y_MAX_POS;
	state.max_pos[2] = CFG_Z_MAX_POS;
	
	state.destOffset[0] = 0;
	state.destOffset[1] = 0;
	state.destOffset[3] = 0;
	
	state.max_feedrate[0] = CFG_X_MAX_FEEDRATE;
	state.max_feedrate[1] = CFG_Y_MAX_FEEDRATE;
	state.max_feedrate[2] = CFG_Z_MAX_FEEDRATE;
	state.max_feedrate[3] = CFG_E_MAX_FEEDRATE;
	
	state.offsets[0] = 0;
	state.offsets[1] = 0;
	state.offsets[2] = 0;
	state.offsets[3] = 0;
	
	state.zoffset = 0;
	
	state.feedmultiply = 100;
	state.extrudemultiply = 100; //100=1X 200=2X, etc
	state.volumetric_multiplier[0] = 1.0;
	state.volumetric_multiplier[1] = 1.0;
	
	state.fanSpeed = 0;
	state.active_extruder = EXT1;
	
	
	
	state.max_inactive_time = CFG_MAX_IDLE_TIME * 1000; //second to milli
	
	for(int8 i=0; i< CFG_BUFSIZE; i++){
		memset(sbuff[3],0,CFG_MAX_CMD_SIZE);
		gc_commands[i].complete = true;
	}
	
	gc_methods = calloc(0, sizeof(struct gc_method) * gc_methods_count);
	
	gcode_initialize_methods();
	
}

void gcPrintJob::registerGCODE(char group, int code, fn_callback fn) {
	int8 i = (++gc_methods_count)-1;
	gc_methods = realloc(gc_methods, sizeof(struct gc_method) * gc_methods_count);
	

	gc_methods[i].group = group;
	gc_methods[i].code = code;
	gc_methods[i].fn = fn;
	
	
}

void gcPrintJob::triggerError(int code, char *msg) {
	
	serial_printf("echo: ERROR|%d|%s", code, msg);
	
}
void gcPrintJob::setFeedRate(float feed) {
	state.feedrate_saved = state.feedrate;
	state.feedmultiply_saved = state.feedmultiply;
	state.feedrate = feed;
}
void gcPrintJob::setFeedRateWithMultiplier(float feed, float mul) {
	state.feedrate_saved = state.feedrate;
	state.feedmultiply_saved = state.feedmultiply;
	
	state.feedmultiply = mul;
	state.feedrate = feed;
}
void gcPrintJob::restoreFeedRate() {
	state.feedrate = state.feedrate_saved;
	state.feedmultiply = state.feedmultiply_saved;
}
bool gcPrintJob::isStopped() {
	return (state.status == kStatusStopped);
}

void gcPrintJob::kill() {
	
	serial_println("echo: ERR|STOPPED\n");
	
	cli(); // Stop interrupts
	temp_heaters_disable();
	
	disable_x();
	disable_y();
	disable_z();
	disable_e0();
	disable_e1();
	
#if defined(PS_ON_PIN) && PS_ON_PIN > -1
	//pinMode(PS_ON_PIN,INPUT);
#endif
	
	state.status = kSTATUS_STOPPED;
	while(1) { /* Intentionally left empty */ } // Wait for reset
}
void gcPrintJob::stop() {
	cli();
	
	temp_heaters_disable();
	if(state.status != kSTATUS_STOPPED){
		state.status = kSTATUS_STOPPED;
		serial_println("echo: ERR|STOPPED\n");
	}
	
	
}

void gcPrintJob::settingsChaged() {
	//Update key elements and values
	
	// steps per sq second need to be updated to agree with the units per sq second (as they are what is used in the planner)
	reset_acceleration_rates(); //planner.cpp
	
	// Call updatePID (similar to when we have processed M301)
	#ifdef temperature_h
		temp_updatePID();
	#endif
}
void gcPrintJob::pollInactivity() {
	if( (millis() - lastActivityTime) >  state.max_inactive_time ){
		
		if(blocks_queued() == false) {
			disable_x();
			disable_y();
			disable_z();
			disable_e0();
			disable_e1();
		}
		
		if(state.max_inactive_time){
			temp_heaters_disable();
			
		}
	}
	
	check_axes_activity();
}
void gcPrintJob::pollInteractions() {
	//check buttons and other interactive control sources
	//etc

	//lcd_buttons_update();
	
}
void gcPrintJob::pollJob() {
	//check critical elements
	temp_checkheaters();
	
	
	
	//manage_inactivity()
	updateInteractions();
	
}
void gcPrintJob::updateInteractions() {
	
	//lcd_update
	
}
void gcPrintJob::run(){
	//our main run loop
	
	if(cmdCount < CFG_BUFSIZE){
		//there is space for more commands
		//load more lines from serial
		readLineFromSerial();
	}
	
	nextCommand();
	
	pollInteractions();
	pollJob();
	pollInactivity();
	checkHitEndstops(); //stepper.cpp
}
void gcPrintJob::nextCommand(){
	
	char cmdLine[CFG_MAX_CMD_SIZE] = "";
	if(cmdCount){ //we have lines from serial
		
		//debugDumpCommands();

		if(cmdIdx>=0){
			source = kCMD_SOURCE_SERIAL;
			strncpy(cmdLine, sbuff[cmdIdx], CFG_MAX_CMD_SIZE);
			
			cmdCount = (cmdCount-1);
			cmdIdx = (cmdIdx + 1) % CFG_BUFSIZE;
		}
		
	}else if(state.isSDPrinting && flgSDIsReady && flgSDPrintFileReady){
		source = kCMD_SOURCE_SD;
		if(!sd_print_file_cmd(cmdLine,CFG_MAX_CMD_SIZE)){
			cmdLine[0] = '\0';
			state.isSDPrinting = false;
			return;
		}
	}
	
	//serial_printf("echo: DBG|GCODE CMD[%d]=[%s] comment=[%s]\n", cmdIdx, gc.cmd, gc.comment);
	ltrim(cmdLine);
	
	if(strlen(cmdLine) == 0) return;
	//serial_printf("echo: DBG|GCODE CMD[%s]\n", cmdLine);
	
	if(cmdLine[0] == ';') return;
	
	parser.loadBuffer(cmdLine);
	int code = parser.code;
	int group = parser.group;
	
	if(group <= 0){
		serial_printf("echo: ERR|INVALID CMD|%s|\n", cmdLine);
		serial_println("ok");
		return;
	}
	
	//serial_printf("echo: DBG|GCODE PARSER[%d]=[%d] [%s]\n", parser.group, parser.code, parser.params);

	
	if(state.status >= kSTATUS_NOTREADY){
		serial_printf("echo: ERR|MACHINE NOT READY|\n", cmdLine);
		serial_println("ok");
		return;
	}
	
	int8 flgExecuted = false;
	for(int i=0; i < gc_methods_count; i++){
		//serial_printf("echo: DBG|GCODE FN GROUP[%c][%d]\n", gc_methods[i].group, gc_methods[i].code);
		if(gc_methods[i].group != parser.group ) continue;
		if(gc_methods[i].code != code ) continue;
		gc_methods[i].fn();
		flgExecuted = true;
		break;
	}
	
	if(!flgExecuted){
		serial_printf("echo: ERR|CMD NOT IMPLEMENTED|%s|\n", cmdLine);
		serial_println("ok");
		return;
	}
	
}
void gcPrintJob::doCommand(gc_cmd gc){
	
	//serial_printf("echo: DBG|GCODE CMD[%d]=[%s] comment=[%s]\n", cmdIdx, gc.cmd, gc.comment);
	if(strlen(gc.cmd) == 0) return;
	serial_printf("echo: DBG|GCODE CMD[%d]=[%s] comment=[%s]\n", cmdIdx, gc.cmd, gc.comment);
	
	parser.loadBuffer(gc.cmd);
	int code = (int) parser.code;
	int group = (int) parser.group;
	if(group <= 0){
		serial_printf("echo: ERR|INVALID CMD|%s|\n",gc.cmd);
		serial_println("ok");
		return;
	}
	//serial_printf("echo: DBG|GCODE PARSER[%d]=[%d]\n", parser.group, parser.code);
	
	
	if(state.status >= kSTATUS_NOTREADY){
		serial_printf("echo: ERR|MACHINE NOT READY|\n", gc.cmd);
		serial_println("ok");
		return;
	}
	
	int8 flgExecuted = false;
	for(int i=0; i < gc_methods_count; i++){
		//serial_printf("echo: DBG|GCODE FN GROUP[%c][%d]\n", gc_methods[i].group, gc_methods[i].code);
		if(gc_methods[i].group != parser.group ) continue;
		if(gc_methods[i].code != code ) continue;
		gc_methods[i].fn();
		flgExecuted = true;
		break;
	}
	
	if(!flgExecuted){
		serial_printf("echo: ERR|CMD NOT IMPLEMENTED|%s|\n", gc.cmd);
		serial_println("ok");
		return;
	}
	
	
	
}
void gcPrintJob::debugDumpCommands(){
	int i;

	
	serial_printf("echo: DBG|MSG QUEUE LENGHT [%d]\n", cmdCount);
	
	gc_cmd gcCMD;
	for(i=0; i<= CFG_BUFSIZE-1; i++){
		gcCMD = gc_commands[i];
		serial_printf("echo: DBG|MSG[%d]=[%s] comment=[%s]\n", i, gcCMD.cmd, gcCMD.comment);
	}
}
void gcPrintJob::readLineFromSerial() {
	static bool flgIgnoreLine = false;
	
	static int8 ridx = 0; //what line buffer r we going to write
	static int8 sz = 0;
	if(cmdCount == CFG_BUFSIZE) return; //we already have a line read and buff is full
	
	while( (Serial.available()>0) && (cmdCount < CFG_BUFSIZE)) {
		char ch = (char)Serial.read();
		//serial_printf("ridx=%d sz=%d ch=[%c] cmd=[%s]\n", ridx, sz, ch, sbuff[ridx]);
		if(ch == '\n'){
			sz = 0;
			flgIgnoreLine = false;
			ridx = (ridx + 1) % CFG_BUFSIZE;
			cmdCount++;
		}else if(flgIgnoreLine){
			continue;
		}else if(sz == (CFG_MAX_CMD_SIZE)){
			//Serial.print("echo: ERR|comment exceeds buffer\n");
			//Serial.flush();
			flgIgnoreLine = true;
		}else{
			sbuff[ridx][sz++] = ch;
			sbuff[ridx][sz] = '\0';
		}
	}
}

void gcPrintJob::loadCMDReadFromSerial(){
	int ch = 0;
	
	static bool flgInComment = false;
	static bool flgInChecksum = false;
	static bool flgInCommentSpecial = false;
	static bool flgIgnoreLine = false;
	
	gc_cmd *gcCMD;
	static byte checksum = 0;
	
	static int sz = -1;
	static int csz = -1;
	static char cmd[CFG_MAX_CMD_SIZE] = "";
	static char comment[CFG_MAX_CMD_SIZE] = "";

	while( Serial.available() > 0  && cmdCount < CFG_BUFSIZE) {
		ch = Serial.read();
		
		if(ch == '\0') continue;
		//serial_printf("echo:CH[%d]=[%d] cmd=[%s]\n", sz, ch, cmd);
		
		if(ch == '\n' || ch == '\r'){
			//Serial.print("echo: cmd[");
			//Serial.print(cmd);
			//Serial.print("]\n");
			flgIgnoreLine = false;
			flgInComment = false;
			flgInCommentSpecial = false;
			flgInChecksum = false;
			
			if(strlen(cmd)==0 || (cmd[0] == ';') ){
				//skip empty lines or comments
				//TODO checl thermalManager.manage_heater(
				continue;
			}
			
			if(++cmdReadIdx >= CFG_BUFSIZE) cmdReadIdx=0;
			gcCMD = &gc_commands[cmdReadIdx];
			gcCMD->source = kCMD_SOURCE_SERIAL;
			
			strcpy(gcCMD->cmd, cmd);
			strcpy(gcCMD->comment, comment);
			cmdCount++;
			
			sz = -1;
			memset(cmd,0,CFG_MAX_CMD_SIZE);
			memset(comment,0,CFG_MAX_CMD_SIZE);
			//cmd[0] = '\0';
			//comment[0] = '\0';
			
			//TODO fromsd[bufindw] = false; // how is this used?
			
			
		}else{
			if(flgIgnoreLine) continue;
			if(!flgInComment && !flgInCommentSpecial && (ch == '*')) {
				//serial_printf("echo:CH[%d]=[%d] cmd=[%s]\n", sz, ch, cmd);
				checksum = 0;
				flgInChecksum = true;
				continue;
			}
			
			if(flgInChecksum){
				///TODO IMPLEMENT CHECKSUM
				/*
				 checksum = commandToGetChecksumFor[0];
				 for (int i = 1; i < commandToGetChecksumFor.Length; i++)
				 {
				 checksum ^= commandToGetChecksumFor[i];
				 }
				 */
				continue;
			}
			if(ch == ';') {
				flgInComment = true;
				flgInCommentSpecial = false;
				continue;
			}
			if(flgInComment) continue;
			
			if(ch == '('){ //start special comment
				flgInCommentSpecial = true;
				comment[0] = '\0';
				continue;
			}
			
			
			if(flgInCommentSpecial){
				if(ch == ')'){
					flgInCommentSpecial = false;
					continue;
				}
				
				int csz = strlen(comment);
				
				//serial_printf("echo: DBG|comment CH[%d]=[%c][%d]=[%s]\n", csz, ch, ch, comment);
				
				if(csz >= (CFG_MAX_CMD_SIZE-1)){
					Serial.print("echo: ERR|comment exceeds buffer\n");
					//Serial.flush();
					flgIgnoreLine = true;
				}
				
				comment[csz] = ch;
				comment[csz+1] = '\0';
				continue;
			}
			
			//cmdbuffer[bufindw][serial_count++] = serial_char;
			
			if( (sz < 0) && ((ch==' ') || (ch=='\t')) ){ //skip whitespace at beggining...
				continue;
			}
			if(ch=='\t') ch = ' '; //replace taps with spaces
			
			sz++;
			if(sz >= (CFG_MAX_CMD_SIZE-1)){
				Serial.print("echo: ERR|command exceeds buffer\necho: cmd:[");
				cmd[sz] = '\0';
				Serial.print(cmd);
				Serial.print("]\n");
				//SERIAL.flush();
				flgIgnoreLine = true;
			}else{
				cmd[sz] = ch;
				cmd[sz+1] = '\0'; //end command
			}
			
		}
		
	}
}

