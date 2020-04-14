#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "git2.h"
#include "git_init.h"
#include "g_objects.h"
#include "g_string.h"
#include "g_http.h"

static git_indexer* indexer;
static void *payload;

static int feed_indexer(void *ptr, size_t len, void *payload);
static int prog_callbck(const git_transfer_progress *sts, void *pyld);
static int index_cb(const git_indexer_progress *stats, void *data);

int git_packbuilder_oid(g_str_t* hx, g_str_t* message, git_repository* r, const char* oid, g_str_t* path){
	int error = 0;
	
	g_str_t* path_t = string_init();
	
	string_add(path_t, path->str);
	string_add(path_t, "objects/pack/");	// Repo path + pack dir -> save packfile there
		
	if(access(path_t->str, W_OK) != 0){
		return -1;
	} 
			 
	git_packbuilder* pbuilder;
	
	git_transfer_progress stats;
	git_transfer_progress_cb stats_cb;
	git_oid hash;
	
	git_indexer_options opts = GIT_INDEXER_OPTIONS_INIT;
	
	opts.progress_cb = prog_callbck;
		
	char hex[GIT_OID_HEXSZ+1]; hex[GIT_OID_HEXSZ] = '\0';
	
	error = git_packbuilder_new(&pbuilder, r);
	 
	commit_vector commits = {NULL, 0};
	
	git_commit* wcommit;
	git_oid g_oid;
	git_oid_fromstr(&g_oid, oid);
	
	git_revwalk* walker = NULL;
 
	git_revwalk_new(&walker, r);
	git_revwalk_sorting(walker, GIT_SORT_TIME);
	git_revwalk_push(walker, &g_oid);
	
	while ((git_revwalk_next(&g_oid, walker)) == 0){
    error = git_commit_lookup(&wcommit, r, &g_oid);

    if (error == 0){
			git_commit_insert(&commits, wcommit, g_oid);
			git_packbuilder_insert(pbuilder, &commits.oid[commits.size-1], NULL);
		}
  }
  
  git_object *obj;
  size_t i;

  for (i=0;i<commits.size;i++){
		git_object_lookup(&obj, r, &commits.oid[i], GIT_OBJ_COMMIT);
		git_packbuilder_insert_tree(pbuilder, git_commit_tree_id((git_commit *)obj));
		
		git_object_free(obj);
	}
			
	git_indexer_new(&indexer, path_t->str, 0, NULL, &opts);	
	git_packbuilder_foreach(pbuilder, feed_indexer, &stats);
	git_indexer_commit(indexer, &stats);
	git_oid_fmt(hex, git_indexer_hash(indexer));
	 
	/*if(message->size == 0){
		http_buff_git_line(message, "Counting objects: %d, done.\n", stats.total_objects);
		http_buff_git_line(message, "Total %d (delta 0), reused 0 (delta 0)\n", stats.total_objects);
	}*/
	
	//
	 //	LEFT FOR LATER REFERENCE
	 // 
	 //	typedef struct git_transfer_progress {
	 //		unsigned int total_objects;
	 //		unsigned int indexed_objects;
	 //		unsigned int received_objects;
	 //		unsigned int local_objects;
	 //		unsigned int total_deltas;
	 //		unsigned int indexed_deltas;
	 //		size_t received_bytes;
	 //	} git_transfer_progress;
   //
   //
		    
  free(commits.oid);
  
  for(i=0;i<commits.size;i++){
		git_commit_free(commits.comm[i]);
	}
	
	git_indexer_free(indexer);
	git_packbuilder_free(pbuilder);
	git_revwalk_free(walker);
	
	string_free(path_t);
	
	string_allocate(hx, 41);																										// Added terminating character!
	strcpy(hx->str, hex);
	
	return 0;
}

static int prog_callbck(const git_transfer_progress *sts, void *pyld){
	// DO SOMETHING IN CALLBACK?
	// return less than 0 to cancel indexer process
		
	return 1;
}

