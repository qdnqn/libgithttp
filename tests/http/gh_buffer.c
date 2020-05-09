#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include "gh_config.h"
#include "gh_string.h"
#include "gh_buffer.h"

g_buff_t* buffer_init(){
	g_buff_t* temp = malloc(sizeof(g_buff_t));
	temp->buff = NULL;
	temp->size = 0;
	
	return temp;
}

uint8_t buffer_allocate(g_buff_t* gbuff, size_t length){
	gbuff->buff = malloc(length * sizeof(char));
	gbuff->size = length;
	
	int i;
	for(i=0;i<gbuff->size;i++)
		*(gbuff->buff+i) = '\0';
	
	if(gbuff->buff != 0){
		return 0;
	} else {
		return -1;
	}
}

uint8_t buffer_reallocate(g_buff_t* gbuff, size_t length){
	gbuff->buff = realloc(gbuff->buff, length * sizeof(char) + gbuff->size * sizeof(char));
	gbuff->size = length;
	
	if(gbuff->buff != 0){
		return 0;
	} else {
		return -1;
	}
}

uint8_t buffer_char(g_buff_t* gbuff, char* literal){
	if(gbuff != NULL){
		size_t length = strlen(literal);
				
		if(length != gbuff->size){
			buffer_free(gbuff);
			buffer_allocate(gbuff, length);
			memcpy(gbuff->buff, literal, length);
		} else {
			memcpy(gbuff->buff, literal, length);
		}
		
		return 0;
	} else {
		return -1;
	}
}

uint8_t buffer_copy(g_buff_t* gbuff1, g_buff_t* gbuff2){
	if(gbuff1 != NULL && gbuff2 != NULL){
		size_t length = gbuff2->size;
		
		if(length != gbuff1->size){
			buffer_free(gbuff1);
			buffer_allocate(gbuff1, length);
			memcpy(gbuff1->buff, gbuff2->buff, length);
		} else {
			memcpy(gbuff1->buff, gbuff2->buff, length);
		}
		
		return 0;
	} else {
		return -1;
	}
}

uint8_t buffer_concate(g_buff_t* gbuff1, g_buff_t* gbuff2){
	if(gbuff1 != NULL && gbuff2 != NULL){
		size_t length = gbuff2->size;
		
		buffer_reallocate(gbuff1, length);
		memcpy(gbuff1->buff+gbuff1->size-length, gbuff2->buff, length);
				
		return 0;
	} else {
		return -1;
	}
}

uint8_t buffer_add(g_buff_t* gbuff, const char* literal){
	if(gbuff != NULL){
		size_t length = strlen(literal);
		
		buffer_reallocate(gbuff, length);
		memcpy(gbuff->buff+gbuff->size, literal, length);
				
		return 0;
	} else {
		return -1;
	}
}

uint8_t buffer_hexsign(g_buff_t* gbuff){
		g_buff_t* temp = buffer_init();
		buffer_allocate(temp, gbuff->size+4);
		snprintf(temp->buff, 5, "%04lx", gbuff->size + 4);
		memcpy(temp->buff+4, gbuff->buff, gbuff->size);
		
		buffer_free(gbuff);
		
		gbuff->buff = temp->buff;
		gbuff->size = temp->size;	
		
		buffer_destroy_container(temp);
		
		return 0;
}

uint16_t buff_extract(g_buff_t* gbuff, char* literal, int start, int end)
{
	if(start < end)
	{
		if(gbuff->buff != NULL)
			buffer_free(gbuff);
			
		size_t size = end - start, j = 0, i;
		buffer_allocate(gbuff, size);
		
		for(i=start;i<end;i++)
		{
			*(gbuff->buff + j) = literal[i];
			++j;
		}
				
		return size;
	}
	else 
	{
		return -1;
	}
}

void buffer_free(g_buff_t* gbuff){
	if(gbuff != NULL){
		if(gbuff->buff != NULL){
			free(gbuff->buff);
			gbuff->buff = NULL;
			gbuff->size = 0;
		}
	}
}

void buffer_destroy_container(g_buff_t* gbuff){
	free(gbuff);
}

void buffer_clean(g_buff_t* gbuff){
	buffer_free(gbuff);
	
	if(gbuff != NULL)
		buffer_destroy_container(gbuff);
}

