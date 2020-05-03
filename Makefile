EXE = libgithttp.out
SOURCES = refs.c g_auth.c git_init.c g_objects.c g_parser.c g_http.c g_buffer.c g_string.c
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
