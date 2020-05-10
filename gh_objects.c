#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "gh_config.h"

#if defined(GH_USEBROOKER)
#include "hiredis.h"
#endif

#include "git2.h"
#include "git2/odb_backend.h"

#include "gh_init.h"
#include "gh_string.h"
#include "gh_http.h"
#include "gh_parser.h"
#include "gh_vectors.h"
#include "gh_broker.h"
#include "gh_objects.h"
#include "gh_auth.h"

static int index_cb(const git_indexer_progress *stats, void *data);
static int foreach_cb(const git_oid *oid, void *data);
static int indexed_objects = 0;
static commit_vector commits;

uint8_t git_create_packfile_from_repo(g_str_t* pack, g_str_t* message, git_repository* repo, const char* oid, g_str_t* path){
	int error = 0;
	
	g_str_t* path_to_pack = string_init();
	g_str_t* temp = string_init();
	
	string_add(path_to_pack, path->str);
	string_add(path_to_pack, "objects/pack/");
		
	git_packbuilder* pbuilder = NULL;
	git_revwalk* walker = NULL;
				
	//char hash[GIT_OID_HEXSZ+1]; hash[GIT_OID_HEXSZ] = '\0';
	
	commit_vector commits = {NULL, 0};
	
	git_commit* wcommit;
	git_oid g_oid;
	git_oid_fromstr(&g_oid, oid);
	
	if(access(path_to_pack->str, W_OK) != 0){
		error = -1;
		goto clean;
	} 

	error = git_commit_lookup(&wcommit, repo, &g_oid);
	git_commit_free(wcommit);
			
	if(error < 0){
		goto clean;
	}
	
	error = git_packbuilder_new(&pbuilder, repo);
	
	if(error < 0)
		goto clean;
	 	
	git_revwalk_new(&walker, repo);
	git_revwalk_sorting(walker, GIT_SORT_TIME);
	git_revwalk_push(walker, &g_oid);
	
	while ((git_revwalk_next(&g_oid, walker)) == 0){
    error = git_commit_lookup(&wcommit, repo, &g_oid);
    
    if (error == 0){
			git_commit_insert(&commits, g_oid);
			git_packbuilder_insert(pbuilder, &g_oid, NULL);
		}
		
		git_commit_free(wcommit);
  }
  
  git_object *obj;
  size_t i;

  for (i=0;i<commits.size;i++){
		git_object_lookup(&obj, repo, commits.oid[i], GIT_OBJ_COMMIT);
		git_packbuilder_insert_tree(pbuilder, git_commit_tree_id((git_commit *)obj));
		git_object_free(obj);
	}  
	
	//git_packbuilder_write(pbuilder, path_to_pack->str, 0, NULL, NULL);
  
  git_buf buf = {NULL, 0, 0};
  git_packbuilder_write_buf(&buf, pbuilder);
   
  temp->str = buf.ptr;
  temp->size = buf.size;
  
  string_copy_bytes(pack, temp, 0, temp->size);
 
  git_buf_free(&buf);
	string_destroy_container(temp);
clean:
	
	if(commits.size > 0){		
		for(i=0;i<commits.size;i++){
			free(commits.oid[i]);
		}
		
		free(commits.oid);
	}
	
	if(pbuilder != NULL)
		git_packbuilder_free(pbuilder);
	
	if(walker != NULL)
		git_revwalk_free(walker);
	
	string_clean(path_to_pack);
	
	return error;
}

