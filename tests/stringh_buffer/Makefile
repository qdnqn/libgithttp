## MakeFile learning and observations:
## if you use @? instead of @^ then some object files will be ignored? -> Research more
##

EXE = string_buffer_test.out
SOURCES = g_string_buffer_test.c gh_string.c gh_buffer.c gh_log.c
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
UNAME_S := $(shell uname -s)

CXX = gcc
CXXFLAGS1 = -fPIC -Wall -Wformat
CXXFLAGS2 = 
CXXFLAGS2LIB = -shared
LIBS = -I. -I../../ -L../libgit2/build/ -lgit2

%.o: %.c
	$(CXX) $(CXXFLAGS1) $(LIBS) -c $< -o $@

$(EXE): $(OBJS)
	#$(CXX) -o libgitrefs.so $^ $(CXXFLAGS2LIB) $(LIBS)
	$(CXX) -o $@ $^ $(CXXFLAGS2) $(LIBS)
	
clean:
	rm -f $(EXE), $(OBJS), libgitrefs.so
