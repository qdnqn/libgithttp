#include <stdio.h>
#include <stdlib.h>
#include "../../g_buffer.h"
#include "../../g_string.h"
#include "../../g_http.h"
#include "../../g_parser.h"

int main(){
	g_http_resp* http = response_init();
	
	g_str_t* packfile = string_init();
	string_add(packfile, "/home/wubuntu/ext10/Projects/git-server/git_handler/tests/parser/");		
	parser_packhex(http, "/home/wubuntu/ext10/Projects/git-server/git_handler/tests/parser/push.bin", packfile);
	
	string_clean(packfile);
	
	debug_caps(http);		
	
	/*parser_refs(http, "/home/wubuntu/ext10/Projects/git-server/git_handler/tests/parser/wants.txt");
	
	printf("Detected refs: Want(%ld), Have(%ld), Advertised(%ld)\n", http->refs_sz[0], http->refs_sz[1], http->refs_sz[2]);
	printf("Printing want refs: \n");
	
	int i;
	for(i=0;i<http->refs_sz[0];i++)
		printf("Want: %s\n", http->refs_w[i]->str);
		
	g_str_t* caps = generate_cap_string(http);
	printf("Caps string: %s\n", caps->str);
	
	string_clean(caps);*/
	
	response_clean(http);
	return 0;
}
