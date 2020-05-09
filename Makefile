EXE = libgithttp.out
SOURCES = gh_refs.c gh_auth.c gh_init.c gh_objects.c gh_broker.h gh_vectors.h gh_parser.c gh_http.c gh_buffer.c gh_log.c gh_string.c
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
UNAME_S := $(shell uname -s)

CXX = gcc
CXXFLAGS1 = -g -fPIC -Wall -Wformat
CXXFLAGS2 = 
CXXFLAGS2LIB = -shared
LIBS = -I. -I../nginx/src/core -I../redis/ -L../libgit2/build/ -lgit2 -L../redis/ -lhiredis

%.o: %.c
	$(CXX) $(CXXFLAGS1) $(LIBS) -c $< -o $@

$(EXE): $(OBJS)
	$(CXX) -o libgithttp.so $^ $(CXXFLAGS2LIB) $(LIBS)
	$(CXX) -o $@ $^ $(CXXFLAGS2) $(LIBS)
	
clean:
	rm -f $(EXE), $(OBJS), libgithttp.so
