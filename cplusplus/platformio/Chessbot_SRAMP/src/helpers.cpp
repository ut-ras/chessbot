//
//  helpers.c
//  
//
//  Created by JOSE L CUEVAS on 9/9/19.
//

#include "helpers.h"


void ltrim(char *src){
	char *dst;
	
	/* find position of first non-space character */
	for (dst=src; *src == ' '; src++) {;}
	
	/* nothing to do */
	if (dst==src) return;
	
	/* K&R style strcpy() */
	while ((*dst++ = *src++)) {;}
	
	return;
}
