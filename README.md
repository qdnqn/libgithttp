# http-libgit2
Library providing git http transfer using libgit2. This library provides no http interface to world. 
Instead you can feed it with http request data retrieved from other source in text/binary file form and process it to get desired output. 

It can process http request headers and body from git client, handle negotiate process, and generate appropriate response so you can send it back to git client. Although you will need medium for http transfer. There's example of creating custom nginx module [nginx-git-module](https://github.com/qdnqn/nginx-git-module) to handle HTTP transfer. You can use any other http server you prefer for handling http transfer of data.  
## Building library
## git fetch
## git push
