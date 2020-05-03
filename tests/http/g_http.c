#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "g_buffer.h"
#include "g_string.h"
#include "g_http.h"

#include "hiredis.h"

static void string_free_array(g_str_t** array, size_t* size);

g_http_resp* response_init(g_str_t* username, g_str_t* repo, redisContext* context, uint8_t auth){
	g_http_resp* temp = malloc(sizeof(g_http_resp));
	
	temp->refs = string_init();
	temp->output = string_init();
	temp->pack = string_init();
	temp->message =  string_init();
	temp->request_file = string_init();
	temp->username = string_init();
	temp->repo = string_init();
	
	temp->redis = context;
	temp->auth = auth;
	temp->allowed = 0;
	
	string_append(temp->username, username->str);
	string_append(temp->repo, repo->str);
	
	temp->refs_w = malloc(sizeof(g_str_t*));	
	temp->refs_h = malloc(sizeof(g_str_t*));
	temp->refs_a = malloc(sizeof(g_str_t*));
	
	temp->refs_sz[0] = 0;
	temp->refs_sz[1] = 0;
	temp->refs_sz[2] = 0;
	
	temp->push_old_oids = malloc(sizeof(g_str_t*));
	temp->push_new_oids = malloc(sizeof(g_str_t*));
	temp->push_refs = malloc(sizeof(g_str_t*));
	temp->push_capabilities = malloc(sizeof(g_str_t*));
	
	temp->push_sz[0] = 0;
	temp->push_sz[1] = 0;
	temp->push_sz[2] = 0;
	temp->push_sz[3] = 0;
	
	temp->multi_ack_detailed = 0;
	temp->no_done = 0;
	temp->side_band_64k = 0;
	temp->thin_pack = 0;
	temp->ofs_delta = 0;
	temp->agent = string_init();
	temp->symref = string_init();
	temp->report_status = 0;
				
	return temp; 
}

void response_clean(g_http_resp* http){
	if(http != NULL){
		string_clean(http->refs);
		string_clean(http->output);
		string_clean(http->pack);
		string_clean(http->message);
		string_clean(http->request_file);
		string_clean(http->username);
		string_clean(http->repo);
		
		string_free_array(http->refs_w, &http->refs_sz[0]);
		string_free_array(http->refs_h, &http->refs_sz[1]);
		string_free_array(http->refs_a, &http->refs_sz[2]);
		
		string_free_array(http->push_old_oids, &http->push_sz[0]);
		string_free_array(http->push_new_oids, &http->push_sz[1]);
		string_free_array(http->push_refs, &http->push_sz[2]);
		string_free_array(http->push_capabilities, &http->push_sz[3]);
				
		string_clean(http->agent);
		string_clean(http->symref);
		
		redisFree(http->redis);
		
		free(http);
	}
}

static void string_free_array(g_str_t** array, size_t* size){
	if((*size) != 0){
		int i=0;
		
		for(i=0;i<(*size);i++){
			string_clean(array[i]);
		}
		
		*size = 0;
	}
	
	free(array);
}

uint8_t add_ref_w(g_http_resp* http, char* id){
		g_str_t* temp = string_init();
		string_char(temp, id);
		
		if(http->refs_sz[0] == 0){
			http->refs_w[0] = temp; 
		} else {
			http->refs_w = realloc(http->refs_w, (http->refs_sz[0]+1) * sizeof(g_str_t*));
			http->refs_w[http->refs_sz[0]] = temp; 
		}
		
		http->refs_sz[0]++;
		
		return 0;
}

uint8_t add_ref_h(g_http_resp* http, char* id){
		g_str_t* temp = string_init();
		string_char(temp, id);
		
		if(http->refs_sz[1] == 0){
			http->refs_h[0] = temp; 
		} else {
			http->refs_h = realloc(http->refs_h, (http->refs_sz[1]+1) * sizeof(g_str_t*));
			http->refs_h[http->refs_sz[1]] = temp; 
		}
		
		http->refs_sz[1]++;
		
		return 0;
}

