#ifndef G_PARSER
#define G_PARSER

#include "gh_config.h"

uint8_t parser_refs(g_http_resp* http, char *file);
uint8_t parser_capabilities(g_http_resp* http, char *line);
uint8_t parser_packhex(g_http_resp* http, char *file, g_str_t* packfile);

#endif
