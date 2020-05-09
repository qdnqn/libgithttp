#ifndef GH_INIT
#define GH_INIT

#define GIT_REPO_INITIALIZED  		1
#define GIT_REPO_FAILED						0

#define GIT_REQ_GET_REFS				100
#define GIT_REQ_UPLOAD_PACK			101
#define GIT_REQ_SET_REFS				102
#define GIT_REQ_RECEIVE_PACK		103

#include "gh_config.h"

int	git_request;

uint8_t git_init(git_repository **repo, char *path);
void git_deinit(git_repository* repo);

#endif
