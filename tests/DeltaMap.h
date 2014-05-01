#ifndef DELTAMAP_d79c6851bc294c57bc25c7bc3bcd5859
#define DELTAMAP_d79c6851bc294c57bc25c7bc3bcd5859

// Class used in unit tests to test two different configurations of maps.
// This tests the internal state that should be consistent, not just the
// results.

#include <map>
#include <memory>

#include "MapMutator.h"
#include "Path.h"
#include "Coord.h"
#include "Grid.h"


namespace suncatcher {

namespace pathfinder {
  class Door;
  class Map;
  class MapBuilder;
}  // namespace pathfinder

namespace test {



class DeltaMap;


class DeltaMutator {
  public:
    DeltaMutator(pathfinder::MapMutator&& m1_i, pathfinder::MapMutator&& m2_i);

    DeltaMutator(const DeltaMutator& other) = delete;
    DeltaMutator& operator=(const DeltaMutator& other) = delete;

    DeltaMutator(DeltaMutator&& other);

    DeltaMutator& operator=(DeltaMutator&& other);

    DeltaMutator& set_door_open_cost(pathfinder::Coord door, uint_least8_t cost);

    DeltaMutator& set_door_open(pathfinder::Coord door, bool open);

    DeltaMutator& toggle_door_open(pathfinder::Coord door);

    DeltaMutator& set_cost(pathfinder::Coord cell, uint_least8_t cost);

    DeltaMutator& create_door(pathfinder::Coord cell, bool open, uint_least8_t open_cost);

    DeltaMutator& remove_door(pathfinder::Coord cell, uint_least8_t new_cost);

  private:
    friend class DeltaMap;
    pathfinder::MapMutator m1, m2;
};


class DeltaMap {
  public:
    explicit DeltaMap(pathfinder::MapBuilder mb);

    // Verifies both maps have the same cached data.
    void check_invariant() const;

    // Find a path.
    pathfinder::Path path(pathfinder::Coord start, pathfinder::Coord finish) const;

    bool path_exists(pathfinder::Coord start, pathfinder::Coord finish) const;

    bool is_door(pathfinder::Coord cell) const;

    // Mutate both maps and check the invarant.
    DeltaMutator get_mutator();
    void mutate(DeltaMutator&& mutator);

    // Get map access
    const pathfinder::Map& get_simple() const {
      return *simple_map;
    }

    const pathfinder::Map& get_optimized() const {
      return *optimized_map;
    }

    const std::map<const pathfinder::Coord, pathfinder::Door>& get_doors() const;

    const util::Grid<uint8_t>& get_data() const;

  private:
    std::unique_ptr<pathfinder::Map> optimized_map, simple_map;
};



}  // namespace test
}  // namespace suncatcher


#endif  /* DELTAMAP_d79c6851bc294c57bc25c7bc3bcd5859 */
