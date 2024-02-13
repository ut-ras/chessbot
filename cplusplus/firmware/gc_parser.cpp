//
//  gc_parser.cpp
//  gc_parser
//
//  Created by JOSE L CUEVAS on 5/11/19.
//  Copyright © 2019 JOSE L CUEVAS. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "firmware.h"

gcodeParser::gcodeParser(){
	code = 0;
	group = 0;
	memset(comment, 0, 30);
	memset(params, 0, 100);
}

void gcodeParser::loadBuffer(char *cmdBuffer){
	#define kCommentSize 30
	#define kParamsSize 100
	#define kValueSize 30
	
	#define kReadingN 1
	#define kLooking 0
	#define kReadingSpecial 2
	#define kReadingGCode 3
	#define kReadingParams 4
	#define kReadingChecksum 5
	#define kReadingComment 6
	
	#define GCODE_GROUPS ((ch=='T') || (ch=='M') || (ch=='G') || (ch=='F'))
	#define CLEAR_VALUE sz = -1; value[0] = '\0';
	#define SKIP_WHITE while((*pos == ' ') || (*pos == '\t')){ pos++; } ch = *pos;
	
	pos = cmdBuffer;
	char ch;
	char value[kValueSize] = "";
	char *tend;
	
	uint8_t sz = -1;
	uint8_t szp = -1;
	uint8_t szc = -1;
	uint8_t state = kLooking;
	
	//reset previous values
	n = 0;
	code = 0;
	group = 0;
	checksum = 0;
	memset(comment, 0, kCommentSize);
	memset(params, 0, kParamsSize);

	
	while((*pos != '\0')){
		ch = *pos;
		
		//serial_printi("STATE=[%d] CH=%d group=%d code=%d value=[%s] params=[%s] comment=[%s] \n", state, ch, group, code, value, params, comment);
		if(ch == ';') break;
		if(ch == '\n') break;
		
		if( (ch == 'N') && (state == kLooking)){
			pos++;

			n = strtol(pos,&tend, 10); //cmd number
			pos = tend;
			
			SKIP_WHITE
			
			//serial_printi("FOUND N ch=[%c] [%d]\n", ch, n);
			continue;
		}else if(ch == '('){
			pos++;
			
			memset(comment, 0, kCommentSize);
			if( (tend = strchr(pos, ')')) != NULL){
				sz = tend-pos;
				strncpy(comment, pos, sz);
				comment[sz] = '\0';
				pos += sz + 1;
			}
			SKIP_WHITE
			
			//serial_printi("FOUND COMMENT2 ch=[%d] [%s] %d\n", ch, comment, strlen(comment));
			continue;
		}else if( GCODE_GROUPS && (state == kLooking)){
			
			group = ch;
			pos++;
			code = strtol(pos,&tend, 10); //cmd number
			pos = tend;
			if(*pos == '.'){
				code = code * 1000;
				code = code + strtol(pos,&tend, 10);
				
				pos = tend;
			}
			ch = *pos;
			
			state = kReadingParams;
			
			szp = -1;
			memset(params, 0, kParamsSize);
			
			
			SKIP_WHITE
			//serial_printf("FOUND GCODE ch=[%d] group=[%c] code=[%d] params=[%s]\n", ch, group, code, params);
			continue;
		}else if( state == kReadingParams ){
			
			if(ch == '*'){
				state = kReadingChecksum;
				pos++;
				checksum = (int) strtod(pos,&tend);
				pos = tend;
				continue;
			}
			if(++szp < kParamsSize){
				params[szp] = ch;
				params[szp+1] = '\0';
				
			}
			
			pos++;
		}else if( (ch == ' ') || (ch == '\t') ){
			pos++;
		}
	}
	
	pos = params;
	//serial_printf("At End ch=[%d] group=[%c] code=[%d] params=[%s] comments=[%s]\n", ch, group, code, params, comment);
}



int gcodeParser::parseString(int mp, char *out, int out_sz){
	
	char ch;
	int i=-1;
	int sz=0;
	
	char * p = pos;
	p += mp; //move pointer to start
	
	if(*p == '\0') return sz;
	
	if(*p == '+' || *p == '-'){
		out[++i] = *p;
		out[i+1] = '\0';
		p++; sz++;
	}
	
	char kValid[20] = "._-/\\[]$@?:+";
	while(*p != '\0'){
		ch = *p;
		if((i+1) == out_sz) break;
		if( (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')) {
			p++; sz++;
			out[++i] = ch;
			out[i+1] = '\0';
		}else if( strchr(kValid,ch)!= NULL ){
			p++; sz++;
			out[++i] = ch;
			out[i+1] = '\0';
		}else{
			break;
		}
	}
	//printf("n=%s\n", out);
	return sz;
}
float gcodeParser::parseFloat(int mp, float defValue){
	
	char ch;
	char value[25] = "";
	int i=-1;
	int stop = 0;
	int mz=0;
	
	float out = defValue;
	char * p = pos;
	p += mp; //move pointer
	
	if(*p == '\0') return out;
	
	if(*p == '+' || *p == '-'){
		value[++i] = *p;
		value[i+1] = '\0';
		p++; mz++;
	}
	
	while(*p != '\0' && stop == 0){
		ch = *p;
		if(i+1>=25) break;
		switch(ch){
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '.':
				p++; mz++;
				value[++i] = ch;
				value[i+1] = '\0';
				break;
			default:
				stop = 1;
				break;
		}
		
	}
	
	if(mz > 0){
		out = strtod(value, NULL);
		
	}
	return out;
}

int gcodeParser::findParam(char code){
	char * p = pos;
	char ch;
	
	if(*p == '\0') return -1;
	//if(*p != ' ') return NULL; //next param must have a space delimiter
	int mz=0;
	while((*p != '\0') && (*p != code)){
		ch = *p;
		if(ch == '('){
			break;
		}
		p++; mz++;
	}
	
	if(*p != code){
		return -1;
	}
	
	p++; mz++;
	//pos+= mz;
	return mz;
}
float gcodeParser::getNumeric(const char param, float defValue){
	int found;
	float value;
	
	found = findParam(param);
	value = defValue;
	
	if(found >= 0){
		value = parseFloat(found, defValue);
	}
	
	return value;
}
bool gcodeParser::getBool(const char param){
	
	int found;
	float value;
	
	found = findParam(param);
	
	if(found >= 0){
		value = parseFloat(found, -1);
		if(value < 0) return true;
		return (value > 0);
	}
	
	return false;
}
bool gcodeParser::getNParams(const char *params, float *values){
	int c,i, found;
	int m=0;
	float value;
	char key;
	if(params == NULL) return false;
	c = strlen(params);
	
	if(c == 0) return false;
	
	for(i=0; i<c; i++){
		key = params[i];
		found = findParam(key);
		value = NULL;
		
		if(found >= 0){
			m++;
			value = parseFloat(found, NULL);
		}
		values[i] = value;
	}
	
	return (m>0);
}
