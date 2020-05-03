#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "g_string.h"
#include "g_auth.h"
#include "g_http.h"
#include "hiredis.h"

uint8_t authenticate(g_http_resp* http){
	uint32_t j;
	http->reply = redisCommand(http->redis,"HGETALL user:%s", http->username->str);
	http->allowed = FORBIDDEN;
		
	if ( http->reply->type == REDIS_REPLY_ERROR ) {
		return REDIS_ERROR;
	} else if ( http->reply->type == REDIS_REPLY_NIL ) {
		return REDIS_EMPTY;
	} else if ( http->reply->type != REDIS_REPLY_ARRAY ) {
		return REDIS_INVALID_TYPE;	
	} else {
			for (j = 0; j < http->reply->elements; j+=2) {
				if(strcmp(http->reply->element[j]->str, http->repo->str) == 0){
					if(strcmp(http->reply->element[j+1]->str, "1") == 0){
						freeReplyObject(http->reply);
						http->allowed = ALLOWED_USER;
						return ALLOWED_USER;
					}
				}
			}
	}
	
	freeReplyObject(http->reply);
	
	if(http->auth == 1){
		http->allowed = FORBIDDEN;
		return FORBIDDEN;
	} else {
		http->allowed = ALLOWED_GUEST;
		return ALLOWED_GUEST;
	}
}

void git_log(g_http_resp* http, char* data){
	http->reply = redisCommand(http->redis,"LPUSH %s:%s %s", http->username->str, http->repo->str, data);
	freeReplyObject(http->reply);
}
