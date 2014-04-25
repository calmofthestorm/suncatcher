#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <tuple>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "micropather/micropather.h"

#include "Coord.h"
#include "Map.h"
#include "util.h"

namespace {

using suncatcher::util::manhattan;
using suncatcher::pathfinder::Map;
using suncatcher::pathfinder::Coord;

int timer() {
  static auto begin = std::chrono::high_resolution_clock::now();
  auto tmp = std::chrono::high_resolution_clock::now();
  int rval = std::chrono::duration_cast<std::chrono::microseconds>(tmp - begin).count();
  begin = tmp;
  return rval;
}

class MicropatherGraph : public micropather::Graph {
  public:
    explicit MicropatherGraph(const Map* my_graph)
    : graph(my_graph) { }

    virtual float LeastCostEstimate(void* stateStart, void* stateEnd) const {
      return manhattan(decode(stateStart), decode(stateEnd));
    }

    virtual void AdjacentCost(
        void* state, MP_VECTOR< micropather::StateCost > *adjacent
      ) const {
      auto cur = decode(state);
      for (const auto& n : graph->data.get_adjacent(cur)) {
        float cost = (n.row == cur.row || n.col == cur.col) ? 1 : 1.4142135623730951;
        if (graph->data.at(n) != '*') {
          adjacent->push_back({encode(n), cost});
        }
      }
    }

    virtual void PrintStateInfo(void*) const { }

    void* encode(const suncatcher::pathfinder::Coord& c) const {
      uintptr_t encoded = (c.row * graph->get_size().col + c.col);
      return (void*)encoded;
    }

    suncatcher::pathfinder::Coord decode(void* c) const {
      uintptr_t encoded = (uintptr_t)c;
      uintptr_t width = graph->get_size().col;
      return {(uint16_t)(encoded / width),
              (uint16_t)(encoded % width)};
    }

  private:
    const Map* graph;
};

}  // anonymous namespace

int main(int argc, char** argv) {
  std::cout << sizeof(Coord) << std::endl;
  assert(argc == 4);
  std::ifstream is(argv[1]);
  Map my_map(is);
  MicropatherGraph mgraph(&my_map);

  std::unique_ptr<micropather::MicroPather> mp(new micropather::MicroPather(&mgraph, 4000000, 8, false));
  while (1) {
    MP_VECTOR<void*> pa;
    float co = 23;
    std::cout << "starting" << std::endl;
    Coord start{1, 1};
    Coord finish{(uint16_t)atoi(argv[2]), (uint16_t)atoll(argv[3])};

    my_map.path(start, finish);
    mp->Solve(mgraph.encode(start), mgraph.encode(finish), &pa, &co);
    mp->Reset();

    timer();
    auto my_path = my_map.path(start, finish);
    std::cout << timer() << std::endl;
    mp->Solve(mgraph.encode(start), mgraph.encode(finish), &pa, &co);
    std::cout << timer() << std::endl;
    std::cout << "Distance is " << co << std::endl;

    // std::cout << "*** me" << std::endl;
    // for (const auto& it : my_path) {
    //   std::cout << it << std::endl;
    // }
    //
    // std::cout << "*** MP" << std::endl;
    // for (size_t i = 0; i < pa.size(); ++i) {
    //   std::cout << my_map.decode(pa[i]) << std::endl;
    // }
    //
    return 0;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    // print_components(std::cout);
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    // print_equivalence_classes(std::cout);
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    // print_map(std::cout);

    // char bees;
    // std::cin >> bees;
    // if (bees >= '0' && bees <= '9') {
    //   Door& d = doors[bees - '0'];
    //   bool state = !d.get_open();
    //   d.set_open(state);
    //   data.at(d.get_pos()) = state ? '_' : 'd';
    //
    //   update_equivalence(d.get_pos(), state);
    // }
  }

  return 0;
}
