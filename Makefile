CXX=g++
RM=rm -f

COMMON_FLAGS=-I. -std=c++11 -Wall -pedantic -isystem lib/gtest/include -Wextra
DEBUG_FLAGS=-g -ggdb -gstabs -Og
PROFILE_FLAGS=-O3 -DNDEBUG

# CPPFLAGS=$(COMMON_FLAGS) $(PROFILE_FLAGS)
CPPFLAGS=$(COMMON_FLAGS) $(DEBUG_FLAGS)

LDFLAGS=-pthread -gstabs -g -ggdb
LDLIBS=

SRCS=Map.cc micropather/micropather.cc MapMutator.cc
TEST_SRCS=tests/test_main.cc tests/test_pathfinder.cc
OBJS=$(subst .cc,.o,$(SRCS))
TEST_OBJS=$(subst .cc,.o,$(TEST_SRCS))

all: pathfinder_repl

pathfinder_repl: $(OBJS) pathfinder_repl.o
	g++ $(LDFLAGS) $(DEBUG_FLAGS) -o pathfinder_repl $(OBJS) pathfinder_repl.o $(LDLIBS)

test: $(OBJS) $(TEST_OBJS)
	g++ $(LDFLAGS) $(DEBUG_FLAGS) -o test_main $(TEST_OBJS) $(OBJS) lib/gtest/lib/.libs/libgtest.a $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(TEST_OBJS) $(OBJS) pathfinder_repl.o pathfinder_repl test_main tests/test_main.o

dist-clean: clean
	$(RM) *~ .dependtool

include .depend
