//
//  serial.h
//  
//
//  Created by JOSE L CUEVAS on 9/1/19.
//

#ifndef serial_h
#define serial_h


#define SEND_OK serial_println("ok");

void serial_init();
void serial_println(char *msg);
void serial_echo(char *msg);
void serial_printf(char *format,...);
void serial_printi(const char *format, ...);

//Things to write to serial from Program memory. Saves 400 to 2k of RAM.
FORCE_INLINE void serial_print(const char *str){
	char ch=pgm_read_byte(str);
	while(ch){
		Serial.write(ch);
		ch=pgm_read_byte(++str);
	}
}

#endif /* serial_h */
