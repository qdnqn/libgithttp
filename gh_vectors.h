#ifndef GH_VECTORS
#define GH_VECTORS

#include "gh_config.h"

#include <git2.h>

typedef struct GIT_Vector {
	git_oid** oid;
	size_t size;
} commit_vector;

void git_commit_insert(commit_vector* vec, git_oid oid);
void git_commit_vector_clean(commit_vector* vec);

#endif