void load_packfile(g_str_t* pack, g_str_t* path, g_str_t* hex){
	g_str_t* path_t = string_init();
	string_add(path_t, path->str);
	
	string_append(path_t, "objects/pack/pack-%s.pack", hex->str);		// Repo path + pack dir -> save packfile there
	
	string_load_from_file_binary(pack, path_t->str);
}

void handle_load_packfile(g_http_resp* http, git_repository* repo, g_str_t* path, char* request_file)
{
	parser_refs(http, request_file);
	
	int i;
	g_str_t* hex_packfile = string_init();	

	for (i=0;i<http->refs_sz[0];i++){
		if (git_packbuilder_oid(hex_packfile, http->message, repo, http->refs_w[i]->str, path) == 0){
			load_packfile(http->pack, path, hex_packfile);
		}
	}
	
	string_clean(hex_packfile);
}

static int feed_indexer(void *ptr, size_t len, void *payload)
{
	git_transfer_progress *stats = (git_transfer_progress *)payload;
	
	return git_indexer_append(indexer, ptr, len, stats);
}

static int index_cb(const git_indexer_progress *stats, void *data){
	(void)data;
	printf("\rProcessing %u of %u", stats->indexed_objects, stats->total_objects);

	return 0;
}

uint8_t git_packreader_file(g_str_t* packfile, g_str_t* packdir, g_str_t* new_head, git_repository* repo){			
	string_append(packdir,"objects/pack/");
	
	git_indexer *idx = NULL;
	git_indexer_progress stats = {0, 0};
	
	int error = 0;
	char hash[GIT_OID_HEXSZ + 1]; hash[GIT_OID_HEXSZ + 1] = '\0';
	int fd = 0;
	ssize_t read_bytes = 0;
	char buf[512];

	if (git_indexer_new(&idx, packdir->str, 0, NULL, NULL) < 0) {
		printf("bad idx \n");
		return -1;
	}

	if ((fd = open(packfile->str, 0)) < 0) {
		printf("file open error");
		return -1;
	}

	do {
		read_bytes = read(fd, buf, sizeof(buf));
		if (read_bytes < 0)
			break;

		if ((error = git_indexer_append(idx, buf, read_bytes, &stats)) < 0)
			printf("\rNot commited to indexer");

		index_cb(&stats, NULL);
	} while (read_bytes > 0);

	if (read_bytes < 0) {
		error = -1;
		perror("failed reading");
	}
	
	close(fd);

	if ((error = git_indexer_commit(idx, &stats)) < 0)
		printf("\rNot commited");

	printf("\rIndexing %u of %u\n", stats.indexed_objects, stats.total_objects);

	git_oid_fmt(hash, git_indexer_hash(idx));
	
	git_indexer_free(idx);
	
	string_add(new_head, hash);
	remove(packfile->str);
			
	return 0;
}

void handle_save_packfile(g_http_resp* http, git_repository* repo, g_str_t* path, char* request_file){
	g_str_t* hex_packfile = string_init();	
	g_str_t* packdir = string_init();
	g_str_t* new_head = string_init();
				
	string_add(hex_packfile, path->str);
	string_add(packdir, path->str);
		
	parser_packhex(http, request_file, hex_packfile);
			
	if(git_packreader_file(hex_packfile, packdir, new_head, repo) == 0){
		/*g_str_t* path_head = string_init();
		
		string_add(path_head, path->str);
		string_append(path_head, http->push_refs[0]->str);
				
		//string_save_to_file(http->push->new_oid, path_head->str);
		
		string_debug(path_head);
		
		//string_debug()
		
		//http_buff_c(http->message, '\1');
		//http_buff_git_line(http->message,"unpack ok\n");
		//http_buff_git_line(http->message,"ok %s\n", http->push->ref->str);
		//http_buff(http->message, "00000000");*/
	}
	
	string_clean(hex_packfile);
	string_clean(packdir);
	string_clean(new_head);
}

