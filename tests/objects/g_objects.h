#ifndef GIT_OBJECTS_H
#define GIT_OBJECTS_H

#include "g_string.h"
#include "g_http.h"

/*int git_packbuilder_oid(g_str_t* h, g_str_t* m, git_repository* r, const char* oid, g_str_t* path);
void save_packfile(g_buff_t* pack, g_str_t* path, g_str_t* hex);
void handle_save_packfile(http_resp* h, git_repository* repo, g_str_t* path, char* request_file);*/

uint8_t git_create_packfile_from_repo(g_str_t* pack, g_str_t* message, git_repository* repo, const char* oid, g_str_t* path);
void get_packfile(g_http_resp* http, git_repository* repo, g_str_t* path, char* request_file);

int8_t git_commit_packfile(g_str_t* packfile, g_str_t* packdir, g_str_t* new_head, git_repository* repo);
void save_packfile(g_http_resp* http, git_repository* repo, g_str_t* path, char* request_file);

#endif
