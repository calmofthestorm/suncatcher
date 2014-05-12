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

#include "suncatcher/graph/EuclideanGraphBuilder.hh"

#include <fstream>
#include <string>

#include "suncatcher.hh"
#include "suncatcher/MapImpl.hh"
#include "suncatcher/graph/EuclideanCoordRange.hh"
#include "suncatcher/Coord.hh"

namespace suncatcher {
namespace graph {


namespace {
  using pathfinder::Door;
}  // anonymous namespace


EuclideanGraphBuilder::EuclideanGraphBuilder()
: data({0, 0, 0}, {0, 0, 0}, 0),
  color({0, 0, 0}, {0, 0, 0}, 0),
  dynamic_updates(true),
  doors() { }


void EuclideanGraphBuilder::add_door(Coord cell, bool open, uint_least8_t cost_open,
                          uint_least8_t cost_closed) {
  assert(doors.find(cell) == doors.end());
  doors[cell] = {open, cost_open, cost_closed};
  data.at(cell) = open ? cost_open : cost_closed;
}


EuclideanGraphBuilder::EuclideanGraphBuilder(std::istream& is) {

  std::string line;
  Coord chunks, chunk_size;
  is >> chunks.row >> chunks.col >> chunks.layer;
  is >> chunk_size.row >> chunk_size.col >> chunk_size.layer;
  std::getline(is, line);
  assert(is);
  data = decltype(data)(chunks, chunk_size, 1);
  color = decltype(color)(chunks, chunk_size, COLOR_UNKNOWN);
  line.clear();
  auto it = line.begin();
  for (const Coord& cell : EuclideanCoordRange(data.size())) {
    while (it == line.end()) {
      assert(is);
      std::getline(is, line);
      it = line.begin();
    }
    assert(is);
    cost(cell) = (*it == '*') ? PATH_COST_INFINITE : 1;
    if (*it == 'd') {
      add_door(cell, true, 1, PATH_COST_INFINITE);
    } else if (*it == 'D') {
      add_door(cell, false, 1, PATH_COST_INFINITE);
    }
    ++it;
  }
}



}  // namespace pathfinder
}  // namespace suncatcher
