#include <git2.h>

#include "gh_config.h"

#include "gh_vectors.h"

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

void git_commit_vector_clean(commit_vector* vec){
	for(int i=0; i<vec->size; i++){
		free(vec->oid[i]);
	}
	
	free(vec->oid);
	vec->size = 0;
}
