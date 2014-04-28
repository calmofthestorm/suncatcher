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

#include "DynamicDisjointSets.h"
#include "UnionFind.h"
#include "micropather/MicropatherGraph.h"
#include "micropather/micropather.h"

#include "Coord.h"
#include "Map.h"
#include "UnionFind.h"
#include "MapMutator.h"
#include "util.h"

using suncatcher::util::manhattan;
using suncatcher::pathfinder::Map;
using suncatcher::pathfinder::MicropatherGraph;
using suncatcher::pathfinder::MapMutator;
using suncatcher::pathfinder::MapBuilder;
using suncatcher::pathfinder::Path;
using suncatcher::pathfinder::Coord;
using suncatcher::pathfinder::PATH_COST_INFINITE;

using namespace suncatcher::util;

using micropather::MicroPather;

class MPWrapper {
  public:
    MPWrapper(const Map* map) {
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
      if (!mp) {
        new_pather();
      }
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
    const Map* my_map;
    std::unique_ptr<MicropatherGraph> mpg;
    std::unique_ptr<micropather::MicroPather> mp;
};


class DynamicMapTest {
  public:
    void make_map_copy(const MapBuilder& mb) {
      map.reset(new Map(MapBuilder(mb)));
    }

    std::unique_ptr<Map> map;
};


// Checks all door configurations (exponential!) and all pairs of coordinates
// three ways -- optimized mutaton map, map with no mutation (both my
// algo and MicroPather).
int main(int, char**  argv) {
  DynamicMapTest df;
  std::ifstream is(argv[1]);
  MapBuilder builder(is);
  df.make_map_copy(MapBuilder(builder));

  DynamicMapTest mpdmt;
  // builder.enable_dynamic_updates(false);
  mpdmt.make_map_copy(MapBuilder(builder));

  auto doors = df.map->get_doors();

  size_t num_iterations = std::pow(2, doors.size());

  for (size_t iterations = 0; iterations < num_iterations; ++iterations) {
    size_t mask = 1;
    for (const auto& door : doors) {
      df.map->mutate(std::move(df.map->get_mutator().set_door_open(door.first, mask & iterations)));
      mpdmt.map->mutate(std::move(mpdmt.map->get_mutator().set_door_open(door.first, mask & iterations)));
      mask <<= 1;
    }

    MPWrapper mpw(mpdmt.map.get());
    for (size_t j = 0;j < df.map->size().row; ++j) {
      for (size_t i = 0; i < df.map->size().col; ++i) {
        for (size_t j2 = 0; j2 < df.map->size().row; ++j2) {
          for (size_t i2 = 0; i2 < df.map->size().col; ++i2) {
            auto actual = df.map->path({j, i}, {j2, i2});
            auto expected = mpw.path({j, i}, {j2, i2});
            auto expected2 = mpdmt.map->path({j, i}, {j2, i2});
            if (std::abs(actual.get_length() - expected.get_length()) >= 0.1) {
              std::cout << "(" << j << ", " << i << ") -> (" << j2 << ", " << i2 << ")" << std::endl;
              df.map->print_map(std::cout, actual);
              std::cout << std::endl;
              df.map->print_components(std::cout);
              std::cout << std::endl;
              df.map->print_equivalence_classes(std::cout);
              std::cout << std::endl;
              std::cout << std::endl;
              std::cout << actual << std::endl;
              mpdmt.map->print_map(std::cout, expected);
              std::cout << std::endl;
              mpdmt.map->print_components(std::cout);
              std::cout << std::endl;
              mpdmt.map->print_equivalence_classes(std::cout);
              std::cout << std::endl;
              std::cout << std::endl;
              std::cout << expected << std::endl;
            }
            assert(std::abs(actual.get_length() - expected.get_length()) < 0.1);
            assert(std::abs(actual.get_length() - expected2.get_length()) < 0.1);
            assert(std::abs(expected.get_length() - expected2.get_length()) < 0.1);
          }
        }
      }
    }

    mask = 1;
    for (const auto& door : doors) {
      df.map->mutate(std::move(df.map->get_mutator().set_door_open(door.first, !(mask & iterations))));
      mpdmt.map->mutate(std::move(mpdmt.map->get_mutator().set_door_open(door.first, !(mask & iterations))));
      mask <<= 1;
    }

    std::cerr << "Iteration " << (1 + iterations) << "/" << num_iterations << " complete!" << std::endl;
  }
}
