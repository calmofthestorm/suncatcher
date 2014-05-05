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

#include "suncatcher/MapBuilder.hh"

#include <fstream>
#include <string>

#include "suncatcher.hh"
#include "suncatcher/MapImpl.hh"
#include "suncatcher/CoordRange.hh"

namespace suncatcher {
namespace pathfinder {



MapBuilder::MapBuilder()
: data({0, 0, 0}, 0),
  dynamic_updates(true),
  doors() { }


MapBuilder::MapBuilder(Coord size, uint_least8_t default_cost)
: data(size, default_cost),
  dynamic_updates(true),
  doors() { }


void MapBuilder::add_door(Coord cell, bool open, uint_least8_t cost_open,
                          uint_least8_t cost_closed) {
  assert(doors.find(cell) == doors.end());
  doors[cell] = {open, cost_open, cost_closed};
  data.at(cell) = open ? cost_open : cost_closed;
}


MapBuilder::MapBuilder(std::istream& is) {

  std::string line;
  Coord size;
  is >> size.row >> size.col >> size.layer;
  std::getline(is, line);
  assert(is);
  *this = MapBuilder(size, 1);
  line.clear();
  auto it = line.begin();
  for (const Coord& cell : CoordRange(size)) {
    while (it == line.end()) {
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
