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

#include "suncatcher/MapView.hh"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <queue>
#include <stack>
#include <set>
#include <string>
#include <tuple>

#include "suncatcher/util/util.hh"

#include "suncatcher/MapImpl.hh"
#include "suncatcher/MapBuilder.hh"

namespace suncatcher {
namespace pathfinder {

using suncatcher::util::Grid;
using suncatcher::util::find_representative;
using suncatcher::util::manhattan;


MapView::MapView()
: map(nullptr) { }


MapView::MapView(pathfinder::MapBuilder&& builder)
: map(std::make_shared<MapImpl>(std::move(builder))) { }


MapView::MapView(std::shared_ptr<MapImpl> backing)
: map(backing) { }


Path MapView::path(Coord src, Coord dst) const {
  #ifndef POLYMORPHIC_API
  Grid<uint_fast8_t> expanded(map->domain().euclidean_size(), 0);
  Grid<Coord> previous(expanded.size(), {(uint16_t)-1, (uint16_t)-1, (uint16_t)-1});
  Grid<float> distance(expanded.size(), INFINITY);
  #else
  std::unordered_map<Coord, bool> expanded;
  std::unordered_map<Coord, Coord> previous;
  std::unordered_map<Coord, float> distance;
  #endif

  size_t num_expanded = 0;

  if (!map->check_bounds(src) || !map->check_bounds(dst)) {
    return Path({}, -1);
  }

  // A path exists iff they are in the same equivalence class and both are
  // passable squares.
  if (!is_passable(src) || !is_passable(dst) || !path_exists(src, dst)) {
    return Path(std::vector<Coord>(), -1);
  }

  // Order intentional, if A is impassable there is no path from A to A.
  if (src == dst) {
    return Path(std::vector<Coord> {src}, 0);
  }

  distance[src] = 0;

  struct Entry {
    Coord pos;
    float cost;

    // Make pqueue work in proper order.
    bool operator< (const Entry& e) const {
      return cost > e.cost;
    }
  };

  std::priority_queue<Entry> fringe;
  fringe.push({src, manhattan(src, dst)});

  while (!fringe.empty()) {
    Entry cur = fringe.top();
    assert(is_passable(cur.pos));
    fringe.pop();
    if (!expanded[cur.pos]) {
      expanded[cur.pos] = 1;
      ++num_expanded;

      if (cur.pos == dst) {
        // TODO: unnecessary copy
        std::vector<Coord> rval{dst};
        rval.reserve((unsigned int)(distance[dst] + 1));
        while (rval.back() != src) {
          rval.push_back(previous[rval.back()]);
        }
        std::reverse(rval.begin(), rval.end());
        return Path(std::move(rval), distance[dst]);
      }

      for (const auto& next : map->get_adjacent(cur.pos)) {
        float cost = move_cost(cur.pos, next);
        float my_dist = distance[cur.pos] + cost;
        if (distance[next] > my_dist &&
            !expanded[next] &&
            cost != -1) {
          distance[next] = my_dist;
          fringe.push({next, manhattan(next, dst) + my_dist});
          previous[next] = cur.pos;
        }
      }
    }
  }
  assert(0);
  return Path(std::vector<Coord>(), -1);
}



}  // namespace pathfinder
}  // namespace suncatcher
