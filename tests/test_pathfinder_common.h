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

// TODO: figure out why MP segfaults only under gtest in path-dependent ways:/
// #define MICROPATHER_DELTA_TEST

#ifdef MICROPATHER_DELTA_TEST
#include "micropather/micropather.h"
#include "micropather/MicropatherGraph.h"
using suncatcher::pathfinder::MicropatherGraph;
#endif

#include "Coord.h"
#include "DeltaMap.h"
#include "Map.h"
#include "MapMutator.h"
#include "util.h"

using suncatcher::util::manhattan;
using suncatcher::pathfinder::Map;
using suncatcher::test::DeltaMap;
using suncatcher::pathfinder::MapMutator;
using suncatcher::pathfinder::MapBuilder;
using suncatcher::pathfinder::Path;
using suncatcher::pathfinder::Coord;
using suncatcher::pathfinder::PATH_COST_INFINITE;

namespace test {
  extern const char MAP_OPEN[];
  extern const char MAP_MAIN[];
  extern const char MAP_DOORLAND[];
  extern const char MAP_MICRO[];
}  // namespace test

#ifdef MICROPATHER_DELTA_TEST
class MPWrapper {
  public:
    MPWrapper(const DeltaMap* map) {
      my_map = map;
      mpg.reset(new MicropatherGraph(map));
    }

    void new_pather() {
      mp.reset(new micropather::MicroPather(mpg.get(), 4000000, 8, false));
    }

    Path path(Coord start, Coord finish) {
      if (!my_map->get_data().check_bounds(start) ||
          !my_map->get_data().check_bounds(finish) ||
          !my_map->is_passable(start) || !my_map->is_passable(finish)) {
        return Path({}, -1);
      }
      if (start == finish) {
        return Path{{start}, 0};
      }
      // Paradoxically, keeping the pather around slows it down AND caches even
      // when manually cleared and constructed with disabled cache...
      new_pather();
      MP_VECTOR<void*> path_vc;
      float length = 279;
      mp->Solve(mpg->encode(start), mpg->encode(finish), &path_vc, &length);
      std::vector<Coord> real_path(path_vc.size());
      for (size_t i = 0; i < path_vc.size(); ++i) {
        real_path[i] = mpg->decode(path_vc[i]);
      }
      if (real_path.size() > 0) {
        return Path(std::move(real_path), length);
      } else {
        return Path{{}, -1};
      }
    }

  private:
    const DeltaMap* my_map;
    std::unique_ptr<MicropatherGraph> mpg;
    std::unique_ptr<micropather::MicroPather> mp;
};
#endif

class ResourceManager {
  public:
    static const suncatcher::pathfinder::MapBuilder& get_builder(std::string fn) {
      if (get_me().builder_cache.find(fn) == get_me().builder_cache.end()) {
        std::ifstream is(std::string("maps/" + fn + ".txt"));
        get_me().builder_cache[fn] = MapBuilder(is);
      }
      return get_me().builder_cache[fn];
    }

    static const DeltaMap& get_map(std::string fn) {
      if (get_me().map_cache.find(fn) == get_me().map_cache.end()) {
        get_me().map_cache[fn].reset(new DeltaMap(suncatcher::pathfinder::MapBuilder(get_builder(fn))));
      }
      return *get_me().map_cache[fn];
    }

    #ifdef MICROPATHER_DELTA_TEST
      static const MPWrapper& get_micropather(std::string fn) {
        return MPWrapper(&get_map(fn));
      }
    #endif

  private:
    static ResourceManager& get_me() {
      static ResourceManager self;
      return self;
    }

    ResourceManager() {};
    std::map<const std::string, suncatcher::pathfinder::MapBuilder> builder_cache;
    std::map<const std::string, std::unique_ptr<DeltaMap>> map_cache;
};


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

  protected:
    const suncatcher::test::DeltaMap& map;
};

// //TODO: so not ok...figure out how to do this right
// //TODO: still absurdly slow...figure out why (no it's not the map copy that's ~1k bytes taking 100 ms for the slow cases)
// //TODO: relies on side effect of MapTest setup...this can't end well...
// //the Builder.
// class DynamicMapTest : public ::testing::Test {
//   protected:
//     void make_map_copy(const std::unique_ptr<const MapBuilder>& mb) {
//       map.reset(new DeltaMap(MapBuilder(*mb)));
//       #ifdef MICROPATHER_DELTA_TEST
//         micro.reset(new MPWrapper(map.get()));
//       #endif
//     }
//
//     bool is_door(Coord c) {
//       return (map->get_doors().find(c) != map->get_doors().end());
//     }
//
//   public:
//     std::unique_ptr<DeltaMap> map;
//     #ifdef MICROPATHER_DELTA_TEST
//       std::unique_ptr<MPWrapper> micro;
//     #endif
// };
//
// class DynamicMainMapTest : public DynamicMapTest {
//   protected:
//     virtual void SetUp() final override {
//       make_map_copy(MapTest::main_map_builder);
//     }
// };
//
// class DynamicMicroMapTest : public DynamicMapTest {
//   protected:
//     virtual void SetUp() final override {
//       make_map_copy(MapTest::micro_map_builder);
//     }
// };
//
// class DynamicDoorlandMapTest : public DynamicMapTest {
//   protected:
//     virtual void SetUp() final override {
//       make_map_copy(MapTest::doorland_map_builder);
//     }
// };
//
// class DynamicEmptyMapTest : public DynamicMapTest {
//   protected:
//     virtual void SetUp() final override {
//       make_map_copy(MapTest::empty_map_builder);
//     }
// };
//
