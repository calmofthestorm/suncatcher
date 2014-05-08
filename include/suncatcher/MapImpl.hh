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

#ifndef MAPIMPL_2c83331d8b5849b28b5f40b38a444a7a
#define MAPIMPL_2c83331d8b5849b28b5f40b38a444a7a

// Pathfinder abstraction of the map. The MapImpl class maintains a simplified
// abstraction of the map, intended to support efficient pathfinding,
// reachability, and updates in a dynamic world.
//
// In an early version of this code, I ran a benchmark suite to determine
// whether using smaller integral types for better cache performance versus
// using native word types resulted in better performance. I found ~5%
// improvement on a moderate sample size (p < 0.01) to using smaller types,
// which makes sense given all the computations are simple and most of this
// algorithm is fetching the map from memory.
//
// The other concern is that predictive pathing in multiple threads could run
// into memory pressure.
//
// The code has undergone substantial refactoring since then, however, so this
// may need revisiting in the future.

#include <iostream>
#include <limits>
#include <map>
#include <vector>

#include "suncatcher/util/Grid.hh"
#include "suncatcher/util/UnionFind.hh"

#include "platform.hh"
#include "suncatcher.hh"
#include "suncatcher/Path.hh"
#include "suncatcher/Door.hh"
#include "suncatcher/GraphDelegate.hh"
#include "suncatcher/EuclideanGraph.hh"

namespace suncatcher {

namespace test {
  class DeltaMap;
}  // namespace test

namespace pathfinder {

class MapBuilder;
class MapMutator;

class MapImpl {
  public:
    explicit MapImpl(const MapMutator& mutator, bool incremental = true);
    explicit MapImpl(MapBuilder&& builder);

    // Returns the map's size, as a coordinate.
    inline Coord get_size() const { return graph.size(); }

    // Returns a constant reference to the mapping from coordinates to doors.
    // TODO: fix
    const std::map<const Coord, Door>& get_doors() const { return doors; }

    // True iff the cell/static component is a door. We can take either a
    // coordinate or a (valid) color, since all doors have their own color.
    // Both are constant time.
    inline bool is_door(Coord cell) const;
    inline bool is_door(int_least32_t cell_color) const;

    // True iff the cell is always passable to all movement modes and factions.
    // Thus, returns false for doors regardless of state.
    inline bool is_transparent(Coord cell) const;

    // True if we can currently path into it.
    inline bool is_passable(Coord cell) const;

    // Returns the current cost to move from start to finish. The two must
    // be adjacent (including diagonal). Start must be passable. Returns -1
    // if the move is illegal. Asserts coordinates adjacent.
    inline float move_cost(Coord start, Coord finish) const;

    // Returns true if it is possible to path from src to dst. Unlike path,
    // this will always run in constant time. Invalid inputs result in false.
    inline bool path_exists(Coord src, Coord dst) const;

    inline std::vector<pathfinder::Coord> get_adjacent(
        const pathfinder::Coord cell,
        bool include_diagonals = true
      ) const;

    inline bool check_bounds(Coord cell) const { return graph.check_bounds(cell); }

    // Useful debugging features -- dump a simple representation of aspects
    // of the map to a stream.
    void print_colors(std::ostream& os) const;
    void print_static_components(std::ostream& os) const;
    void print_dynamic_components(std::ostream& os) const;
    void print_map(
        std::ostream& os,
        bool number_doors=true,
        const Path& path= {}
      ) const;

  private:
    friend class test::DeltaMap;

    GraphDelegate graph;

    typedef std::map<const Coord, Door>::iterator DoorIter;

    // Cost to traverse terrain.

    // All registered doors.
    std::map<const Coord, Door> doors;

    // The map is divided into "colors" -- areas connected by purely transparent
    // terrain along Manhattan directions. Colors are handles into static
    // components (thus, we can union two colors in constant time at the price
    // of a small indirect lookup table). Typically there will be anywhere from
    // 1-5 colors per door on the map, plus one for each isolated walled off
    // region.

    // Static component -- represent the same areas as colors, but there may
    // be a many-to-one mapping of colors to static components -- for example,
    // if we remove a door, the three areas (two rooms and door itself) retain
    // their colors but would all map to the same static component.
    util::UnionFind<int_least32_t> static_component;

    // Dynamic component tracking -- organizes static components (rooms) into
    // dynamic components (reachability). This lets us minimize expensive
    // operations such as DFS to determine connected components by performing
    // the frequent ones on a simplified graph.
    util::UnionFind<int_least32_t> dynamic_component;

    // The color of the next door/component to assign. Components grow up;
    // doors grow down.
    int_least32_t next_component_color;
    int_least32_t next_door_color;

    // Helpers for each mutate operation's cases.
    void incremental_create_door(Coord cell, bool state, uint_least8_t cost);
    void incremental_remove_door(Coord cell, bool state, uint_least8_t cost);
    void incremental_set_cost(Coord cell, bool state, uint_least8_t cost);
    void incremental_update_door(Coord cell, bool state, uint_least8_t cost);

    // Helpers for the incremental mutations -- most mutations can be
    // decomposed into combinations of a few constant-time transformations.
    void incremental_wall_to_transparent(Coord cell);
    void incremental_closed_door_to_open_door(Coord cell, DoorIter door_iter);
    void incremental_closed_door_to_wall(
        Coord cell,
        DoorIter door_iter,
        uint_least8_t cost
      );
    void incremental_open_door_to_closed_door(Coord cell, DoorIter door_iter);
    void incremental_transparent_to_wall(Coord cell);
    DoorIter incremental_wall_to_closed_door(
        Coord cell,
        uint_least8_t cost_closed,
        uint_least8_t cost_open
      );

    // Shoutout to Sarah Northway, one of several sources of inspiration for
    // this project! http://rebuildgame.com/
    void rebuild();
    void incremental_regenerate_dynamic_components();
};

}  // namespace pathfinder
}  // namespace suncatcher

#include "MapImpl-inl.hh"

#endif  /* MAPIMPL_2c83331d8b5849b28b5f40b38a444a7a */
