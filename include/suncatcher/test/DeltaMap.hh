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

#ifndef DELTAMAP_d79c6851bc294c57bc25c7bc3bcd5859
#define DELTAMAP_d79c6851bc294c57bc25c7bc3bcd5859

// Class used in unit tests to test two different configurations of maps.
// This tests the internal state that should be consistent, not just the
// results.

#include <map>
#include <memory>

#include "suncatcher/util/Grid.hh"

#include "suncatcher/CheckedMutator.hh"
#include "suncatcher/MapView.hh"
#include "suncatcher/Path.hh"
#include "suncatcher/Coord.hh"
#include "suncatcher/GraphDelegate.hh"

namespace suncatcher {

namespace pathfinder {
  class Door;
  class Map;
  class MapBuilder;
}  // namespace pathfinder

namespace test {



class DeltaMap;


// TODO: evaluate if polymorphism is appropriate for testing code mutators.
//
// DeltaMap is a wrapper class around two map implementations, one with
// optimizations enabled and one with them disabled. Mutations are applied to
// both maps. After each mutation, equivalents of the internal state between
// the maps is tested to ensure that the optimizations did not diverge.
//
// When running in quick check mode (run automatically after every compile),
// the Delta map simply passes commands along to an optimized map and does not
// check against a simple map nor does it verify internal state.
//
//
class DeltaMutator {
  public:
    DeltaMutator(
        pathfinder::CheckedMutator&& m1_i,
        pathfinder::CheckedMutator&& m2_i,
        bool enable_delta_i
      );

    DeltaMutator(const DeltaMutator& other) = delete;
    DeltaMutator& operator=(const DeltaMutator& other) = delete;

    DeltaMutator(DeltaMutator&& other);

    DeltaMutator& operator=(DeltaMutator&& other);

    DeltaMutator& set_door_open_cost(Coord door, uint_least8_t cost);

    DeltaMutator& set_door_open(Coord door, bool open);

    DeltaMutator& toggle_door_open(Coord door);

    DeltaMutator& set_cost(Coord cell, uint_least8_t cost);

    DeltaMutator& create_door(Coord cell, bool open, uint_least8_t open_cost);

    DeltaMutator& remove_door(Coord cell, uint_least8_t new_cost);

  private:
    friend class DeltaMap;
    pathfinder::CheckedMutator m1, m2;
    bool enable_delta;
};


class DeltaMap {
  public:
    DeltaMap();

    explicit DeltaMap(pathfinder::MapView view);

    // Verifies both maps have the same cached data.
    void check_invariant() const;

    // Find a path.
    pathfinder::Path path(Coord start, Coord finish) const;

    bool path_exists(Coord start, Coord finish) const;

    bool is_door(Coord cell) const;

    // Mutate both maps and check the invarant.
    DeltaMutator get_mutator();
    void mutate(const DeltaMutator& mutator);

    // Get map access
    const pathfinder::MapView& get_simple() const { return simple_map; }

    const pathfinder::MapView& get_optimized() const { return optimized_map; }

    const std::map<const Coord, pathfinder::Door>& get_doors() const;

    uint_least8_t get_cost(Coord cell) const;

    void clear_cache();

    pathfinder::Domain domain() const;

    bool enable_delta;

  private:
    pathfinder::MapView optimized_map, simple_map;
};



}  // namespace test
}  // namespace suncatcher


#endif  /* DELTAMAP_d79c6851bc294c57bc25c7bc3bcd5859 */
