#ifndef G_AUTH_H
#define G_AUTH_H

#include <inttypes.h>

#include "gh_config.h"
#include "gh_http.h"

#define REDIS_EMPTY					1
#define REDIS_INVALID_TYPE			2
#define REDIS_ERROR					3
#define REDIS_NOT_FOUND				4
#define REDIS_FOUND					5

#define ALLOWED_USER				10
#define ALLOWED_GUEST				11
#define FORBIDDEN					12

uint8_t authenticate(g_http_resp* http);
void git_log(g_http_resp* http, char* data);

#endif
