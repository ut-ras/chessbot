//
//  sd card support
//  marlin
//
//  Created by JOSE L CUEVAS on 9/1/19.
//  Copyright © 2019 JOSE L CUEVAS. All rights reserved.
//





#include "firmware.h"

#ifdef CFG_SDSUPPORT
#include <SD.h>

//https://github.com/greiman/SdFat implements long file names

bool flgSDPrintFileReady = false;
bool flgSDIsReady = false;

unsigned int sd_file_bytes_read = 0;
unsigned int sd_file_bytes_size = 0;

static File sdRoot;
static File sdFileSelected;



bool sd_init1(){
	
}
void sd_release(){
	if (sdRoot) {
		sdRoot.close();
	}
	flgSDIsReady = false;
	sd_print_file_close();
}
void sd_init(char *path){
	if (sdRoot) {
		sdRoot.close();
	}
	sd_print_file_close();
	
	flgSDIsReady = true;
	if (!SD.begin(SDCS_PIN)) {
		flgSDIsReady = false;
		serial_println("echo: ERROR|SDCARD|Init failed 1\n");
		return;
	}
	
	sdRoot = SD.open(path);
	if (!sdRoot) {
		flgSDIsReady = false;
		serial_println("echo: ERROR|SDCARD|Init failed 2\n");
		return;
	}
	
	flgSDIsReady = true;
}

void sd_list_files(uint8_t fmt){
	if(!flgSDIsReady) return;
	
	File entry;
	int i=0;
	if(fmt == kSD_LIST_STYLE_JSON){
		serial_printf("{\"dir\":\"%s\",\"files\":[", sdRoot.name());
	}else{
		serial_println("Begin file list:");
	}
	while((entry = sdRoot.openNextFile())){
		i++;
		
		if(fmt == kSD_LIST_STYLE_JSON){
			if(i == 1) Serial.print("\"");
			if(i>1) Serial.print(",\"");
			Serial.print(entry.name());
			if (entry.isDirectory()) Serial.print("/");
			Serial.print("\"");
		}else{
			Serial.print(entry.name());
			if (entry.isDirectory()) Serial.print("/");
			Serial.print("\n");
		}
	}
	
	if(fmt == kSD_LIST_STYLE_JSON){
		serial_println("]}");
	}else{
		serial_println("End file list\nok\n");
	}
}
void sd_print_file_close(){
	if(sdFileSelected){
		sdFileSelected.close();
	}
	flgSDPrintFileReady = false;
	
}
bool sd_print_file_readline(char *buff, int bsz){
	if (!flgSDPrintFileReady || !sdFileSelected.available()){
		return false;
	}
	
	
	int i =0;
	char c = sdFileSelected.read();
	//serial_printf("char[%d]=%d\n",i,c);
	
	while (c != '\n' && (i < bsz)) {
		buff[i] = c;
		buff[i + 1] = '\0';
		i++;
		
		if (!sdFileSelected.available()) break;
		c = sdFileSelected.read();
		//serial_printf("char[%d]=%d\n",i,c);
		//serial_printf("line[%s]\n",buff);
		if(i>30) break;
	}
	
	return true;
}
bool sd_print_file_cmd(char *buff, int bsz){
	if (!flgSDPrintFileReady || !sdFileSelected.available()){
		return false;
	}
	
	
	int i =0;
	char c = sdFileSelected.read();
	//serial_printf("char[%d]=%d\n",i,c);
	
	while (c != '\n') {
		
		if(i < bsz){
			buff[i++] = c;
			buff[i] = '\0';
		}
		
		if (!sdFileSelected.available()) break;
		c = sdFileSelected.read();
	}
	sd_file_bytes_read += i;
	
	return true;
}
bool sd_print_file_open(char *fname){
	flgSDPrintFileReady = false;
	sd_file_bytes_read = 0;
	sd_file_bytes_size = 0;
	
	if(!flgSDIsReady) return false;
	
	sdFileSelected = SD.open(fname, FILE_READ);
	if(!sdFileSelected){
		serial_printi("echo: ERROR|SDCARD|Failed to open|%s\n", fname);
		return false;
	}
	
	
	sd_file_bytes_size = sdFileSelected.size();
	flgSDPrintFileReady = true;
	return true;
}
#endif //CFG_SDSUPPORT
