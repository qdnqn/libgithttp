#include "g_git.h"
#include "git2.h"

void git_commit_insert(commit_vector* vec, git_commit* commit, git_oid oid)
{
	if(vec->size == 0){
		vec->comm = malloc(sizeof(git_commit*));
		vec->oid = malloc(sizeof(git_oid));
		
		vec->comm[0] = commit;
		*vec->oid = oid;
		
		vec->size++;
	} else {
		vec->comm = realloc(vec->comm, vec->size * sizeof(git_commit*) + sizeof(git_commit*));
		vec->oid = realloc(vec->oid, vec->size * sizeof(git_oid) + sizeof(git_oid));
		
		vec->comm[vec->size] = commit;
		vec->oid[vec->size] = oid;
		
		vec->size++;
	}
}
