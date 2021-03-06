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

#ifndef MAPMUTATOR_fce1b7b2c66b4d5f90254ebb435a21c5
#define MAPMUTATOR_fce1b7b2c66b4d5f90254ebb435a21c5

#include <cstdint>
#include <map>

#include "platform.hh"

#include "suncatcher/Coord.hh"

namespace suncatcher {
namespace pathfinder {

class MapImpl;

// All operations are deferred until execute is called. This includes things
// like checking the bounds of the cell. Coord must always be in bounds.
// If you make more than one call affecting the same cell, the result will be
// the same as if only the last per cell were executed, but is less efficient.
//
// MapMutator only specifies the final state of cells, which means it is simply
// a collection of batched changes. It is not tied to any particular map.
//
// It is suggested to use CheckedMutator instead for actual programs, but may
// be useful for building intermediate layers (such as predictive caches)
class MapMutator {
  public:
    // Sets the specified coordinate to be a door, with the specified open
    // state and cost to traverse (when open). Cost must not be
    // PATH_COST_INFINITE.
    MapMutator& make_door(Coord door, bool open, uint_least8_t cost);

    // Sets the specified coordinate to be a ordinary square with the specified
    // cost to traverse. If the cost is PATH_COST_INFINITE, this creates a
    // wall.
    MapMutator& make_normal(Coord cell, uint_least8_t cost);


  private:
    friend class MapImpl;

    struct Mutation {
      Coord cell;
      bool door, open;
      uint_least8_t cost;
    };

    std::vector<Mutation> mutations;
};

}  // namespace pathfinder
}  // namespace suncatcher

#endif  /* MAPMUTATOR_fce1b7b2c66b4d5f90254ebb435a21c5 */
