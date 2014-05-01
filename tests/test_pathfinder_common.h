#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <stack>
#include <string>
#include <tuple>

#include <gtest/gtest.h>

#include "platform.h"

#ifdef MICROPATHER_DELTA_TEST
#include "micropather/micropather.h"
#include "micropather/MicropatherGraph.h"
#include "MPWrapper.h"
using suncatcher::pathfinder::MicropatherGraph;
#endif

#include "Coord.h"
#include "DeltaMap.h"
#include "Map.h"
#include "MapMutator.h"
#include "util/util.h"
#include "ResourceManager.h"

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
    : map(ResourceManager::get_map(MAP)) { }

    #ifdef MICROPATHER_DELTA_TEST
      MPWrapper get_micropather() {
        return ResourceManager::get_micropather(MAP);
      }
    #endif

  protected:
    const suncatcher::test::DeltaMap& map;
};

// Base class for tests that modify the map. A new map will be constructed for
// each test.
template <const char* MAP>
class MapTest : public ::testing::Test {
  public:
    MapTest()
    : map(ResourceManager::get_builder(MAP)) { }

    #ifdef MICROPATHER_DELTA_TEST
      MPWrapper get_micropather() {
        return MPWrapper(&map);
      }
    #endif

  protected:
    suncatcher::test::DeltaMap map;
};

}  // namespace test
}  // namespace suncatcher
