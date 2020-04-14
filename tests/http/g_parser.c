#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "g_buffer.h"
#include "g_string.h"
#include "g_http.h"

void parser_refs(http_resp* http, char *file){
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	
	fp = fopen(file, "r");
	if (fp == NULL)
			exit(EXIT_FAILURE);
	
	g_str_t* det_zeros = string_init();
	g_str_t* type = string_init();
	g_str_t* oid = string_init();
	int i, flg;
	
	while ((read = getline(&line, &len, fp)) != -1) {			
		string_extract(type, line, 4, 8);
		string_extract(det_zeros, line, 0, 4);
										
		if (strcmp(type.str, "want") == 0){
			string_extract(&oid, line, 9, 49);
			
			flg = 0;
			for (i=0;i<http->refs->refs_sz;i++){
				if (strcmp(http->refs->refs[i]->str, oid.str) == 0){
					flg = 1;
					break;
				}
			}
								
			if (flg == 0){			
				add_ref_w(http, oid->str);
			}
		} else if (strcmp(type.str, "have") == 0){
			string_extract(oid, line, 9, 49);
			add_ref_h(http, oid->str);
		}	else {
			if(strcmp(det_zeros.str, "0000") == 0){
				string_extract(&oid, line, 13, 53);
				add_ref_w(http, oid->str);
			}		
		}
		
		string_free(oid);
		string_free(type);
		string_free(det_zeros);
	}
	
	string_clean(oid);
	string_clean(type);
	string_clean(det_zeros);

	fclose(fp);
	
	if (line)
		free(line);
}

void parse_packhex(http_resp* http, char *file, g_str_t* packfile){
	g_str_t* temp = string_init();
	g_str_t* line = string_init();
	g_str_t* lines = string_init();
		
	FILE *fp;
	fp = fopen (file, "rb");
	
	size_t sz = 0;
	fseek(fp, 0L, SEEK_END);
	sz = ftell(fp);
			
	string_allocate(temp, sz);	 		
				
	fseek(fp, 0L, SEEK_SET);
	fread(temp->buff, sz, 1, fp);	
	fclose(fp);
	
	int i, copy_bytes=0, starting_address=0, start_pack = 0;
		
	for (i=0;i<sz;i++){
		if (start_pack == 0 && temp->buff[i] != '\n' && temp->buff[i] != '0'){
				copy_bytes++;
		} else if (start_pack == 0 && temp->buff[i] == '\n'){
			//printf("Detected new line:");
			
			string_free(line);
			string_copy_bytes(line, temp, starting_address, copy_bytes);
			parse_packline(http, line);
			
			starting_address=copy_bytes;
			copy_bytes=0;				
			//print_buff(&line);
			//printf("\n");
			
		} else if (start_pack == 0 && temp->buff[i] == '0'){
			if(i+3 < sz && temp->buff[i+1] == '0' && temp->buff[i+2] == '0' && temp->buff[i+3] == '0'){
				i += 3;
				//printf("Detected zeros at %d: ", copy_bytes);
				
				string_free(line);
				string_copy_bytes(line, temp, starting_address, copy_bytes);
				parse_packline(h, line);
				
				starting_address=copy_bytes;
				copy_bytes=0;
				start_pack = 1;
				//print_buff(&line);
				//printf("\n");
			} else {
				copy_bytes++;
			}
		}	else {
			//printf("AAAAAAAAAA%s", h->push->new_oid->str);
			
			string_append(packfile, "objects/pack/pack-%s.pack", h->push->new_oid->str);
			
			string_free(line);
			string_copy_bytes(line, temp, i, sz-i);
			string_save_to_file(line, packfile->str);
			
			//??? string_init(packfile,  packfile->str);*/
			
			break;
		}
	}
}

/* Nisi radio nikako parser_packline moras zavrsit */

int parser_packline(http_resp* h, g_buff_t* line)
{
	h->push = malloc(sizeof(http_push));
	h->push->capabilities = NULL;
	h->push->caps_sz = 0;

	g_str_t* old_oid;
	g_str_t* new_oid;
	g_str_t* ref;
	g_str_t* packfile;
	g_str_t* temp;
	g_str_t* caps;
	size_t num_caps;
	
	string_init_empty(&old_oid);
	string_init_empty(&new_oid);
	string_init_empty(&ref);
	string_init_empty(&packfile);
	string_init_empty(&temp);
	string_init_empty(&caps);
		
	string_buff2str(old_oid, line, 4, 40);
	string_buff2str(new_oid, line, 45, 40);
	int c = string_buff2str_tillChar(ref, line, 86, '\0');
				
	if(c > 0)
	{
		int i,copy_bytes=0,starting_address=88+c;
		
		for(i=88+c;i<line->size+1;i++)
		{
				if(line->buff[i] != ' ' && i != line->size)
				{
					copy_bytes++;
				}
				else
				{
					string_buff2str(temp, line, starting_address, copy_bytes);
					starting_address=copy_bytes+1;
					copy_bytes=0;
					
					if(h->push->capabilities == NULL){		
						h->push->capabilities = malloc(sizeof(g_str_t));
					} else {
						h->push->capabilities = realloc(h->push->capabilities, h->push->caps_sz * sizeof(g_str_t) + sizeof(g_str_t));
					}
										
					string_init_n(&h->push->capabilities[h->push->caps_sz], temp->str, temp->size);  
					
					h->push->caps_sz++;
					string_free(&temp); 
				}
		}
				
		string_copy(h->push->old_oid, old_oid->str); 
		string_copy(h->push->new_oid, new_oid->str); 
		//string_copy(h->push->ref, ref->str);
				
		/*string_free(old_oid); 
		string_free(new_oid); 
		string_free(ref); */
		
		/*string_kill(old_oid);
		string_kill(new_oid);
		string_kill(ref);*/
			
		return 0;
	} 
	else
	{
		return -1;
	}
}
