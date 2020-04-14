#ifndef GIT_INIT_H
#define GIT_INIT_H

#define GIT_REPO_INITIALIZED  	1
#define GIT_REPO_FAILED			0

#define GIT_REQ_GET_REFS			100
#define GIT_REQ_UPLOAD_PACK			101
#define GIT_REQ_SET_REFS			102
#define GIT_REQ_RECEIVE_PACK		103

int	git_request;

typedef struct GIT_Vector {
	git_oid** oid;
	size_t size;
} commit_vector;

void git_commit_insert(commit_vector* vec, git_oid oid);

uint8_t git_init(git_repository **repo, char *path);
void git_deinit(git_repository* repo);

#endif