void get_packfile(g_http_resp* http, git_repository* repo, g_str_t* path_repo, char* request_file){
	parser_refs(http, request_file);
	
	int i, error, failed = 0;
	g_str_t* path_to_newpack = string_init();

	for (i=0;i<http->refs_sz[0];i++){
		error = git_create_packfile_from_repo(http->pack, http->message, repo, http->refs_w[i]->str, path_repo);
		
		if (error == 0){
		} else if (error == -3) {
			failed = 1;
			printf("Commit not found in this repo: %s\n", http->refs_w[i]->str);
		} else {
			failed = 1;
			printf("Something went south (Debug libgit2).\n");
		}
	}
	
	#if defined(GH_USEBROOKER)
		if(error == 0){
			gh_broker* broker = broker_init(http);
			
			broker_channel(broker, "%s.repo", http->repo->str);
			
			broker_message(broker, "{\"message\": \"%s\", "
														 "\"user\": \"%s\", "
														 "\"repo\": \"%s\", "
														 "\"path_repo\": \"%s\", "
														 "\"type\": \"%s\", "
														 "\"ref_oid\": \"%s\", "
														 "\"old_oid\": \"\", "
														 "\"new_oid\": \"\", "
														 "\"indexed_obj\": \"%d\"}",
														 "Pulling from repository.", 
															http->username->str, 
															http->repo->str, 
															path_repo->str,
															"push", 
															http->refs_w[i]->str);						 
				 
			broker->reply = redisCommand(broker->redis, "PUBLISH %s %s",	broker->channel->str, broker->message->str);											 
					 
			broker_reply_clean(broker);	
			broker_clean(broker);
		}
	#endif
	
	string_clean(path_to_newpack);
}

int8_t git_commit_packfile(g_str_t* packfile, g_str_t* packdir, g_str_t* new_head, git_repository* repo, g_str_t* idx_hash){			
	string_append(packdir, "objects/pack/");
	
	git_odb *odb = NULL;
	git_repository_odb(&odb, repo);								// Needed for fixing thin-pack
	
	git_indexer *idx;
	git_indexer_progress stats = {0, 0};
	int error = 0;
	char hash[GIT_OID_HEXSZ + 1] = {0};
	int fd;
	ssize_t read_bytes;
	char buf[512];

	if (git_indexer_new(&idx, packdir->str, 0, odb, NULL) < 0) {
		puts("bad idx");
		return -1;
	}

	if ((fd = open(packfile->str, 0)) < 0) {
		perror("open");
		return -1;
	}

	do {
		read_bytes = read(fd, buf, sizeof(buf));
		if (read_bytes <= 0)
			break;

		if ((error = git_indexer_append(idx, buf, read_bytes, &stats)) < 0)
			goto cleanup;

		index_cb(&stats, NULL);
	} while (read_bytes > 0);

	if (read_bytes < 0) {
		error = -1;
		perror("failed reading");
		goto cleanup;
	}

	if ((error = git_indexer_commit(idx, &stats)) < 0){
		printf("ERROR: %d: %s\n", error, git_error_last()->message);
		goto cleanup;
	}

	printf("\rIndexed %u of %u\n", stats.indexed_objects, stats.total_objects);
	
	indexed_objects = stats.received_objects;
	
	git_oid_fmt(hash, git_indexer_hash(idx));
	puts(hash);
	
	string_append(idx_hash, hash);
	
	git_odb_free(odb);
	
cleanup:
	close(fd);
	git_indexer_free(idx);
		
	string_add(new_head, hash);
	remove(packfile->str);				// DELETE PACKFILE?
			
	return error;
}

