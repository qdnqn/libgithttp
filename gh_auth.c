#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include "gh_config.h"

#if defined (GH_USEBROOKER)
#include "hiredis.h"
#endif

#include "gh_string.h"
#include "gh_auth.h"
#include "gh_http.h"
#include "gh_broker.h"

uint8_t authenticate(g_http_resp* http){
	#if defined (GH_USEBROOKER)
		gh_broker* broker = broker_init(http);
		uint32_t j;
				
		broker->reply = redisCommand(broker->redis, "HGETALL user:%s", http->username->str);
		http->allowed = FORBIDDEN;
					
		if ( broker->reply->type == REDIS_REPLY_ERROR ) {
			return REDIS_ERROR;
		} else if ( broker->reply->type == REDIS_REPLY_NIL ) {
			return REDIS_EMPTY;
		} else if ( broker->reply->type != REDIS_REPLY_ARRAY ) {
			return REDIS_INVALID_TYPE;	
		} else {
				for (j = 0; j < broker->reply->elements; j+=2) {
					if(strcmp(broker->reply->element[j]->str, http->repo->str) == 0){
						if(strcmp(broker->reply->element[j+1]->str, "1") == 0){
							broker_reply_clean(broker);
							broker_clean(broker);
							
							http->allowed = ALLOWED_USER;
							return http->allowed;
						}
					}
				}
		}
		
		broker_reply_clean(broker);
		broker_clean(broker);
				
		if(http->auth == 1){
			http->allowed = FORBIDDEN;
			return FORBIDDEN;
		} else {
			http->allowed = ALLOWED_GUEST;
			return ALLOWED_GUEST;
		}
	#endif
	
	#if !defined(GH_USEBROOKER)
		if(http->auth == 1){
			http->allowed = FORBIDDEN;
			return FORBIDDEN;
		} else {
			http->allowed = ALLOWED_GUEST;
			return ALLOWED_GUEST;
		}
	#endif
}
