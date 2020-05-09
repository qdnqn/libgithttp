# http-libgit2
C library providing git http transfer processing using libgit2. This library provides no http interface to world. Instead you can feed it with http request data retrieved from other source in text/binary file form and process it to get desired output. 

Library can process http request containing git push request and process thin pack file accordingly and git pull/clone request. Currently only option made available is  report-status for git smart requests. 

It can process http request headers and body from git client, handle negotiate process, and generate appropriate response so you can send it back to git client. Although you will need medium for http transfer. There's an example of creating custom nginx module [nginx-git-module](https://github.com/qdnqn/nginx-git-module) to handle HTTP transfer. You can use any other http server you prefer for handling http transfer of data.  
## Building library
```
git clone https://github.com/qdnqn/http-libgit2
cd http-libgit2
make
```
#### Linking
```
gcc myprogram.c -Ipath/to/libgit_header_files -Lpath/to/ligbgit.so_dir -llibgithttp
```
#### Dependencies

libgithttp library is using libgit2 (https://libgit2.org/) and hiredis (https://github.com/redis/hiredis) (optional) for brokering between libgit2 and git controller (https://github.com/qdnqn/libgithttp-controller).

If you don't want brokering service you need to remove USEBROKER from gh_config.h and compile.

You can reference yourself to Makefile in repo for compiling instructions. You need to take in consideration that Makefile in repo uses brokering service if you wish to compile libgithttp with it.

#### Pull example
You can find example code for custom ngxin module (https://github.com/qdnqn/nginx-git-module). 

Here is basic example of code needed for git pull of specific commit.
```c
#include "git2.h"           // include libgit2.h file(libgit2 library)
#include "gh_init.h"
#include "gh_string.h"
#include "gh_http.h"
#include "gh_parser.h"
#include "gh_objects.h"
#include "gh_auth.h"
#include "gh_refs.h"

int main(){
    g_str_t* path = string_init();
    g_str_t* username = string_init();

    string_add(path, "absolute path to your repo directory");
    string_add(username, "guest");

    g_http_resp* http = NULL;
    g_http = response_init(username, path, NULL, 1);

    git_repository* repo;
    
    if(git_init(&repo, path->str) == GIT_REPO_FAILED) {
      return 0;
    }
    
    get_packfile(http, repo, path, "request.txt");
    string_debug(http->pack);

    string_clean(path);
    string_clean(username);
    
    return 0;
}
```
Where requests.txt file contains
```
0032want bdd6ceb415a8f5cfbecf0ed64310309c7880e1b8
00000009done
```
Response in http->pack will contain packfile generated from current HEAD in your repository:
```
0008NAK
PACK
.... CONTENT HERE
```
#### Push example
```c
#include "git2.h"           // include libgit2.h file(libgit2 library)
#include "gh_init.h"
#include "gh_string.h"
#include "gh_http.h"
#include "gh_parser.h"
#include "gh_objects.h"
#include "gh_auth.h"
#include "gh_refs.h"

int main(){
    g_str_t* path = string_init();
    g_str_t* username = string_init();

    string_add(path, "absolute path to your repo directory");
    string_add(username, "guest");

    g_http_resp* http = NULL;
    g_http = response_init(username, path, NULL, 1);

    git_repository* repo;
    
    if(git_init(&repo, path->str) == GIT_REPO_FAILED) {
      return 0;
    }
    
    save_packfile(http, repo, path, "request.txt");
    string_debug(http->pack);

    string_clean(path);
    string_clean(username);
    
    return 0;
}
```
Assumingly you already captured git push request in reqest.txt code above will process it and apply to your git repository.