void save_packfile(g_http_resp* http, git_repository* repo, g_str_t* path, char* request_file){
	g_str_t* hex_packfile = string_init();	
	g_str_t* packdir = string_init();
	g_str_t* new_head = string_init();
	g_str_t* idx = string_init();
				
	string_add(hex_packfile, path->str);
	string_add(packdir, path->str);
		
	parser_packhex(http, request_file, hex_packfile);
	int error = git_commit_packfile(hex_packfile, packdir, new_head, repo, idx);
			
	if(error == 0){
		verify_pack(http, packdir, idx);
		g_str_t* path_head = string_init();
		
		string_add(path_head, path->str);
		string_append(path_head, http->push_refs[0]->str);
				
		string_save_to_file(http->push_new_oids[0], path_head->str);
		
		string_debug(http->push_new_oids[0]);
		string_debug(path_head);
		
		//string_append(http->message,"\1");		
		string_append_hexsign(http->message,"unpack ok\n");
		string_append_hexsign(http->message,"ok %s\n", http->push_refs[0]->str);
		string_append(http->message, "00000000");
		//string_hexsign_exclude_sign(http->message);
		
		string_clean(path_head);
		
		#if defined(GH_USEBROOKER)
			gh_broker* broker = broker_init(http);
			
			broker_channel(broker, "%s.repo", http->repo->str);		
			
			broker_message(broker, "{\"message\": \"%s\", "
														 "\"user\": \"%s\", "
														 "\"repo\": \"%s\", "
														 "\"path_repo\": \"%s\", "
														 "\"type\": \"%s\", "
														 "\"ref_oid\": \"%s\", "
														 "\"old_oid\": \"%s\", "
														 "\"new_oid\": \"%s\", "
														 "\"indexed_obj\": \"%d\"}",
														 "Pushing to repository.", 
															http->username->str,
															http->repo->str, 
															path->str, 
															"push", 
															http->push_refs[0]->str, 
															http->push_old_oids[0]->str, 
															http->push_new_oids[0]->str, 
															indexed_objects);						 
					 
			broker->reply = redisCommand(broker->redis, "PUBLISH %s %s",	broker->channel->str, broker->message->str);
															
			broker_reply_clean(broker);				
			broker_clean(broker);
		#endif
	}
	
	string_clean(hex_packfile);
	string_clean(packdir);
	string_clean(new_head);
	string_clean(idx);
}

void verify_pack(g_http_resp* http, g_str_t* packdir, g_str_t* idx){
	g_str_t* path = string_init();
	
	string_append(path, packdir->str);
	string_append(path, "pack-%s.idx", idx->str);
	
	git_repository *repo;
	git_odb_backend *backend = NULL;
	git_odb *odb = NULL;
	git_odb_new(&odb);
	
	git_odb_backend_one_pack(&backend, path->str);
	git_odb_add_backend(odb, backend, 1);
	git_repository_wrap_odb(&repo, odb);
	git_odb_foreach(odb, foreach_cb, repo);
	
	char hash[GIT_OID_HEXSZ+1];
	
	#if defined(GH_USEBROOKER)
			gh_broker* broker = broker_init(http);
			broker_channel(broker, "%s.repo", http->repo->str);
	#endif
	
	for(int i=0; i<commits.size; i++){
		git_oid_tostr(hash, GIT_OID_HEXSZ+1, commits.oid[i]);
				
		#if defined(GH_USEBROOKER)	
			broker_message(broker, "{\"type\": \"%s\", \"parent_oid\": \"%s\", \"child_oid\": \"%s\"}",
															"child-commit", 
															http->push_new_oids[0]->str, 
															hash);
															
			broker->reply = redisCommand(broker->redis, "PUBLISH %s %s", broker->channel->str, broker->message->str);
			
			broker_reply_clean(broker);													
		#endif
	}
		
	#if defined(GH_USEBROOKER)
		broker_clean(broker);
	#endif
				
	git_commit_vector_clean(&commits);	
			
	git_odb_free(odb);
	git_repository_free(repo);
		
	string_clean(path);
}

static int foreach_cb(const git_oid *oid, void *data){
	git_repository* repoodb = data;

	git_commit * commit = NULL;
	int error = git_commit_lookup(&commit, repoodb, oid);
      
  if(error == 0){	
		git_commit_insert(&commits, *oid);
	}
	
	git_commit_free(commit);

	return 0;
}


static int index_cb(const git_indexer_progress *stats, void *data){
	(void)data;
	printf("Processing %d of %d\n", stats->indexed_objects, stats->total_objects);

	return 0;
}
