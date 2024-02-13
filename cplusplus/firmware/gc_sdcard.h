//
//  motion.cpp
//  marlin
//
//  Created by JOSE L CUEVAS on 9/1/19.
//  Copyright © 2019 JOSE L CUEVAS. All rights reserved.
//


#ifndef sdcard_h
#define sdcard_h

#define kSD_LIST_STYLE_NORMAL 0 //marlin style
#define kSD_LIST_STYLE_JSON 2

extern bool flgSDPrintFileReady;
extern bool flgSDIsReady;

extern unsigned int sd_file_bytes_read;
extern unsigned int sd_file_bytes_size;

void sd_init(char *path);
void sd_release();
void sd_print_file_close();
bool sd_print_file_readline(char *buff, int bsz);
bool sd_print_file_open(char *fname);
void sd_list_files(uint8_t fmt);
bool sd_print_file_cmd(char *buff, int bsz);

#if (SDCARDDETECT > -1)
#	ifdef CFG_SD_DETECTINVERTED
#		define IS_SD_INSERTED (READ(SD_DETECT_PIN)!=0)
#	else
#		define IS_SD_INSERTED (READ(SD_DETECT_PIN)==0)
#	endif //SDCARDTETECTINVERTED
#else
//If we don't have a card detect line, aways asume the card is inserted
#	define IS_SD_INSERTED false
#endif

#endif //sdcard
