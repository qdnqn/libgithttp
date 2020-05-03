#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "g_buffer.h"
#include "g_string.h"
#include "g_http.h"

static uint8_t parser_pushline(g_http_resp* http, g_str_t* line);

uint8_t parser_capabilities(g_http_resp* http, g_str_t* caps){
	g_str_t* temp = string_init();
		
	//printf("Processing line:%s", caps->str);
	
	int i = 0, start_address = 0, copy_bytes = 0;
	for (i=0;i<caps->size;i++){
		if (caps->str[i] != ' ' && caps->str[i] != '\n'){
			copy_bytes++;
		} else {		
			if (caps->str[i] == '\n'){
				string_copy_bytes(temp, caps, start_address+6, copy_bytes-6);
				add_cap(http, "agent", temp->str);
			} else {
				string_copy_bytes(temp, caps, start_address, copy_bytes);
				add_cap(http, temp->str, "");
			}
			
			//string_debug(temp);//Optional for debugging
			string_free(temp);
			
			start_address += copy_bytes+1;
			copy_bytes = 0;			
		}
	}	
	
	string_clean(temp);
	
	return 0;
}

uint8_t parser_refs(g_http_resp* http, char *file){
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	
	fp = fopen(file, "r");
	if (fp == NULL)
			return -1;
	
	g_str_t* det_zeros = string_init();
	g_str_t* type = string_init();
	g_str_t* oid = string_init();
	g_str_t* caps = string_init();
	int i, flg;
	
	while ((read = getline(&line, &len, fp)) != -1) {			
		string_extract(type, line, 4, 8);
		string_extract(det_zeros, line, 0, 4);
										
		if (strcmp(type->str, "want") == 0){
			string_extract(oid, line, 9, 49);
						
			/*
			 * Check added refs so we don't make duplicates
			 */
			flg = 0;
			for (i=0;i<http->refs_sz[0];i++){
				if (strcmp(http->refs_w[i]->str, oid->str) == 0){
					flg = 1;
					break;
				}
			}
								
			if (flg == 0){			
				string_extract(caps, line, 50, strlen(line));	// Radi zato sto kad snimimo ne snimimo \0 char
				parser_capabilities(http, caps);
				add_ref_w(http, oid->str);
			}
		} else if (strcmp(type->str, "have") == 0){
			string_extract(oid, line, 9, 49);
			add_ref_h(http, oid->str);
		}	else {
			/*
			 * Detect 0000 break between wants/haves else done
			 */
			if(strcmp(det_zeros->str, "0000") == 0 && strcmp(type->str, "0009") != 0){
				string_extract(oid, line, 13, 53);
				add_ref_h(http, oid->str);
			} else {
				break;
			}
		}
		
		string_free(oid);
		string_free(type);
		string_free(det_zeros);
		string_free(caps);
	}
	
	string_clean(oid);
	string_clean(type);
	string_clean(det_zeros);
	string_clean(caps);
	
	fclose(fp);
	
	if (line)
		free(line);
		
	return 0;
}

uint8_t parser_packhex(g_http_resp* http, char *file, g_str_t* packfile){
	g_str_t* temp = string_init();
	g_str_t* line = string_init();
		
	FILE *fp;
	fp = fopen (file, "rb");
	
	size_t sz = 0;
	fseek(fp, 0L, SEEK_END);
	sz = ftell(fp);
			
	string_allocate(temp, sz);	 		
				
	fseek(fp, 0L, SEEK_SET);
	fread(temp->str, sz, 1, fp);	
	fclose(fp);
	
	int i, copy_bytes=0, starting_address=0, start_pack = 0;
		
	for (i=0;i<sz;i++){
		if (start_pack == 0 && temp->str[i] != '\n' && temp->str[i] != '0'){
				copy_bytes++;
		} else if (start_pack == 0 && temp->str[i] == '\n'){
			//printf("Detected new line:");
			
			string_free(line);
			string_copy_bytes(line, temp, starting_address, copy_bytes);
						
			//string_debug(line);
			parser_pushline(http, line);
			
			starting_address=copy_bytes;
			copy_bytes=0;				
			//print_buff(&line);
			//printf("\n");
			
		} else if (start_pack == 0 && temp->str[i] == '0'){
			if(i+3 < sz && temp->str[i+1] == '0' && temp->str[i+2] == '0' && temp->str[i+3] == '0' && temp->str[i+4] == 'P'){
				i += 3;
				//printf("Detected zeros at %d: ", copy_bytes);
				
				string_free(line);
				string_copy_bytes(line, temp, starting_address, copy_bytes);
				
				//string_debug(line);
								
				parser_pushline(http, line);
				
				starting_address=copy_bytes;
				copy_bytes=0;
				start_pack = 1;
				//print_buff(&line);
				//printf("\n");
			} else {
				copy_bytes++;
			}
		}	else {
			string_append(packfile, "objects/pack/pack-%s.pack", http->push_new_oids[0]->str);
			
			//string_debug(packfile);
			string_free(line);
			string_copy_bytes(line, temp, i, sz-i);
			
			//string_debug(packfile);
			string_save_to_file_binary(line, packfile->str);	// Without terminating char
						
			string_clean(temp);
			string_clean(line);
			
			return 0;
		}
	}
	
	string_clean(temp);
	string_clean(line);
	
	return -1;
}

static uint8_t parser_pushline(g_http_resp* http, g_str_t* line){
	g_str_t* old_oid = string_init();
	g_str_t* new_oid = string_init();
	g_str_t* ref = string_init();
	g_str_t* caps = string_init();
	
	//string_debug(line);
		
	string_copy_bytes(old_oid, line, 4, 40);
	string_copy_bytes(new_oid, line, 45, 40);
	uint16_t c = string_copy_bytes_stop_at_char(ref, line, 86, '\0');
		
	if(88+c < line->size){
		string_copy_bytes(caps, line, 88+c, line->size-88-c-1);	
		parser_capabilities(http, caps);	
	} else {
		string_append(caps,"empty");
	}
		
	add_push(http, old_oid->str, new_oid->str, ref->str, caps->str);
					
	string_clean(old_oid); 
	string_clean(new_oid); 
	string_clean(ref); 
	string_clean(caps); 
		
	return 0;
}
