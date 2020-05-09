#ifndef GIT_OBJECTS_H
#define GIT_OBJECTS_H

#include "gh_config.h"

#include "gh_string.h"
#include "gh_http.h"

uint8_t git_create_packfile_from_repo(g_str_t* pack, g_str_t* message, git_repository* repo, const char* oid, g_str_t* path);
void get_packfile(g_http_resp* http, git_repository* repo, g_str_t* path, char* request_file);

int8_t git_commit_packfile(g_str_t* packfile, g_str_t* packdir, g_str_t* new_head, git_repository* repo, g_str_t* idx_hash);
void save_packfile(g_http_resp* http, git_repository* repo, g_str_t* path, char* request_file);

void verify_pack(g_http_resp* http, g_str_t* packdir, g_str_t* idx);

#endif
