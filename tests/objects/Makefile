## MakeFile learning and observations:
## if you use @? instead of @^ then some object files will be ignored? -> Research more
##

EXE = objects_test.out
SOURCES = g_objects_test.c g_objects.c git_init.c g_parser.c g_http.c g_string.c g_buffer.c
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
UNAME_S := $(shell uname -s)

CXX = gcc
CXXFLAGS1 = -fPIC -Wall -Wformat
CXXFLAGS2 = -g
CXXFLAGS2LIB = -shared
LIBS = -I. -I../../ -L../libgit2/build/ -lgit2

%.o: %.c
	$(CXX) $(CXXFLAGS1) -c $< -o $@

$(EXE): $(OBJS)
	#$(CXX) -o libgitrefs.so $^ $(CXXFLAGS2LIB) $(LIBS)
	$(CXX) -o $@ $^ $(CXXFLAGS2) $(LIBS)
	
clean:
	rm -f $(EXE), $(OBJS), libgitrefs.so
