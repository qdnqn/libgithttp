#include <git2.h>

#include "git_init.h"

uint8_t git_init(git_repository** repo, char* path)
{
	git_libgit2_init();

	int error = git_repository_open(repo, path);

	if(error == 0){
		return GIT_REPO_INITIALIZED;
	} else {
		return GIT_REPO_FAILED;
	}
}

void git_deinit(git_repository* repo)
{
	git_repository_free(repo);
	git_libgit2_shutdown();
}

void git_commit_insert(commit_vector* vec, git_oid oid){
	if (vec->size == 0){
		vec->oid = malloc(sizeof(git_oid*));
		
		git_oid* temp_oid = malloc(sizeof(git_oid));
		
		*temp_oid = oid;		
		
		vec->oid[0] = temp_oid;
		
		vec->size++;
	} else {
		vec->oid = realloc(vec->oid, vec->size * sizeof(git_oid*) + sizeof(git_oid*));
		
		git_oid* temp_oid = malloc(sizeof(git_oid));
		
		*temp_oid = oid;		
				
		vec->oid[vec->size] = temp_oid;
		
		vec->size++;
	}
}

