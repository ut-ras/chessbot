//
//  gc_parser.h
//  gc_parser
//
//  Created by JOSE L CUEVAS on 5/11/19.
//  Copyright © 2019 JOSE L CUEVAS. All rights reserved.
//

#ifndef gcode_hpp
#define gcode_hpp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define CFG_MAX_CMD_SIZE 96
class gcodeParser {
public:
	char *pos;
	
	char group;
	int code;
	int n;
	int checksum;
	char comment[30];
	char params[100];
	
	float paramValueFloat;
	float paramValueInt;
	
	gcodeParser();
	
	float parseFloat(int mp, float defValue);
	int parseString(int mp, char *out, int out_sz);
	int findParam(char code);
	float getNumeric(const char param, float defValue);
	bool getBool(const char param);
	bool getNParams(const char *params, float *values);
	void loadBuffer(char *cmdBuffer);
	
	inline int16_t getInt(const char param, int defValue) { return (int16_t)getNumeric(param, defValue); }
	inline uint16_t getUShort(const char param, int defValue) { return (uint16_t)getNumeric(param, defValue); }
	inline char getChar(const char param, char defValue) { return (char)getNumeric(param, (float)defValue); }
	
	
	
	inline int16_t parseInt(int mp, float defValue) { return (int16_t)parseFloat(mp, defValue); }
	inline uint16_t parseUShort(int mp, float defValue) { return (uint16_t)parseFloat(mp, defValue); }
	inline char parseChar(int mp, float defValue) { return (char)parseFloat(mp, defValue); }
};
#endif /* gcode_hpp */
