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
#include "micropather/MicropatherGraph.h"

namespace {

using suncatcher::util::manhattan;
using suncatcher::pathfinder::Map;
using suncatcher::pathfinder::Path;
using suncatcher::pathfinder::Coord;
using suncatcher::pathfinder::MicropatherGraph;

int timer() {
  static auto begin = std::chrono::high_resolution_clock::now();
  auto tmp = std::chrono::high_resolution_clock::now();
  int rval = std::chrono::duration_cast<std::chrono::microseconds>(tmp - begin).count();
  begin = tmp;
  return rval;
}

}  // anonymous namespace

int main(int argc, char** argv) {
  assert(argc == 2 || argc == 4);
  std::ifstream is(argv[1]);
  suncatcher::pathfinder::MapBuilder mb(is);

  timer();
  Map my_map(std::move(mb));
  std::cout << "Flood fill time: " << timer() << std::endl;
  my_map.rebuild_equivalence_classes();
  std::cout << "Transitive closure time: " << timer() << std::endl;

  std::vector<Coord> door_index_to_coords;
  for (const auto& it : my_map.get_doors()) {
    door_index_to_coords.push_back(it.first);
  }

  MicropatherGraph mgraph(&my_map);

  std::unique_ptr<micropather::MicroPather> mp(new micropather::MicroPather(&mgraph, 4000000, 8, false));
  Path my_path;

  if (argc == 4) {
    Coord start{1, 1};
    Coord finish{(uint16_t)atoi(argv[2]), (uint16_t)atoi(argv[3])};
    MP_VECTOR<void*> pa;
    float co = 23;

    timer();
    my_path = my_map.path(start, finish);
    std::cout << "Alex: " << timer() << std::endl;
    mp->Solve(mgraph.encode(start), mgraph.encode(finish), &pa, &co);
    std::cout << "Micropather: " << timer() << std::endl;
    std::cout << "MicroPather says distance is " << co << std::endl;
    return 0;
  }

  bool autoshow = false;
  while (std::cin) {
    if (autoshow) {
      my_map.print_map(std::cout, my_path);
    }
    char cmd;
    std::cout << "\n\n>> ";
    std::cin >> cmd;
    switch (cmd) {
      case 'p':
        {
          uint16_t a, b, c, d;
          std::cin >> a >> b >> c >> d;
          MP_VECTOR<void*> pa;
          float co = 23;
          Coord start{a, b};
          Coord finish{c, d};

          timer();
          my_path = my_map.path(start, finish);
          std::cout << "Alex: " << timer() << std::endl;
          mp->Solve(mgraph.encode(start), mgraph.encode(finish), &pa, &co);
          std::cout << "Micropather: " << timer() << std::endl;
          std::cout << "MicroPather says distance is " << co << std::endl;
          break;
        }

      case 'd':
        uint32_t door;
        std::cin >> door;
        if (door >= door_index_to_coords.size()) {
          std::cout << "There are only " << door_index_to_coords.size() << " doors." << std::endl;
        } else {
          Coord door_coord = door_index_to_coords.at(door);
          my_map.set_door(door_coord, !my_map.get_door(door_coord));
        }
        break;

      case 'S':
        autoshow = !autoshow;
        break;

      case 's':
        my_map.print_map(std::cout, my_path);
        break;

      case 'c':
        my_path = Path();
        break;
    }

    // my_map.print_components(std::cout);

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
//     return 0;
//     // print_equivalence_classes(std::cout);
//     // print_map(std::cout);
//
//     // char bees;
//     // std::cin >> bees;
//     // if (bees >= '0' && bees <= '9') {
//     //   Door& d = doors[bees - '0'];
//     //   bool state = !d.get_open();
//     //   d.set_open(state);
//     //   data.at(d.get_pos()) = state ? '_' : 'd';
//     //
//     //   update_equivalence(d.get_pos(), state);
//     // }
  }

  return 0;
}
