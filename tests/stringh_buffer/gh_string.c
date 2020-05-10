#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <stdarg.h>

#include "gh_config.h"

#include "gh_buffer.h"
#include "gh_string.h"
#include "gh_log.h"

g_str_t* string_init(){
	g_str_t* temp = malloc(sizeof(g_str_t));
	temp->str = NULL;
	temp->size = 0;
	
	return temp;
}

uint8_t string_allocate(g_str_t* gstr, size_t length){
	gstr->str = malloc(length * sizeof(char));
	gstr->size = length;
	
	int i;
	for(i=0;i<gstr->size;i++)
		*(gstr->str+i) = '\0';
	
	if(gstr->str != 0){
		return 0;
	} else {
		return -1;
	}
}

uint8_t string_reallocate(g_str_t* gstr, size_t length){
	gstr->str = realloc(gstr->str, length * sizeof(char) + gstr->size * sizeof(char));
	gstr->size = gstr->size+length;
	
	int i;
	for(i=gstr->size-length;i<gstr->size;i++)
		*(gstr->str+i) = '\0';
	
	if(gstr->str != 0){
		return 0;
	} else {
		return -1;
	}
}

uint8_t string_char(g_str_t* gstr, char* literal){
	if(gstr != NULL){
		size_t length = strlen(literal) + 1;
		
		if(length != gstr->size){
			string_free(gstr);
			string_allocate(gstr, length);
			strcpy(gstr->str, literal);
		} else {
			strcpy(gstr->str, literal);
		}
		
		return 0;
	} else {
		return -1;
	}
}

uint8_t string_copy(g_str_t* gstr1, g_str_t* gstr2){
	if(gstr1 != NULL && gstr2 != NULL){
		size_t length = gstr2->size;
		
		if(length != gstr1->size){
			string_free(gstr1);
			string_allocate(gstr1, length);
			strcpy(gstr1->str, gstr2->str);
		} else {
			strcpy(gstr1->str, gstr2->str);
		}
		
		return 0;
	} else {
		return -1;
	}
}

uint8_t string_to_buffer(g_buff_t* gbuff, g_str_t* gstr){
	if(gstr != NULL && gbuff != NULL){
		size_t length = gstr->size-1;
		
		if(length != gbuff->size){
			buffer_free(gbuff);
			buffer_allocate(gbuff, length);
			memcpy(gbuff->buff, gstr->str, length);
		} else {
			memcpy(gbuff->buff, gstr->str, length);
		}
		
		return 0;
	} else {
		return -1;
	}
}

uint8_t buffer_to_string(g_str_t* gstr, g_buff_t* gbuff){
	if(gstr != NULL && gbuff != NULL){
		size_t length = gbuff->size+1;
		
		if(length != gstr->size){
			string_free(gstr);
			string_allocate(gstr, length+1);
			memcpy(gstr->str, gbuff->buff, length-1);
		} else {
			memcpy(gstr->str, gbuff->buff, length-1);
		}
		
		*(gstr->str+length) = '\0';
		
		return 0;
	} else {
		return -1;
	}
}

uint8_t string_concate(g_str_t* gstr1, g_str_t* gstr2){
	if(gstr1 != NULL && gstr2 != NULL){
		size_t length = gstr2->size-1;
		
		if(gstr1->size == 0){
			string_allocate(gstr1, length+1);
			memcpy(gstr1->str + gstr1->size - length - 1, gstr2->str, length);
		} else {
			string_reallocate(gstr1, length);
			memcpy(gstr1->str + gstr1->size - length - 1, gstr2->str, length);
		}	
		
		*(gstr1->str + gstr1->size - 1) = '\0';
		
		return 0;
	} else {
		return -1;
	}
}

uint8_t string_add(g_str_t* gstr, const char* literal){
	if(gstr != NULL){
		size_t length = strlen(literal);
		
		if(gstr->size == 0){
			string_allocate(gstr, length+1);
			memcpy(gstr->str, literal, length+1);
		} else {
			string_reallocate(gstr, length);
			memcpy(gstr->str+gstr->size - length - 1, literal, length);
		}
		
		//*(gstr->str+gstr->size-1) = '\0';
		
		return 0;
	} else {
		return -1;
	}
}

uint16_t string_extract(g_str_t* gstr, char* literal, int start, int end)
{
	if(start < end)
	{
		if(gstr->str != NULL)
			string_free(gstr);
			
		size_t size = end - start, j = 0, i;
		string_allocate(gstr, size+1);
		
		for(i=start;i<end;i++)
		{
			*(gstr->str + j) = literal[i];
			++j;
		}
		
		*(gstr->str + j) = '\0';
		
		return size;
	}
	else 
	{
		return -1;
	}
}

/*
 * !string_copy_bytes takes empty initialized gstring as first argument and allocate needed memory
 */
 
uint8_t string_copy_bytes(g_str_t* gstr1, g_str_t* gstr2, int start, int length){
	string_allocate(gstr1, length+1);
	memcpy(gstr1->str, gstr2->str+start, length);
	*(gstr1->str+gstr1->size-1) = '\0';
	
	return 0;
}

uint8_t string_copy_char_nullterminate(g_str_t* gstr1, char* str, int size){
	string_allocate(gstr1, size+1);
	memcpy(gstr1->str, str, size);
	*(gstr1->str+size) = '\0';
	
	return 0;
}

uint16_t string_copy_bytes_stop_at_char(g_str_t* gstr1, g_str_t* gstr2, int start, char end){
	size_t i = 0;
	
	for (i = start; i < gstr2->size-1; i++){
		if (gstr2->str[i] == end){
			break;
		}
	}
		
	int length = i - start;	// Neat
		
	string_allocate(gstr1, length+1);
	memcpy(gstr1->str, gstr2->str+start, length);
	*(gstr1->str+gstr1->size-1) = '\0';
	
	return length;
}

