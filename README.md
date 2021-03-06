
# libgithttp
C library providing git http transfer processing using libgit2. This library provides no http interface to world. Instead you can feed it with http request data retrieved from other source in text/binary file form and process it to get desired output. 

Library can process http request containing git push request and process thin pack file accordingly and git pull/clone request. Currently only option made available is  report-status for git smart requests. 

It can process http request headers and body from git client, handle negotiate process, and generate appropriate response so you can send it back to git client. Although you will need medium for http transfer. There's an example of creating custom nginx module [nginx-git-module](https://github.com/qdnqn/nginx-git-module) to handle HTTP transfer. You can use any other http server you prefer for handling http transfer of data.  
## Use case
You can find working code for custom ngxin module (https://github.com/qdnqn/nginx-git-module) using this library.

## Building library
```
# Assuming you have libgit2 compiled in directory ../libgit2/build and
# hiredis in directory ../redis/ (If you don't want brokering(See brokering section) just delete from LIBS in makefile)
# otherwise just update LIBS = -I. -I../nginx/src/core -I../redis/ -L../libgit2/build/ -lgit2 -L../redis/ -lhiredis
# to configuration you have on your machine.

git clone https://github.com/qdnqn/libgithttp
cd libgithttp
make
```
#### Linking
```
gcc myprogram.c -Ipath/to/libgithttp_header_files -Lpath/to/ligbgithttp.so_dir -llibgithttp -llibgit2
```
#### Dependencies

libgithttp library is using libgit2 (https://libgit2.org/) and hiredis (https://github.com/redis/hiredis) (optional) for brokering between libgit2 and git controller (https://github.com/qdnqn/libgithttp-controller).

If you don't want brokering service you need to remove USEBROKER from gh_config.h and compile.

You can reference yourself to Makefile in repo for compiling instructions. You need to take in consideration that Makefile in repo uses brokering service if you wish to compile libgithttp with it.

#### Brokering
What's the thing about brokering? Well library can publish data on redis when some event occurs. By event I mean push or pull. So if you need to trigger some action after these events occur you can easily do that. Sibling of this library is [libgithttp-controller](https://github.com/qdnqn/libgithttp-controller). Refer to that link for more information about brokering.



When brokering is used log is saved to mongodb.




If you don't wish brokering enabled you need to remove constant USEBROKER grom gh_config.h.
### Authentication
Authentication is not possible. It is possible to transfer authentication on some other agent if you want to implement it. But when authentication is enabled library uses redis for confirming that specific user have rights to do actions on specific repository in this format:
```redis
HGETALL username:repo_name 
```
If element is found in a format like above actions will be allowed. Also you need to implement your logic for allowing or disabling actions based on user level.


Example is given here (Snippet taken from [nginx-git-module](https://github.com/qdnqn/nginx-git-module)): 
```
g_str_t* username = string_init();
redisContext *c = NULL;

if(git_config->auth == 1){
    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    c = redisConnectWithTimeout((char*)git_redis_host.data, git_config->redis_port, timeout);
    
    string_copy_char_nullterminate(username, (char*)r->headers_in.user.data, r->headers_in.user.len);
    
    g_http = response_init(username, repo_c, c, 1); // User logging level
    r->http_r = g_http;
} else {
    string_append(username,"guest");
    
    g_http = response_init(username, repo_c, c, 0);  // Guest logging level
    r->http_r = g_http;
}

allowed = authenticate(g_http);

if(allowed != ALLOWED_USER && allowed != ALLOWED_GUEST){    
    return NGX_HTTP_FORBIDDEN;
}
```
#### Pull example
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
    g_http = response_init(username, path, NULL, 0);        // 0 means no auth, no logging enabled.

    git_repository* repo;
    
    if(git_init(&repo, path->str) == GIT_REPO_FAILED) {
      return 0;
    }
    
    get_packfile(http, repo, path, "request.txt");
    
    string_append_hexsign(http->output, "NAK\n");
    string_concate(http->output, http->pack);

    /* 
     * Be aware that http->output have '\0' at the end of string so you need to omit it before sending back
     * to git client over http (It may break your git request.).
     */

    string_debug(http->output); // Will output string content to stdio.

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
Response in http->pack will contain packfile generated from current HEAD in your repository and http->output:
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
    g_http = response_init(username, path, NULL, 0);

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
Assumingly you already captured git push request in reqest.txt, code above will process it and apply to your git repository.
Be aware that git push request sent over http is in binary format so you should save it as binary in requests.txt file.
## Under the hood
Most crucial functions can be found in gh_refs.c and gh_objects.c.

File gh_refs.c contains functions for processing request with header content-types:
1. <small>```application/x-git-upload-pack-advertisement -> git_get_refs(g_http_resp*, git_repository*, g_str_t*(g_http->refs), g_str_t*(path_to_repo));```</small>
2. <small> ```application/x-git-receive-pack-advertisement -> git_set_refs(git_repository*, g_str_t*(g_http->refs));```</small>



File gh_objects.c contains functions for processing request with header content-types:
1. <small> ```application/x-git-upload-pack-result -> get_packfile(g_http_resp*, git_repository*, g_str_t*(path_to_repo), g_str_t*(path_to_request_file));```</small>
2. <small> ```application/x-git-receive-pack-result -> save_packfile(g_http_resp*, git_repository*, g_str_t*(path_to_repo), g_str_t*(path_to_request_file));```<small>

If you are interested how is implemented packfile processing (and having trouble finding information on the internet as I had) you can refer yourself to gh_objects.c and section Useful links also!.



There you can find functions for:
1. Generating packfile for git repository
2. Applying pack file to git repository(Fixing thin pack)
3. Getting commits from packfile(Function -> verify_pack)

## Useful links
1. https://libgit2.org/libgit2/#HEAD
2. https://git-scm.com/docs/http-protocol
3. https://mincong.io/2018/05/04/git-and-http/
4. https://git-scm.com/book/en/v2/Git-Internals-Packfiles
5. https://github.com/libgit2/libgit2/blob/2376cd26226771dcf8ef5dfd04c83a50c50bf3d4/tests/pack/indexer.c
6. https://github.com/git/git/blob/9bfa0f9be3e718f701200a242ea04259a4dc4dfc/Documentation/technical/protocol-v2.txt
7. https://codewords.recurse.com/issues/three/unpacking-git-packfiles
8. https://repo.or.cz/w/git.git?a=blob;f=Documentation/technical/pack-format.txt;h=1803e64e465fa4f8f0fe520fc0fd95d0c9def5bd;hb=HEAD
9. https://github.com/git/git/blob/master/Documentation/technical/pack-heuristics.txt (IRC chat between linus and njs)

## Future plans & Motivation
Project is new born as side library doing my homework project, for continous integration, so I'm planning to work on it more in future.
## License
Copyright 2020 adnn.selimovic@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

