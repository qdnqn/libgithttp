#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "gh_config.h"

#if defined(GH_USEBROOKER)
#include "hiredis.h"
#endif

#include "gh_string.h"
#include "gh_broker.h"
#include "gh_http.h"

gh_broker* broker_init(g_http_resp* http){
	gh_broker* broker_temp = malloc(sizeof(gh_broker));
	
	broker_temp->channel = string_init();
	broker_temp->message = string_init();
		
	broker_temp->redis = http->redis;
	
	return broker_temp;
}

void broker_clean(gh_broker* broker){
	string_clean(broker->channel);
	string_clean(broker->message);
	
	free(broker);
}

void broker_reply_clean(gh_broker* broker){
	freeReplyObject(broker->reply);
}

void broker_channel(gh_broker* broker, char* channel, ...){
	va_list args1, args2;
	va_start(args1, channel);
	va_copy(args2, args1);

	ssize_t len = vsnprintf(NULL, 0, channel, args1);								// Only number of chars without terminating char '\0'
	va_end(args1);
	
	string_free(broker->channel);
		
	if(broker->channel->size == 0){
		string_allocate(broker->channel, len+1);
		vsnprintf(broker->channel->str + broker->channel->size - len - 1, len+1, channel, args2);
	} else {
		string_reallocate(broker->channel, len);
		vsnprintf(broker->channel->str + broker->channel->size - len - 1, len+1, channel, args2);
	}
	
	va_end(args2);
}

void broker_message(gh_broker* broker, char* message, ...){
	va_list args1, args2;
	va_start(args1, message);
	va_copy(args2, args1);

	ssize_t len = vsnprintf(NULL, 0, message, args1);								// Only number of chars without terminating char '\0'
	va_end(args1);
	
	string_free(broker->message);
		
	if(broker->message->size == 0){
		string_allocate(broker->message, len+1);
		vsnprintf(broker->message->str + broker->message->size - len - 1, len+1, message, args2);
	} else {
		string_reallocate(broker->message, len);
		vsnprintf(broker->message->str + broker->message->size - len - 1, len+1, message, args2);
	}
	
	va_end(args2);
}
