#ifndef G_HTTP_H
#define G_HTTP_H

#include <stdlib.h>
#include <stdarg.h>

#include "gh_config.h"

#if defined(GH_USEBROOKER)
#include "hiredis.h"
#endif

#include "gh_buffer.h"
#include "gh_string.h"

#define REF_WANT	0
#define REF_HAVE	1
#define REF_ADVERT	2

typedef struct HTTP_response {
	g_str_t* refs;
	g_str_t* output;
	g_str_t* pack;
	g_str_t* message;
	g_str_t* request_file;
	
	g_str_t* username;
	g_str_t* repo;
	
	g_str_t** refs_w;
	g_str_t** refs_h;
	g_str_t** refs_a;
	
	size_t refs_sz[3];
	
	g_str_t** push_old_oids;
	g_str_t** push_new_oids;
	g_str_t** push_refs;
	g_str_t** push_capabilities;
	
	size_t push_sz[4];
	
	#if defined(GH_USEBROOKER)
		redisContext *redis;	
	#endif
	
	uint8_t allowed;
	uint8_t auth;
	
	/*
	 * !Tracking data only for one active ref of type want or have or advertise refs or push.
	 */
	 
	uint8_t multi_ack_detailed;
	uint8_t no_done;
	uint8_t side_band_64k;
	uint8_t thin_pack; 
	uint8_t ofs_delta;
	uint8_t report_status;
	g_str_t* agent;
	g_str_t* symref;
} g_http_resp;

g_http_resp* response_init(g_str_t*, g_str_t*, uint8_t);

uint8_t add_ref_w(g_http_resp* http, char* id);
uint8_t add_ref_h(g_http_resp* http, char* id);
uint8_t add_ref_a(g_http_resp* http, char* id);
uint8_t add_push(g_http_resp* http, char* old_oidc, char* new_oidc, char* refc, char* capabilitiesc);
void add_cap(g_http_resp* http, char* cap, char* value);
void generate_cap_string(g_str_t*, g_http_resp*);

void response_clean(g_http_resp*);

#endif
