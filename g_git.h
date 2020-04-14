#ifndef G_GIT_H
#define G_GIT_H

#include <stdlib.h>
#include "git2.h"

#define GIT_REPO_INITIALIZED  	1
#define GIT_REPO_FAILED			0

/* */

#define GIT_REQ_GET_REFS			100
#define GIT_REQ_UPLOAD_PACK			101
#define GIT_REQ_SET_REFS			102
#define GIT_REQ_RECEIVE_PACK		103

/* */ 

#define commit_vector_default {{NULL, 0}}

int	git_request;

typedef struct GIT_Vector {
	git_commit** comm;
	git_oid* oid;
	size_t size;
} commit_vector;

void git_commit_insert(commit_vector* vec, git_commit* commit, git_oid oid);

#endif
