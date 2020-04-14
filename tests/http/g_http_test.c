#include <stdio.h>
#include <stdlib.h>
#include "../../g_buffer.h"
#include "../../g_string.h"
#include "../../g_http.h"

int main(){
		g_http_resp* http = response_init();
		
		string_char(http->refs, "one");		
		
		printf("Refs: %s\n", http->refs->str);
		
		add_ref_w(http, "test-ref");
		add_ref_w(http, "test2-ref");
		add_ref_w(http, "test3-ref");
		
		printf("Ref: %s\n", http->refs_w[0]->str);
		printf("Ref: %s\n", http->refs_w[1]->str);
		printf("Ref: %s\n", http->refs_w[2]->str);
		
		add_push(http, "old_oid", "new_oid", "ref_name","capability");
		add_push(http, "old_oid", "new_oid", "ref_name2","capability");
		
		printf("My push to ref: %s\n", http->push_refs[1]->str);
				
		response_clean(http);
		return 0;
}
