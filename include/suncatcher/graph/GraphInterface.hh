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

#ifndef GRAPHINTERFACE_0bb164df0d89492d8b330589e323e254
#define GRAPHINTERFACE_0bb164df0d89492d8b330589e323e254

#include "suncatcher/Coord.hh"

namespace suncatcher {
namespace graph {

class GraphDomainInterface;

class GraphInterface {
  public:
    virtual ~GraphInterface() { }

    virtual float move_cost(pathfinder::Coord start, pathfinder::Coord finish) = 0;

    virtual uint_least8_t get_cost(pathfinder::Coord cell) = 0;
    virtual void set_cost(pathfinder::Coord cell, uint_least8_t cost) = 0;

    virtual int_least32_t get_color(pathfinder::Coord cell) = 0;
    virtual void set_color(pathfinder::Coord cell, int_least32_t color) = 0;

    virtual pathfinder::Coord size() = 0;
    virtual bool check_bounds(pathfinder::Coord cell) = 0;

    virtual  std::vector<pathfinder::Coord> get_adjacent(
        const pathfinder::Coord cell,
        bool include_diagonals = true
      ) = 0;

    virtual void fill_color(int_least32_t fill) = 0;

    virtual void is_passable(pathfinder::Coord cell) = 0;

    inline std::unique_ptr<GraphDomainInterface> domain() const = 0;

};

}  // namespace graph
}  // namespace suncatcher

#endif  /* GRAPHINTERFACE_0bb164df0d89492d8b330589e323e254 */
