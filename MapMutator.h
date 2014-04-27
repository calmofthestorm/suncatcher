#ifndef MAPMUTATOR_fce1b7b2c66b4d5f90254ebb435a21c5
#define MAPMUTATOR_fce1b7b2c66b4d5f90254ebb435a21c5

#include <map>

#include "Coord.h"

namespace suncatcher {
namespace pathfinder {

class Map;

// To create a mutator, see Map's docs. Note that your mutator must not outlive
// the map.
//
// You must do at most one of these groups to a given cell in a given Mutator.
// * create_door
// * remove_door
// * set_cost
// * set_door_open_cost, set_door_open, toggle_door_open
//
// This restriction is in place due to the complexity of tracking changes and
// lack of a compelling use case.
class MapMutator {
  public:
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


  public:
    MapMutator()
    : map(nullptr) { }

    ~MapMutator();

    MapMutator(const MapMutator& other) = delete;
    MapMutator& operator=(const MapMutator& other) = delete;

    MapMutator& operator=(MapMutator&& other) = default;
    MapMutator(MapMutator&& other) = default;


  private:
    friend class Map;
    MapMutator(Map* map_in);

    struct Mutation {
      enum class Kind {UPDATE_DOOR, CREATE_DOOR, REMOVE_DOOR, SET_COST};
      Kind kind;
      bool state;
      uint_least8_t cost;
    };


  private:
    Map* map;
    size_t version;
    std::map<Coord, Mutation> mutations;
};

}  // namespace pathfinder
}  // namespace suncatcher

#endif  /* MAPMUTATOR_fce1b7b2c66b4d5f90254ebb435a21c5 */
