#include <git2.h>

#include "gh_config.h"

#include "gh_init.h"

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