/*
 * Sign g_str_t* string with hex value of its length excluding terminating character
 */ 

uint8_t string_hexsign(g_str_t* gstr){
		g_str_t* temp = string_init();
		string_allocate(temp, gstr->size+4);
		snprintf(temp->str, 5, "%04lx", gstr->size - 1 + 4);
		memcpy(temp->str+4, gstr->str, gstr->size);
		
		string_free(gstr);
		
		gstr->str = temp->str;
		gstr->size = temp->size;	
		
		string_destroy_container(temp);
		
		return 0;
}

uint8_t string_hexsign_exclude_sign(g_str_t* gstr){
		g_str_t* temp = string_init();
		string_allocate(temp, gstr->size+4);
		snprintf(temp->str, 5, "%04lx", gstr->size - 1);
		memcpy(temp->str+4, gstr->str, gstr->size);
		
		string_free(gstr);
		
		gstr->str = temp->str;
		gstr->size = temp->size;	
		
		string_destroy_container(temp);
		
		return 0;
}

uint8_t string_append(g_str_t* gstr, char* formated_str, ...)
{
	va_list args1, args2;
	va_start(args1, formated_str);
	va_copy(args2, args1);

	ssize_t len = vsnprintf(NULL, 0, formated_str, args1);								// Only number of chars without terminating char '\0'
	va_end(args1);
		
	if(gstr->size == 0){
		string_allocate(gstr, len+1);
		vsnprintf(gstr->str + gstr->size - len - 1, len+1, formated_str, args2);
	} else {
		string_reallocate(gstr, len);
		vsnprintf(gstr->str + gstr->size - len - 1, len+1, formated_str, args2);
	}
	
	va_end(args2);
	
	return 0;
}

uint8_t string_append_hexsign(g_str_t* gstr, char* formated_str, ...)
{
	va_list args1, args2;
	va_start(args1, formated_str);
	va_copy(args2, args1);

	ssize_t len = vsnprintf(NULL, 0, formated_str, args1);								// Only number of chars without terminating char '\0'
	va_end(args1);
	
	g_str_t* temp = string_init();
	string_allocate(temp, len+1);
	
	vsnprintf(temp->str, len+1, formated_str, args2);
	
	string_hexsign(temp);	
	string_concate(gstr, temp);
	string_clean(temp);
	
	va_end(args2);
	
	return 0;
}

/*
 * Save g_str_t to bianry file without last char! (Often \0 terminating character)
 */

uint8_t string_save_to_file_binary(g_str_t* data,  char* file){
	FILE *fp = NULL;
	fp = fopen(file, "wb");
	
	if (fp == NULL){
		gh_log(G_FILE, "Error opening file: ");	
		return -1;
	}
	
	fwrite(data->str, data->size-1, 1, fp);
		
	fclose(fp);
	
	return 0;
}

uint8_t string_save_to_file(g_str_t* data,  char* file){
	FILE *fp = NULL;
	fp = fopen(file, "w");
	
	if (fp == NULL){
		gh_log(G_FILE, "Error opening file: ");	
		return -1;
	}
	
	fprintf(fp, "%s", data->str);
	
	fclose(fp);
	
	return 0;
}

uint8_t string_load_from_file_binary(g_str_t* pack, char* file){
	FILE *fp;
	fp = fopen (file, "rb");
	
	if (fp == NULL){
		gh_log(G_FILE, "Error opening file: ");	
		return -1;
	}
	
	size_t sz = 0;
	fseek(fp, 0L, SEEK_END);
	sz = ftell(fp);
			
	string_allocate(pack, sz);	 		
				
	fseek(fp, 0L, SEEK_SET);
	fread(pack->str, sz, 1, fp);	
	fclose(fp);
	
	pack->size = sz;
	
	return 0;
}

uint8_t string_load_from_file(g_str_t* pack, char* file){
	FILE *fp;
	fp = fopen (file, "r");
	
	if (fp == NULL){
		gh_log(G_FILE, "Error opening file: ");	
		return -1;
	}
	
	size_t sz = 0;
	fseek(fp, 0L, SEEK_END);
	sz = ftell(fp);
				
	string_allocate(pack, sz+1);	 		
				
	fseek(fp, 0L, SEEK_SET);
	fread(pack->str, sz+1, 1, fp);	
	fclose(fp);
	
	pack->size = sz;
	
	return 0;
}


uint8_t string_load_from_file_bytes(g_str_t* pack, char* file, int length){
	FILE *fp;
	fp = fopen (file, "r");
	
	if (fp == NULL){
		gh_log(G_FILE, "Error opening file: ");	
		return -1;
	}
					
	string_allocate(pack, length+1);	 		
				
	fseek(fp, 0L, SEEK_SET);
	fread(pack->str, length, 1, fp);	
	fclose(fp);
	
	pack->str[length] = '\0';
	pack->size = length+1;
	
	return 0;
}

void string_debug(g_str_t* gstr){
		int i;
		
		printf("String: ");
		
		for(i=0;i<gstr->size;i++){
			if(gstr->str[i] == 0)
				printf("[NULL]");
			else
				printf("%c",gstr->str[i]);
		}
		
		printf("\n");
}

void string_free(g_str_t* gstr){
	if(gstr != NULL){
		if(gstr->str != NULL){
			free(gstr->str);
			
			gstr->str = NULL;
			gstr->size = 0;
		}
	}
}

void string_destroy_container(g_str_t* gstr){
	free(gstr);
}

void string_clean(g_str_t* gstr){
		string_free(gstr);
		
		if(gstr != NULL)
			string_destroy_container(gstr);
}
