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

#include "suncatcher/MapView.hh"

namespace suncatcher {
namespace pathfinder {

class MapImpl;

// You must do at most one of these groups to a given cell in a given Mutator.
// * create_door
// * remove_door
// * set_cost
// * set_door_open_cost, set_door_open, toggle_door_open
//
// This restriction is in place due to the complexity of tracking changes and
// lack of a compelling use case. TODO: remove this restriction.
//
// All operations are deferred until execute is called.
//
// NOTE: currently multiple cell mutations are applied sequentially, which is
// inefficient (IE, no advantage over one call to Map::mutate per affected
// cell). TODO: I plan to fix this later.
class MapMutator {
  public:
    MapMutator();
    explicit MapMutator(MapView view);

    // Sets/toggles a door's openness state Asserts cell is a door. Opening a
    // door is O(1). Closing a door is at worst O(num_doors_on_map). Open cost
    // must not (asserted) be PATH_COST_INFINITE.
    MapMutator& set_door_open_cost(Coord door, uint_least8_t cost);

    // Gets/sets/toggles a door's openness state. Asserts cell is a door.
    // Opening a door is O(1). Closing a door is at worst O(num_doors_on_map).
    MapMutator& set_door_open(Coord door, bool open);

    // Convenience call for changing door state. Same rules as set.
    MapMutator& toggle_door_open(Coord door);

    // Gets/sets the traversal cost of a block. Changing between non-
    // PATH_COST_INFINITE values or from PATH_COST_INFINITE to other values is
    // constant time. Changing to PATH_COST_INFINITE is at worst linear in
    // the size of the map. Asserts cell is not a door.
    MapMutator& set_cost(Coord cell, uint_least8_t cost);

    // Creates/removes a door. Asserts is/is not a door. Creating a door is
    // at worst linear time in entire map. Removing a door is constant time.
    // open_cost must not be (asserted) PATH_COST_INFINITE. new_cost may be
    // any value, including PATH_COST_INFINITE. Essentially this lets you
    // atomically replace a door with a wall.
    MapMutator& create_door(Coord cell, bool open, uint_least8_t open_cost);
    MapMutator& remove_door(Coord cell, uint_least8_t new_cost);

    // Creates a new MapView with the changes applied. These are copy on write
    // with somewhat fine-grained granularity, so while mutation is still
    // kinda pricey it's not absurd. TODO: the entire map is still one chunk.
    MapView execute(bool incremental = true) const;

  private:
    friend class MapImpl;

    struct Mutation {
      enum class Kind {UPDATE_DOOR, CREATE_DOOR, REMOVE_DOOR, SET_COST};
      Kind kind;
      bool state;
      uint_least8_t cost;
    };


  private:
    MapView view;
    std::map<Coord, Mutation> mutations;
};

}  // namespace pathfinder
}  // namespace suncatcher

#endif  /* MAPMUTATOR_fce1b7b2c66b4d5f90254ebb435a21c5 */
