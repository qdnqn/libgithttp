#ifndef GH_BROKER
#define GH_BROKER

#include <inttypes.h>
#include <stdarg.h>

#include "gh_config.h"

#if defined(GH_USEBROOKER)
#include "hiredis.h"
#endif

#include "gh_http.h"

typedef struct GH_BROKER {
	g_str_t *channel;
	g_str_t *message;
	
	#if defined (GH_USEBROOKER)
		redisContext *redis;
		redisReply *reply;
	#endif
} gh_broker;

gh_broker* broker_init(g_http_resp*);
void broker_clean(gh_broker* broker);

void broker_channel(gh_broker* broker, char* channel, ...);
void broker_message(gh_broker* broker, char* message, ...);
void broker_reply_clean(gh_broker* broker);


#endif
