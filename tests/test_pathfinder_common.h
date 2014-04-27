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

#define MICROPATHER_DELTA_TEST

#ifdef MICROPATHER_DELTA_TEST
#include "micropather/micropather.h"
#include "micropather/MicropatherGraph.h"
#endif

#include "Coord.h"
#include "Map.h"
#include "MapMutator.h"
#include "util.h"

using suncatcher::util::manhattan;
using suncatcher::pathfinder::Map;
using suncatcher::pathfinder::MapMutator;
using suncatcher::pathfinder::MapBuilder;
using suncatcher::pathfinder::Path;
using suncatcher::pathfinder::MicropatherGraph;
using suncatcher::pathfinder::Coord;
using suncatcher::pathfinder::PATH_COST_INFINITE;

#ifdef MICROPATHER_DELTA_TEST
class MPWrapper {
  public:
    MPWrapper(const Map* map) {
      my_map = map;
      mpg.reset(new MicropatherGraph(map));
    }

    Path path(Coord start, Coord finish) {
      if (!my_map->get_data().check_bounds(start) ||
          !my_map->get_data().check_bounds(finish)) {
        return Path({}, -1);
      }
      // Paradoxically, keeping the pather around slows it down AND caches even
      // when manually cleared and constructed with disabled cache...
      mp.reset(new micropather::MicroPather(mpg.get(), 4000000, 8, false));
      MP_VECTOR<void*> path_vc;
      float length = 279;
      mp->Solve(mpg->encode(start), mpg->encode(finish), &path_vc, &length);
      std::vector<Coord> real_path(path_vc.size());
      for (size_t i = 0; i < path_vc.size(); ++i) {
        real_path[i] = mpg->decode(path_vc[i]);
      }
      return Path(std::move(real_path), length);
    }

  private:
    const Map* my_map;
    std::unique_ptr<MicropatherGraph> mpg;
    std::unique_ptr<micropather::MicroPather> mp;
};
#endif

// TODO: ugh
class MapTest : public ::testing::Test {
  protected:
    static void SetUpTestCase() {
      {
        std::ifstream is("maps/open.txt");
        empty_map_builder.reset(new MapBuilder(is));
        empty_map.reset(new Map(MapBuilder(*empty_map_builder.get())));
      }
      {
        std::ifstream is("maps/map.txt");
        main_map_builder.reset(new MapBuilder(is));
        main_map.reset(new Map(MapBuilder(*main_map_builder.get())));
      }
      {
        std::ifstream is("maps/micro.txt");
        micro_map_builder.reset(new MapBuilder(is));
        micro_map.reset(new Map(MapBuilder(*micro_map_builder.get())));
      }
      {
        std::ifstream is("maps/doorland.txt");
        doorland_map_builder.reset(new MapBuilder(is));
        doorland_map.reset(new Map(MapBuilder(*doorland_map_builder.get())));
      }
      #ifdef MICROPATHER_DELTA_TEST
        empty_mp.reset(new MPWrapper(empty_map.get()));
        main_mp.reset(new MPWrapper(main_map.get()));
      #endif
    }

  public:
    static std::unique_ptr<const MapBuilder> empty_map_builder, main_map_builder, micro_map_builder, doorland_map_builder;
    static std::unique_ptr<const Map> empty_map, main_map, micro_map, doorland_map;
    static std::unique_ptr<MPWrapper> empty_mp, main_mp, micro_mp, doorland_mp;
};

//TODO: so not ok...figure out how to do this right
//TODO: still absurdly slow...figure out why (no it's not the map copy that's ~1k bytes taking 100 ms for the slow cases)
//TODO: relies on side effect of MapTest setup...this can't end well...
//the Builder.
class DynamicMapTest : public ::testing::Test {
  protected:
    void make_map_copy(const std::unique_ptr<const MapBuilder>& mb) {
      map.reset(new Map(MapBuilder(*mb)));
      #ifdef MICROPATHER_DELTA_TEST
        micro.reset(new MPWrapper(map.get()));
      #endif
    }

    bool is_door(Coord c) {
      return (map->get_doors().find(c) != map->get_doors().end());
    }

  public:
    std::unique_ptr<Map> map;
    std::unique_ptr<MPWrapper> micro;
};

class DynamicMainMapTest : public DynamicMapTest {
  protected:
    virtual void SetUp() final override {
      make_map_copy(MapTest::main_map_builder);
    }
};

class DynamicMicroMapTest : public DynamicMapTest {
  protected:
    virtual void SetUp() final override {
      make_map_copy(MapTest::micro_map_builder);
    }
};

class DynamicEmptyMapTest : public DynamicMapTest {
  protected:
    virtual void SetUp() final override {
      make_map_copy(MapTest::empty_map_builder);
    }
};

