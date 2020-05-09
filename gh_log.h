#ifndef G_LOG
#define G_LOG

#define G_ERROR 1
#define G_DEBUG 2
#define G_INFO 3
#define G_FILE 4

#include <inttypes.h>

#include "gh_config.h"

void gh_log(uint8_t type, char* message, ...);

#endif
