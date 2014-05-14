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

#include "suncatcher/Coord.hh"
#include "suncatcher/MapImpl.hh"
#include "suncatcher/MapBuilder.hh"
#include "suncatcher/PathStateDelegate.hh"
#include "suncatcher/CheckedMutator.hh"

namespace suncatcher {
namespace pathfinder {

using suncatcher::util::Grid;
using suncatcher::util::find_representative;
using suncatcher::util::manhattan;


namespace {
  struct Entry {
    Coord pos;
    float cost;

    // Make pqueue work in proper order.
    bool operator< (const Entry& e) const {
      return cost > e.cost;
    }
  };
}  // anonymous namespace


MapView::MapView()
: map(nullptr) { }


MapView::MapView(pathfinder::MapBuilder&& builder)
: map(std::make_shared<MapImpl>(std::move(builder))) { }


MapView::MapView(std::shared_ptr<MapImpl> backing)
: map(backing) { }


bool MapView::path_helper(Coord src, Coord dst, Path& out_path) const {
  size_t num_expanded = 0;
  if (!map->check_bounds(src) || !map->check_bounds(dst)) {
    out_path = Path({}, -1);
    return true;
  }

  // A path exists iff they are in the same equivalence class and both are
  // passable squares.
  if (!is_passable(src) || !is_passable(dst) || !path_exists(src, dst)) {
    out_path = Path(std::vector<Coord>(), -1);
    return true;
  }

  // Order intentional, if A is impassable there is no path from A to A.
  if (src == dst) {
    out_path = Path(std::vector<Coord> {src}, 0);
    return true;
  }

  return false;
}

// TODO: look at templating this to reduce duplication.
Path MapView::path(Coord src, Coord dst, PatherStateDelegate& state) const {
  state.clear();

  Path rval;
  if (path_helper(src, dst, rval)) {
    return rval;
  }

  state.set_distance(src, 0);

  std::priority_queue<Entry> fringe;
  fringe.push({src, manhattan(src, dst)});

  std::vector<Coord> adjacent;
  while (!fringe.empty()) {
    Entry cur = fringe.top();
    assert(is_passable(cur.pos));
    fringe.pop();
    if (!state.get_expanded(cur.pos)) {
      state.set_expanded(cur.pos, true);

      if (cur.pos == dst) {
        // TODO: unnecessary copy
        std::vector<Coord> points{dst};
        points.reserve((unsigned int)(state.get_distance(dst) + 1));
        while (points.back() != src) {
          points.push_back(state.get_previous(points.back()));
        }
        std::reverse(points.begin(), points.end());
        return Path(std::move(points), state.get_distance(dst));
      }

      map->get_adjacent(cur.pos, true, adjacent);
      for (const auto& next : adjacent) {
        float cost = move_cost(cur.pos, next);
        float my_dist = state.get_distance(cur.pos) + cost;
        if (!state.get_expanded(next) &&
            state.get_distance(next) > my_dist &&
            cost != -1) {
          state.set_distance(next, my_dist);
          fringe.push({next, manhattan(next, dst) + my_dist});
          state.set_previous(next, cur.pos);
        }
      }
    }
  }
  assert(0);
  return Path(std::vector<Coord>(), -1);
}


Path MapView::path(Coord src, Coord dst) const {
  #ifdef POLYMORPHIC_API
    std::unordered_map<Coord, bool> expanded;
    std::unordered_map<Coord, float> distance;
    std::unordered_map<Coord, Coord> previous;
  #else
    util::Grid<bool> expanded(map->domain().euclidean_size(), false);
    util::Grid<float> distance(map->domain().euclidean_size(), INFINITY);
    util::Grid<Coord> previous(map->domain().euclidean_size(), Coord());
  #endif

  Path rval;
  if (path_helper(src, dst, rval)) {
    return rval;
  }

  distance[src] = 0;

  std::priority_queue<Entry> fringe;
  fringe.push({src, manhattan(src, dst)});

  std::vector<Coord> adjacent;
  while (!fringe.empty()) {
    Entry cur = fringe.top();
    assert(is_passable(cur.pos));
    fringe.pop();
    if (!expanded[cur.pos]) {
      expanded[cur.pos] = 1;

      if (cur.pos == dst) {
        // TODO: unnecessary copy
        std::vector<Coord> points{dst};
        points.reserve((unsigned int)(distance[dst] + 1));
        while (points.back() != src) {
          points.push_back(previous.at(points.back()));
        }
        std::reverse(points.begin(), points.end());
        return Path(std::move(points), distance[dst]);
      }

      map->get_adjacent(cur.pos, true, adjacent);
      for (const auto& next : adjacent) {
        float cost = move_cost(cur.pos, next);
        float my_dist = distance[cur.pos] + cost;
        if (!expanded[next] && (
              #ifdef POLYMORPHIC_API
              distance.find(next) == distance.end() ||
              #endif 
              distance[next] > my_dist) &&
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


MapView MapView::apply(const CheckedMutator& mutator, bool incremental) const {
  return apply(mutator.to_map_mutator(), incremental);
}


MapView MapView::apply(const MapMutator& mutator, bool incremental) const {
  std::shared_ptr<MapImpl> next(std::make_shared<MapImpl>(*map));
  next->update(mutator, incremental);
  return MapView(next);
}



}  // namespace pathfinder
}  // namespace suncatcher
