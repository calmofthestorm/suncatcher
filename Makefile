CXX=g++
RM=rm -f

MAKEFLAGS=-j 8
COMMON_FLAGS=-I. -std=c++11 -Wall -pedantic -isystem lib/gtest/include -Wextra -Wunreachable-code -Wformat-nonliteral -Wcast-align -Wpointer-arith -Wmissing-declarations -Wundef -Wcast-qual -Wshadow -Wwrite-strings -Wno-unused-parameter -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused
DEBUG_FLAGS=-g -ggdb -gstabs -O0 -fno-inline-functions
PROFILE_FLAGS=-O3 -DNDEBUG

# CPPFLAGS=$(COMMON_FLAGS) $(PROFILE_FLAGS)
CPPFLAGS=$(COMMON_FLAGS) $(DEBUG_FLAGS)

LDFLAGS=-pthread -gstabs -g -ggdb
LDLIBS=

SRCS=Map.cc micropather/micropather.cc MapMutator.cc MapBuilder.cc
TEST_SRCS=tests/DeltaMap.cc tests/ResourceManager.cc tests/MPWrapper.cc tests/test_main.cc tests/test_pathfinder_static.cc tests/test_pathfinder_common.cc tests/test_pathfinder_mutation.cc
OBJS=$(subst .cc,.o,$(SRCS))
TEST_OBJS=$(subst .cc,.o,$(TEST_SRCS))

all: pathfinder_repl

door_torture: $(OBJS) door_torture.o
	g++ $(LDFLAGS) $(DEBUG_FLAGS) -o door_torture $(OBJS) door_torture.o $(LDLIBS)

pathfinder_repl: $(OBJS) pathfinder_repl.o
	g++ $(LDFLAGS) $(DEBUG_FLAGS) -o pathfinder_repl $(OBJS) pathfinder_repl.o $(LDLIBS)

test: $(OBJS) $(TEST_OBJS)
	g++ $(LDFLAGS) $(DEBUG_FLAGS) -o test_main $(TEST_OBJS) $(OBJS) lib/gtest/lib/.libs/libgtest.a $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(TEST_OBJS) $(OBJS) pathfinder_repl.o pathfinder_repl test_main tests/test_main.o door_torture.o

dist-clean: clean
	$(RM) *~ .dependtool

include .depend
