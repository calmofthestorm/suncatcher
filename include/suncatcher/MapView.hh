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

#ifndef MAPVIEW_9bd2c9ccc16c4fdba627f334fb874d89
#define MAPVIEW_9bd2c9ccc16c4fdba627f334fb874d89


#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <vector>

#include "suncatcher/util/Grid.hh"
#include "suncatcher/util/UnionFind.hh"

#include "platform.hh"
#include "suncatcher/Path.hh"
#include "suncatcher/Door.hh"
#include "suncatcher/GraphDelegate.hh"

namespace suncatcher {

namespace test {
  class DeltaMap;
}  // namespace test

namespace pathfinder {


class MapImpl;
class MapMutator;
class MapBuilder;
class PatherStateDelegate;
class CheckedMutator;


// Thread safety: All methods are thread safe and lock free because the class
// is immutable.
// (TODO: not actually true; union find)
class MapView {
  public:
    MapView();
    explicit MapView(std::shared_ptr<MapImpl> backing);
    explicit MapView(pathfinder::MapBuilder&& builder);

    // Compute the shortest path between two points if one exists. Returns false
    // path on failure (out of bounds, no path, impassable src/dest, etc).
    Path path(Coord src, Coord dst) const;
    Path path(Coord src, Coord dst, PatherStateDelegate& state) const;

    // Returns true if it is possible to path from src to dst. Unlike path,
    // this will always run in constant time. Invalid inputs result in false.
    inline bool path_exists(Coord src, Coord dst) const;

    // Returns a constant reference to the mapping from coordinates to doors.
    // TODO: replace with stable interface.
    inline const std::map<const Coord, Door>& get_doors() const;

    inline bool is_passable(Coord cell) const;

    inline void get_adjacent(
        const Coord cell,
        bool include_diagonals,
        std::vector<Coord>& adj
      ) const;

    inline bool check_bounds(Coord cell) const;

    inline float move_cost(Coord src, Coord dst) const;

    // True iff the cell/static component is a door. We can take either a
    // coordinate or a (valid) color, since all doors have their own color.
    // Both are constant time.
    inline bool is_door(Coord cell) const;
    inline bool is_door(int_least32_t cell_color) const;

    inline Domain domain() const;

    // Applies a [checked] mutator, returning a new MapView with the changes
    // applied.
    MapView apply(const CheckedMutator& mutator, bool incremental) const;
    MapView apply(const MapMutator& mutator, bool incremental) const;

    // Useful debugging features -- dump a simple representation of aspects
    // of the map to a stream.
    inline void print_colors(std::ostream& os) const;
    inline void print_static_components(std::ostream& os) const;
    inline void print_dynamic_components(std::ostream& os) const;
    inline void print_map(
        std::ostream& os,
        bool number_doors = true,
        const Path& path={}
      ) const;


  private:
    // Needed so that MapImpl can clone itself from a view, used as part
    // of mutation.
    friend class MapImpl;

    // Needed for testing invariant checking.
    friend class test::DeltaMap;

    std::shared_ptr<MapImpl> map;

    bool path_helper(Coord src, Coord dst, Path& out_path) const;
};

}  // namespace pathfinder
}  // namespace suncatcher

#include "MapView-inl.hh"

#endif  /* MAPVIEW_9bd2c9ccc16c4fdba627f334fb874d89 */
