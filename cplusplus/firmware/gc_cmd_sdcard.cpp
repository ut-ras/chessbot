#include "firmware.h"


#ifdef CFG_SDSUPPORT
extern unsigned long starttime;
extern unsigned long stoptime;



// M20 - list SD card
void gcode_M20(){
	char sdFileName[26] = "/";
	int found;
	
	if((found = job.parser.findParam('P')) < 0){
		serial_echo("ERR|M20|Missing P argument");
		return;
	}
	int sz = job.parser.parseString(found, sdFileName, 26);
	if(sz <= 0){
		serial_echo("ERR|M20|Invalid P argument");
		return;
	}
	
	int fmt = job.parser.getNumeric('S', 0);
	
	sd_init(sdFileName);
	if(!flgSDIsReady){
		serial_echo("ERR|M20|SD Failed");
		return;
	}
	
	sd_list_files(fmt);
	
	
}

// M21 - init SD card
void gcode_M21(){
	char sdFileName[26] = "/";
	
	int found;
	
	if((found = job.parser.findParam('P')) >= 0){
		
		int sz = job.parser.parseString(found, sdFileName, 26);
		if(sz <= 0){
			serial_println("ERR|M21|Invalid P argument\nok");
			return;
		}
	}
	
	sd_init(sdFileName);
	if(!flgSDIsReady){
		serial_println("ERR|M21|SD Failed\nok");
		return;
	}
	
	SEND_OK
}

//M22 - release SD card
void gcode_M22(){
	if(state.isSDPrinting){
		serial_println("echo: ERR|M22|SDCARD printing cancelled");
		state.isSDPrinting = false;
	}
	
	sd_release();
	SEND_OK
}

//M23 - Select file
void gcode_M23(){
	if(!flgSDIsReady){
		serial_println("echo: ERR|M23|SD not ready\nok");
		return;
	}
	char sdFileName[36];

	int sz = job.parser.parseString(0, sdFileName, 36);
	if(sz <= 0){
		serial_println("echo: ERR|M23|Open failed 1\nok");
		return;
	}
	
	if(!sd_print_file_open(sdFileName)){
		serial_printi("echo: ERR|M23|Open failed|%s|\nok\n", sdFileName);
	}
	
	
	
	SEND_OK
}

//M24 - Start SD print
void gcode_M24(){
	
	if(flgSDIsReady && flgSDPrintFileReady){
		state.isSDPrinting = true;
	}

	SEND_OK
}

//M25 - Pause SD print
void gcode_M25(){
	if(flgSDIsReady && flgSDPrintFileReady){
		state.isSDPrinting = !state.isSDPrinting;
	}else{
		serial_println("echo: ERR|M24|Not printing");
		
	}
	SEND_OK
}

//M26 - Set SD index
void gcode_M26(){
	/*
	if( !card.cardOK ){
		SERIAL_ECHOLN("echo:error:[ERR-SD001] error sd card is not ready");
		return;
	}
	
	int found = job.parser.findParam('S');
	if(found < 0){
		SERIAL_ECHOLN("echo:error:[ERR-SD003] missing file read position");
		return;
	}
	float sdFilePos = job.parser.parseFloat(found, -1);
	
	card.setIndex(sdFilePos);
	 */
}


//M27 - Get SD status
void gcode_M27(){
	
	if(flgSDIsReady && flgSDPrintFileReady){
		serial_printf("SD printing byte %d/%d\n", sd_file_bytes_read, sd_file_bytes_size);
	}else{
		serial_println("Not SD printing");
	}
	SEND_OK
}

//M524 - Abort an SD print
void gcode_M524(){
	if(flgSDIsReady && flgSDPrintFileReady){
		state.isSDPrinting = false;
		sd_print_file_close();
	}
	
	SEND_OK
}

//M30 <filename> Delete File
void gcode_M30(){
	if(!flgSDIsReady){
		serial_println("echo: ERR|M30|SD not ready\nok");
		return;
	}
	char sdFileName[36];
	
	int sz = job.parser.parseString(0, sdFileName, 36);
	if(sz <= 0){
		serial_println("echo: ERR|M30|Open failed 1\nok");
		return;
	}
	
	if( !SD.remove(sdFileName) ){
		serial_printf("echo: ERROR|M30|Delete failed|%s\n", sdFileName);
	}
	
	SEND_OK
}

//M32: Select file and start SD print
void gcode_M32(){
	
	
	if(!flgSDIsReady){
		serial_println("echo: ERR|M32|SD not ready\nok");
		return;
	}
	char sdFileName[36];
	
	int sz = job.parser.parseString(0, sdFileName, 36);
	if(sz <= 0){
		serial_println("echo: ERR|M32|Open failed 1\nok");
		return;
	}
	
	if(!sd_print_file_open(sdFileName)){
		serial_printi("echo: ERR|M32|Open failed|%s|\nok\n", sdFileName);
	}
	
	state.isSDPrinting = true;
	
	SEND_OK
}
#endif
