#ifndef GIT_REFS_H
#define GIT_REFS_H

#include "gh_config.h"

int show_ref(git_reference *ref, void *data);
void git_get_refs(g_http_resp* http, git_repository* r, g_str_t*, g_str_t* path);
void git_set_refs(git_repository* r, g_str_t*);
uint8_t isHead(g_str_t* path, char* ref_name, char* hex);

#endif
