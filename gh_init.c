#include <git2.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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

void checkout(git_repository* repo, char* location, char* oid){
	printf("Making dir: %s\n", location);
	
	struct stat st = {0};

	if (stat(location, &st) == -1) {
			mkdir(location, 0777);
	}
	
	git_repository_set_workdir(repo, location, 0);
				
	git_commit *commit;
	git_revparse_single((git_object **)&commit, repo, oid);

	git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
	opts.checkout_strategy = GIT_CHECKOUT_FORCE;

	git_checkout_tree(repo, (git_object *)commit, &opts);

	git_commit_free(commit);

	git_repository_set_workdir(repo, location, 0);

	git_index_matched_path_cb matched_cb = NULL;
	git_index *index;
	git_strarray array = {0};
	
	int options = 0, count = 0;
	struct print_payload payload;

	git_repository_index(&index, repo);
	git_index_add_all(index, &array, 0, matched_cb, &payload);
	git_index_write(index);
	git_index_free(index);
}
