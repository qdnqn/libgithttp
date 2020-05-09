#ifndef G_STRING_H
#define G_STRING_H

#include <inttypes.h>

#include "gh_config.h"

#include "gh_buffer.h"

typedef struct String {
	char* str;
	size_t size;
} g_str_t;


g_str_t* string_init();
uint8_t string_allocate(g_str_t* str, size_t length);
uint8_t string_reallocate(g_str_t* str, size_t length);
uint8_t string_char(g_str_t* gstr, char* literal);
uint8_t string_copy(g_str_t* gstr1, g_str_t* gstr2);
uint8_t string_to_buffer(g_buff_t* gbuff, g_str_t* gstr);
uint8_t buffer_to_string(g_str_t* gstr, g_buff_t* gbuff);
uint8_t string_concate(g_str_t* gstr1, g_str_t* gstr2);
uint8_t string_add(g_str_t* gstr, const char*);
uint16_t string_extract(g_str_t* gstr, char* literal, int start, int end);
uint8_t string_hexsign(g_str_t* str);
uint8_t string_hexsign_exclude_sign(g_str_t* gstr);
uint8_t string_append(g_str_t* gstr, char* formated_str, ...);
uint8_t string_append_hexsign(g_str_t* gstr, char* formated_str, ...);
uint8_t string_copy_bytes(g_str_t* gstr1, g_str_t* gstr2, int start, int length);
uint8_t string_copy_char_nullterminate(g_str_t* gstr1, char* str, int size);
uint16_t string_copy_bytes_stop_at_char(g_str_t* gstr1, g_str_t* gstr2, int start, char end);
uint8_t string_save_to_file_binary(g_str_t* data, char* file);
uint8_t string_save_to_file(g_str_t* data, char* file);
uint8_t string_load_from_file_binary(g_str_t* pack, char* file);
uint8_t string_load_from_file(g_str_t* pack, char* file);
uint8_t string_load_from_file_bytes(g_str_t* pack, char* file, int length);

void string_debug(g_str_t* gstr);
void string_free(g_str_t*);
void string_destroy_container(g_str_t* gstr);
void string_clean(g_str_t*);
#endif
