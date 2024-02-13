
#include "firmware.h"


void serial_init(){
	Serial.begin(CFG_BAUDRATE);
	Serial.print("start\n");
}


void serial_printf(char *format,...){
	char s[255];
	memset(s, 0, 255);
	
	va_list ap;
	va_start(ap, format);
	//fprintf(stdout, format, ap);
	vsnprintf(s, 255, format, ap);
	va_end(ap);
	Serial.print(s);
}

//a simple printf like interpolation
void serial_printi(const char *format, ...){
	
	char ch;
	bool flgInterpolate = false;
	va_list args;
	va_start( args, format );
	for( ; *format ; ++format ){
		ch = *format;
		if(flgInterpolate){
			flgInterpolate = false;
			if((ch=='d') || (ch=='c')){
				Serial.print(va_arg(args, int));
			}else if((ch=='l')){
				Serial.print(va_arg(args, long));
			}else if((ch=='b')){
				if(va_arg(args, int)){
					Serial.print("TRUE");
				}else{
					Serial.print("FALSE");
				}
			}else if(ch=='s'){
				Serial.print(va_arg(args, char*));
			}else if(ch=='o'){
				Serial.print(va_arg(args, unsigned int));
			}else if((ch=='f') || (ch=='e') || (ch=='a') || (ch=='g')){
				Serial.print(va_arg(args, double));
			}else{
				Serial.print('%');
				Serial.print(ch);
			}
		}else if(ch=='%'){
			flgInterpolate = true;
		}else{
			Serial.print(ch);
		}
	}
	
	va_end( args );
}

void serial_println(char *msg){
	Serial.print(msg);
	Serial.print("\n");
	
}
void serial_echo(char *msg){
	char s[255];
	sprintf(s, "echo: %s\n", msg);
	Serial.print(s);
}
