#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#include "gh_config.h"

#include "gh_string.h"
#include "gh_log.h"

extern int errno;

void gh_log(uint8_t type, char* message, ...){
	va_list args1, args2;
	va_start(args1, message);
	va_copy(args2, args1);

	ssize_t len = vsnprintf(NULL, 0, message, args1);
	va_end(args1);
	
	g_str_t* error = string_init();
		
	string_allocate(error, len+1);
	vsnprintf(error->str + error->size - len - 1, len+1, message, args2);
		
	va_end(args2);
	
	if(type == G_FILE){
		string_append(error, "%s\n", strerror(errno));
	}
	
	string_save_to_file(error, "g_log.txt");	
	string_clean(error);
}