uint8_t add_push(g_http_resp* http, char* old_oidc, char* new_oidc, char* refc, char* capabilitiesc){
		g_str_t* old_oid = string_init();
		g_str_t* new_oid = string_init();
		g_str_t* ref = string_init();
		g_str_t* capabilities = string_init();
		
		string_char(old_oid, old_oidc);
		string_char(new_oid, new_oidc);
		string_char(ref, refc);
		string_char(capabilities, capabilitiesc);
				
		if(http->push_sz[0] == 0){
			http->push_old_oids[0] = old_oid;
			http->push_new_oids[0] = new_oid;
			http->push_refs[0] = ref;
			http->push_capabilities[0] = capabilities; 
		} else {
			http->push_old_oids = realloc(http->push_old_oids, (http->push_sz[0]+1) * sizeof(g_str_t*));
			http->push_new_oids = realloc(http->push_new_oids, (http->push_sz[0]+1) * sizeof(g_str_t*));
			http->push_refs = realloc(http->push_refs, (http->push_sz[0]+1) * sizeof(g_str_t*));
			http->push_capabilities = realloc(http->push_capabilities, (http->push_sz[0]+1) * sizeof(g_str_t*));
			
			http->push_old_oids[http->push_sz[0]] = old_oid;
			http->push_new_oids[http->push_sz[0]] = new_oid;
			http->push_refs[http->push_sz[0]] = ref;
			http->push_capabilities[http->push_sz[0]] = capabilities; 
		}
		
		http->push_sz[0]++;
		http->push_sz[1]++;
		http->push_sz[2]++;
		http->push_sz[3]++;
		
		return 0;
}

void add_cap(g_http_resp* http, char* cap, char* value){
	if(strcmp(cap, "multi_ack_detailed") == 0){
		http->multi_ack_detailed = 1;
	} else if(strcmp(cap, "no-done") == 0){
		http->no_done = 1;
	} else if(strcmp(cap, "side-band-64k") == 0){
		http->side_band_64k = 1;
	} else if(strcmp(cap, "thin-pack") == 0){
		http->thin_pack = 1;
	} else if(strcmp(cap, "ofs-delta") == 0){
		http->ofs_delta = 1;
	} else if(strcmp(cap, "agent") == 0){
		string_free(http->agent);
		string_add(http->agent, value);
	} else if(strcmp(cap, "report-status") == 0){
		http->report_status = 1;
	} else if(strcmp(cap, "symref") == 0){
		string_free(http->symref);
		string_add(http->symref, value);
	} else {
		printf("Unknown capability: %s\n", cap);
	}
}

void generate_cap_string(g_str_t* temp, g_http_resp* http){
	int first = 0;
		
	if(http->multi_ack_detailed == 1 && first == 0){
		string_add(temp, "multi_ack_detailed");
		first = 1;
	} else if (http->multi_ack_detailed == 1 && first == 1) {
		string_add(temp, " multi_ack_detailed");
	} else {}
	
	if(http->side_band_64k == 1 && first == 0){
		string_add(temp, "side-band-64k");
		first = 1;
	} else if (http->side_band_64k == 1 && first == 1) {
		string_add(temp, " side-band-64k");
	} else {}
		
	if(http->thin_pack == 1 && first == 0){
		string_add(temp, "thin-pack");
		first = 1;
	} else if (http->thin_pack == 1 && first == 1) {
		string_add(temp, " thin-pack");
	} else {}
	
	if(http->ofs_delta == 1 && first == 0){
		string_add(temp, "ofs-delta");
		first = 1;
	} else if (http->ofs_delta == 1 && first == 1) {
		string_add(temp, " ofs-delta");
	} else {}
	
	if(http->agent->size != 0 && first == 0){
		string_add(temp, "agent=");
		string_add(temp, http->agent->str);
		first = 1;
	} else if (http->agent->size != 0  && first == 1) {
		string_add(temp, " agent=");
		string_add(temp, http->agent->str);
	} else {}
	
	if(http->report_status == 1 && first == 0){
		string_add(temp, "report-status");
		first = 1;
	} else if (http->report_status == 1 && first == 1) {
		string_add(temp, " report-status");
	} else {}
	
	if(http->symref->size != 0 && first == 0){
		string_add(temp, "symref=");
		string_add(temp, http->symref->str);
		first = 1;
	} else if (http->symref->size != 0  && first == 1) {
		string_add(temp, " symref=");
		string_add(temp, http->symref->str);
	} else {}
}

void debug_caps(g_http_resp* http){
	printf("===========================================\n");
	printf("Debuggin capabilities: \n");
	
	if(http->multi_ack_detailed == 1){
		printf("multi_ack_detailed: yes\n");
	} else {
		printf("multi_ack_detailed: no\n");
	}
	
	if(http->side_band_64k == 1){
		printf("side-band-64k: yes\n");
	} else {
		printf("side-band-64k: no\n");
	}
	
	if(http->thin_pack == 1){
		printf("thin-pack: yes\n");
	} else {
		printf("thin-pack: no\n");
	}
	
	if(http->ofs_delta == 1){
		printf("ofs-delta: yes\n");
	} else {
		printf("ofs-delta: no\n");
	}
	
	if(http->agent->size != 0){
		printf("agent: %s\n", http->agent->str);
	} else {
		printf("agent: unknown\n");
	}
	
	if(http->symref->size != 0){
		printf("symref: %s\n", http->symref->str);
	} else {
		printf("symref: unknown\n");
	}
	
	if(http->report_status == 1){
		printf("report-status: yes\n");
	} else {
		printf("report-status: no\n");
	}
}
