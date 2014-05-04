// This file is part of Suncatcher
// Alex Roper <alex@aroper.net>
//
// Suncatcher is free software: you can redistribute it and/or modify it under
// the terms of version 3 of the GNU General Public License as published by the
// Free Software Foundation.
//
// Suncatcher is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// Suncatcher.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2014 Alex Roper

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <stack>
#include <string>
#include <tuple>

#include <gtest/gtest.h>

#include "suncatcher/platform.hh"

#ifdef MICROPATHER_DELTA_TEST
#include "suncatcher/micropather/micropather.hh"
#include "suncatcher/micropather/MicropatherGraph.hh"
#include "suncatcher/test/MPWrapper.hh"
using suncatcher::pathfinder::MicropatherGraph;
#endif

#include "suncatcher/Coord.hh"
#include "suncatcher/test/DeltaMap.hh"
#include "suncatcher/Map.hh"
#include "suncatcher/MapMutator.hh"
#include "suncatcher/util/util.hh"
#include "suncatcher/test/ResourceManager.hh"

namespace suncatcher {
namespace test {

extern const char MAP_OPEN[];
extern const char MAP_MAIN[];
extern const char MAP_DOORLAND[];
extern const char MAP_MICRO[];

using suncatcher::util::manhattan;
using suncatcher::pathfinder::Map;
using suncatcher::test::DeltaMap;
using suncatcher::pathfinder::MapMutator;
using suncatcher::pathfinder::MapBuilder;
using suncatcher::pathfinder::Path;
using suncatcher::pathfinder::Coord;
using suncatcher::pathfinder::PATH_COST_INFINITE;

// Base class for tests that don't modify their map (and thus don't need their
// own copy). Note one thing: the use of union-finds internally marked as
// mutable means that *bitwise* const is not preserved, *logical* const is.  If
// we give this up, we basically lose const for everything since nearly all
// functions need to look up equivalence classes, so it is unfortunate but
// necessary -- I'd rather have const safety and one small lie than give it
// up entirely or mess with const_cast at the API level.
//
// As far as testing goes, the non-optimized maps will regenerate all
// union finds after every mutation, so the delta test should catch any issues
// arising from this.
template <const char* MAP>
class StaticMapTest : public ::testing::Test {
  public:
    StaticMapTest()
    : map(ResourceManager::get_map(MAP)) {
      char* slow = std::getenv("SLOWTEST");
      enable_micropather = (!slow || !strcmp("1", slow));
    }

    #ifdef MICROPATHER_DELTA_TEST
      MPWrapper get_micropather() {
        return ResourceManager::get_micropather(MAP);
      }
    #endif

  protected:
    bool enable_micropather;
    const suncatcher::test::DeltaMap& map;
};

// Base class for tests that modify the map. A new map will be constructed for
// each test.
template <const char* MAP>
class MapTest : public ::testing::Test {
  public:
    MapTest()
    : map(ResourceManager::get_builder(MAP)) {
      char* slow = std::getenv("SLOWTEST");
      enable_micropather = (!slow || !strcmp("1", slow));
    }

    #ifdef MICROPATHER_DELTA_TEST
      MPWrapper get_micropather() {
        return MPWrapper(&map);
      }
    #endif

  protected:
    bool enable_micropather;
    suncatcher::test::DeltaMap map;
};

}  // namespace test
}  // namespace suncatcher
