#include <stdio.h>
#include <string.h>
#include "git2.h"

#include "git_init.h"
#include "g_string.h"
#include "g_http.h"
#include "g_parser.h"
#include "g_objects.h"
#include "g_auth.h"
#include "refs.h"
#include "hiredis.h"

void git_get_refs(g_http_resp* http, git_repository* repo, g_str_t* grefs, g_str_t* path){
	git_reference_iterator *refs = NULL;
	git_reference *ref = NULL;
	git_reference *resolved = NULL;
	
	char hex[GIT_OID_HEXSZ+1];
	const git_oid *oid;
	git_object *obj;
	
	int ref_size = 0;
	
	git_reference_iterator_new(&refs, repo);
	
	while(git_reference_next(&ref, refs) == 0){
		git_reference_free(ref);
		ref_size++;
	}
	
	git_reference_iterator_free(refs);
	
	git_reference_iterator_new(&refs, repo);
					
	string_append_hexsign(grefs, "# service=git-upload-pack\n");
	string_add(grefs, "0000"); 																							// Dont need new line here! (*Git protocol)
	 
	g_str_t* temp = string_init(); 
	 
	while(git_reference_next(&ref, refs) == 0){
		if (git_reference_type(ref) == GIT_REF_SYMBOLIC)
			git_reference_resolve(&resolved, ref);
	
		oid = git_reference_target(resolved ? resolved : ref);
		git_oid_fmt(hex, oid);
		hex[GIT_OID_HEXSZ] = 0;
		git_object_lookup(&obj, repo, oid, GIT_OBJ_ANY);
		
		char *dummy = (char*) git_reference_name(ref); // Prevent gcc warning
		
		if(isHead(path, dummy, hex)){
			string_free(temp);
			string_add(temp, "HEAD:");
			string_add(temp, dummy);
			
			add_cap(http, "symref", temp->str);
			
			string_free(temp);
			generate_cap_string(temp, http);
									
			string_append_hexsign(grefs, "%s %s%c%s\n", hex, "HEAD", '\0', temp->str);
			string_append_hexsign(grefs, "%s %s\n", hex, dummy);
			string_clean(temp);
		} else {
			string_append_hexsign(grefs, "%s %s\n", hex, git_reference_name(ref));
		}
				
		git_reference_free(ref);
		
		if (resolved)
			git_reference_free(resolved);
			
		git_object_free(obj);
	}
	
	string_add(grefs, "0000");
	git_reference_iterator_free(refs);
}

uint8_t isHead(g_str_t* path, char* ref_name, char* hex){
	g_str_t* hex_fromfile = string_init();
	g_str_t* temp = string_init();
	string_concate(temp, path);
	string_append(temp, ref_name);
	
	string_load_from_file_bytes(hex_fromfile, temp->str, 40);
				
	if(strcmp(hex_fromfile->str, hex) == 0){
		string_clean(temp);
		string_clean(hex_fromfile);
				
		return 1;
	} else {
		return 0;
	}	
}

void git_set_refs(git_repository* r, g_str_t* grefs)
{
	git_reference_iterator *refs = NULL;
	git_reference *ref = NULL;
	git_reference *resolved = NULL;
	
	char hex[GIT_OID_HEXSZ+1];
	const git_oid *oid;
	git_object *obj;
	
	int ref_size = 0;
	
	git_reference_iterator_new(&refs, r);
	
	while(git_reference_next(&ref, refs) == 0)
	{
		git_reference_free(ref);
		ref_size++;
	}
	
	git_reference_iterator_free(refs);
	git_reference_iterator_new(&refs, r);
					
	string_append_hexsign(grefs, "# service=git-receive-pack\n");
	string_add(grefs, "0000"); 																							// Dont need new line here! (*Git protocol)
	
	if(!git_repository_is_empty(r)){
		while(git_reference_next(&ref, refs) == 0)
		{
			if (git_reference_type(ref) == GIT_REF_SYMBOLIC)
			git_reference_resolve(&resolved, ref);
		
			oid = git_reference_target(resolved ? resolved : ref);
			git_oid_fmt(hex, oid);
			hex[GIT_OID_HEXSZ] = 0;
			git_object_lookup(&obj, r, oid, GIT_OBJ_ANY);
			
			string_append_hexsign(grefs, "%s %s%creport-status delete-refs\n", hex, git_reference_name(ref), '\0');
											
			git_reference_free(ref);
			
			if (resolved)
				git_reference_free(resolved);
		}
	} else {
		string_append_hexsign(grefs, "%s %s%creport-status delete-refs\n", "0000000000000000000000000000000000000000", "capabilities^{}", '\0');
	}
	
	string_append(grefs, "0000");
	
	git_reference_iterator_free(refs);
}

int main(){
	g_str_t* username = string_init();
	g_str_t* repox = string_init();
	
	string_add(username, "test");
	string_add(repox, "test");
	struct timeval timeout = { 1, 500000 }; // 1.5 seconds
	redisContext *c = redisConnectWithTimeout("localhost", 6379, timeout);
	
	g_http_resp* http = response_init(username, repox, c, 1);
	g_str_t* path = string_init();
	
	uint8_t d = authenticate(http);
	
	printf("Authenticate: %d\n", d);
			
	string_char(path, "/home/wubuntu/ext10/Projects/git-server/testing_repos/test.repo/");
	
	git_repository *repo = NULL;
	
	if(git_init(&repo, path->str) == GIT_REPO_INITIALIZED){						
		//save_packfile(http, repo, path, "/home/wubuntu/test.txt");
		
		//git_get_refs(http, repo, http->refs, path);
		//git_set_refs(repo, http->refs);
		
		//printf("%s\n", http->refs->str);
		//string_debug(http->refs);
		
		/*unsigned char* x = (unsigned char*)http->refs->str;
		int i;
		for(i=0; i<http->refs->size-1;i++){
			printf("%c", x[i]);
		}		
		
		printf("\n\n\n");*/
	} else {
		
	}
	
	string_clean(path);
	response_clean(http);
	git_deinit(repo);
	
	return 0;
}

