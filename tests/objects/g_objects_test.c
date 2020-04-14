#include <stdio.h>
#include <stdlib.h>
#include "git2.h"
#include "../../git_init.h"
#include "../../g_buffer.h"
#include "../../g_string.h"
#include "../../g_http.h"
#include "../../g_parser.h"
#include "../../g_objects.h"

int main(){
	g_http_resp* http = response_init();
	g_str_t* path = string_init();
	
	git_repository* repo = NULL;
		
	string_char(path, "/home/wubuntu/ext10/Projects/git-server/test.repo/");
	
	if(git_init(&repo, path->str) == GIT_REPO_INITIALIZED){						
		save_packfile(http, repo, path, "/home/wubuntu/ext10/Projects/git-server/git_handler/tests/objects/push.bin");
		//get_packfile(http, repo, path, "/home/wubuntu/ext10/Projects/git-server/git_handler/tests/objects/wants.txt");
	} else {
		
	}
	
	string_clean(path);
	response_clean(http);
	git_deinit(repo);
	
	
	return 0;
}
