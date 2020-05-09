#ifndef G_BUFF_H
#define G_BUFF_H

#include <inttypes.h>

#include "gh_config.h"

typedef struct Buffer {
	char* buff;
	size_t size;
} g_buff_t;

g_buff_t* buffer_init();
uint8_t buffer_allocate(g_buff_t* , size_t );
uint8_t buffer_reallocate(g_buff_t* , size_t );
uint8_t buffer_char(g_buff_t* , char* );
uint8_t buffer_copy(g_buff_t* , g_buff_t* );
uint8_t buffer_concate(g_buff_t* , g_buff_t* );
uint8_t buffer_add(g_buff_t* , const char*);
uint16_t buffer_extract(g_buff_t* , char* , int , int );

void buffer_free(g_buff_t*);
void buffer_destroy_container(g_buff_t* gbuff);
void buffer_clean(g_buff_t*);


#endif
