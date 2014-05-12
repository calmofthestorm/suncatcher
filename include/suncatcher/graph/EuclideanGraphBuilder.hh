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

#ifndef EUCLIDEANGRAPHBUILDER_36ddaa5cac2d43bc999e75a96dafc375
#define EUCLIDEANGRAPHBUILDER_36ddaa5cac2d43bc999e75a96dafc375

#include <cstdint>
#include <iosfwd>
#include <map>

#include "suncatcher/platform.hh"
#include "suncatcher/graph/CowGrid.hh"

#include "suncatcher/Door.hh"
#include "suncatcher/Coord.hh"

namespace suncatcher {

namespace pathfinder {
  class MapImpl;
  class MapBuilder;
}  // namespace pathfinder

namespace graph {

class EuclideanGraph;

class EuclideanGraphBuilder {
  public:
    EuclideanGraphBuilder();

    // Load a EuclideanGraphBuilder from a simple text format. Intended mostly
    // for tests and debugging.
    explicit EuclideanGraphBuilder(std::istream& is);

    // Set/get the cost of the specified cell. Can't do this to a door.
    inline const uint_least8_t& cost(Coord cell) const { return data.at(cell); }
    inline uint_least8_t& cost(Coord cell) {
      assert(doors.find(cell) == doors.end());
      return data.at(cell);
    }

    // If dynamic updates are disabled, every mutation will recompute all cache
    // state (linear in graph size).
    void enable_dynamic_updates(bool enabled) { dynamic_updates = enabled; }

    // Add a door to the given cell.
    void add_door(Coord cell, bool open, uint_least8_t cost_open,
                  uint_least8_t cost_closed);

  private:
    friend class pathfinder::MapImpl;
    friend class pathfinder::MapBuilder;
    friend class EuclideanGraph;

    #ifdef EuclideanGridUseCOW
      CowGrid<uint_least8_t> data;
      CowGrid<int_least32_t> color;
    #else
      util::Grid<uint_least8_t> data;
      util::Grid<int_least32_t> color;
    #endif
    bool dynamic_updates;
    std::map<const Coord, pathfinder::Door> doors;
};

}  // namespace pathfinder
}  // namespace suncatcher

#endif  /* EUCLIDEANGRAPHBUILDER_36ddaa5cac2d43bc999e75a96dafc375 */
