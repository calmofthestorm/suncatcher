CXX=g++
RM=rm -f
CPPFLAGS=-std=c++11 -Wall -pedantic -isystem lib/gtest/include -Wextra -O3 -DNDEBUG # -ggdb -gstabsa
# CPPFLAGS=-std=c++11 -Wall -pedantic -isystem lib/gtest/include -Wextra -ggdb -gstabs
LDFLAGS=-pthread -gstabs -g -ggdb
LDLIBS=

SRCS=Map.cc micropather/micropather.cc
OBJS=$(subst .cc,.o,$(SRCS))

all: pathfinder_repl

pathfinder_repl: $(OBJS) pathfinder_repl.o
	g++ $(LDFLAGS) -o pathfinder_repl $(OBJS) pathfinder_repl.o $(LDLIBS)

test: $(OBJS) tests/test_main.o
	g++ $(LDFLAGS) -o test_main $(OBJS) tests/test_main.o lib/gtest/lib/.libs/libgtest.a $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS) pathfinder_repl.o pathfinder_repl test_main tests/test_main.o

dist-clean: clean
	$(RM) *~ .dependtool

include .depend
