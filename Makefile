CXX=g++
RM=rm -f
CPPFLAGS=-std=c++11 -Wall -pedantic -Wextra -O3 # -ggdb -gstabs
# CPPFLAGS=-std=c++11 -Wall -pedantic -Wextra -ggdb -gstabs
LDFLAGS=-pthread -gstabs -g -ggdb
LDLIBS=

SRCS=Map.cc micropather/micropather.cc
OBJS=$(subst .cc,.o,$(SRCS))

all: pathfinder_repl

pathfinder_repl: $(OBJS) pathfinder_repl.o
	g++ $(LDFLAGS) -o pathfinder_repl $(OBJS) pathfinder_repl.o $(LDLIBS) 

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) *~ .dependtool

include .depend
