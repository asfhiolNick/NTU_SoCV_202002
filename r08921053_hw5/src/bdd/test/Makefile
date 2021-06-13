CSRCS     = $(wildcard *.cpp) $(wildcard *.c)
CHDRS     = $(wildcard *.h)
COBJS     = $(addsuffix .o, $(basename $(CSRCS)))

CXX       = g++_new
CXX       = g++
CFLAGS    = -O3 -Wall
CFLAGS    = -g -Wall
EXTINCDIR = ../../../include
LIBDIR    = ../../../lib

EXEC      = testBdd

$(EXEC): $(COBJS)
	@echo "> building: $@"
	@$(CXX) -o $@ $(CFLAGS) $(COBJS) -L$(LIBDIR) -lbdd

%.o : %.cpp
	@echo "> compiling: $<"
	@$(CXX) $(CFLAGS) -I$(EXTINCDIR) -c -o $@ $<

clean:
	@rm -f $(COBJS) $(EXEC) i.*
