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

namespace suncatcher {
namespace pathfinder {

MapBuilder::MapBuilder()
: data({0, 0}, 0),
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

}  // namespace pathfinder
}  // namespace suncatcher
