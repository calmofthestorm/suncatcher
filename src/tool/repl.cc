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
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <tuple>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "suncatcher/Coord.hh"
#include "suncatcher/MapView.hh"
#include "suncatcher/MapBuilder.hh"
#include "suncatcher/graph/EuclideanGraphBuilder.hh"
#include "suncatcher/MapMutator.hh"
#include "suncatcher/util/util.hh"
#include "suncatcher/platform.hh"

#ifdef MICROPATHER_DELTA_TEST
#include "suncatcher/micropather/MicropatherGraph.hh"
#include "suncatcher/micropather/micropather.hh"
#endif

namespace {

using suncatcher::util::manhattan;
using suncatcher::pathfinder::MapView;
using suncatcher::pathfinder::MapMutator;
using suncatcher::pathfinder::Path;
using suncatcher::Coord;
using suncatcher::pathfinder::MapBuilder;
using suncatcher::graph::EuclideanGraphBuilder;
using suncatcher::graph::EuclideanGraph;

#ifdef MICROPATHER_DELTA_TEST
using suncatcher::test::MicropatherGraph;
#endif

int timer() {
  static auto begin = std::chrono::high_resolution_clock::now();
  auto tmp = std::chrono::high_resolution_clock::now();
  long rval = std::chrono::duration_cast<std::chrono::microseconds>(tmp - begin).count();
  begin = tmp;
  return rval;
}

}  // anonymous namespace

int main(int argc, char** argv) {
  assert(argc == 2 || argc == 4);

  std::ifstream is(argv[1]);
  if (!is) {
    std::cerr << "Unable to open map " << argv[1] << std::endl;
    exit(-1);
  }
  auto builder = MapBuilder(EuclideanGraphBuilder(is));

  timer();
  MapView my_map(std::move(builder));
  std::cout << "Flood fill time: " << timer() << std::endl;

  std::vector<Coord> door_index_to_coords;
  for (const auto& it : my_map.get_doors()) {
    door_index_to_coords.push_back(it.first);
  }

  #ifdef MICROPATHER_DELTA_TEST
  MicropatherGraph mgraph(my_map);
  auto mp(make_unique<micropather::MicroPather>(&mgraph, 4000000, 8, false));
  MP_VECTOR<void*> pa;
  #endif

  Path my_path;

  if (argc == 4) {
    Coord start{1, 1, 0};
    Coord finish{(uint16_t)atoi(argv[2]), (uint16_t)atoi(argv[3]), 0};
    float co = 23;

    timer();
    my_path = my_map.path(start, finish);
    std::cout << "Alex: " << timer() << std::endl;

    #ifdef MICROPATHER_DELTA_TEST
    mp->Solve(mgraph.encode(start), mgraph.encode(finish), &pa, &co);
    std::cout << "Micropather: " << timer() << std::endl;
    std::cout << "MicroPather says distance is " << co << std::endl;
    #endif

    return 0;
  }

  bool autoshow = false;
  while (std::cin) {
    if (autoshow) {
      my_map.print_map(std::cout, true, my_path);
    }
    char cmd;
    std::cout << "\n\n>> ";
    std::cin >> cmd;
    switch (cmd) {
      case 'p':
        {
          uint16_t a, b, c, d;
          std::cin >> a >> b >> c >> d;
          float co = 23;
          Coord start{a, b, 0};
          Coord finish{c, d, 0};

          timer();
          my_path = my_map.path(start, finish);
          std::cout << "Alex: " << timer() << std::endl;

          #ifdef MICROPATHER_DELTA_TEST
          mp->Solve(mgraph.encode(start), mgraph.encode(finish), &pa, &co);
          std::cout << "Micropather: " << timer() << std::endl;
          std::cout << "MicroPather says distance is " << co << std::endl;
          #endif

          break;
        }

      case 'd':
        size_t door;
        std::cin >> door;
        if (door >= door_index_to_coords.size()) {
          std::cout << "There are only " << door_index_to_coords.size() << " doors." << std::endl;
        } else {
          Coord door_coord = door_index_to_coords.at(door);
          timer();
          my_map = MapMutator(my_map).toggle_door_open(door_coord).execute();
          std::cout << timer() << " to open door." << std::endl;
        }
        break;

      case 'S':
        autoshow = !autoshow;
        break;

      case 's':
        my_map.print_map(std::cout, true, my_path);
        break;

      case 'c':
        my_map.print_colors(std::cout);
        break;

      case 'w':
        uint16_t row, col;
        std::cin >> row >> col;
        timer();
        my_map = MapMutator(my_map).set_cost({row, col, 0}, suncatcher::PATH_COST_INFINITE).execute();
        std::cout << "Mutation time: " << timer() << std::endl;
        break;

      case 'E':
        my_map.print_static_components(std::cout);
        break;

      case 'e':
        my_map.print_dynamic_components(std::cout);
        break;

      case 'x':
        return 0;
        break;

      default:
        std::cout << "Commands: p <> <> <> <>; d <>; s; S; c; e" << std::endl;
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
