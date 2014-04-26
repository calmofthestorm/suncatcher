#ifndef MAPMUTATOR_fce1b7b2c66b4d5f90254ebb435a21c5
#define MAPMUTATOR_fce1b7b2c66b4d5f90254ebb435a21c5

#include "Coord.h"

namespace suncatcher {
namespace pathfinder {

class Map;

// To create a mutator, see Map's docs. Note that your mutator must not outlive
// the map.
//
// In general, operations that conflict, the LATTER will take effect (eg,
// setting a cell's traversal value then creating a door, the traversal value
// change has no effect). That said, the main intent is to allow batching of
// many small requests to reduce update needs.
class MapMutator {
  public:
    // Gets/sets/toggles a door's openness state. Asserts cell is a door.
    // Opening a door is O(1). Closing a door is at worst O(num_doors_on_map).
    bool get_door_open(Coord door) const;
    void set_door_open(Coord door, bool state);
    void toggle_door_state(Coord door);

    // Gets/sets a door's traversal cost when open/closed. For now, of limited
    // use because open cost must not (asserted) be PATH_COST_INFINITE and
    // closed cost must (asserted) be PATH_COST_INFINITE. Asserts is a door.
    uint_least8_t get_door_open_cost(Coord door) const;
    void set_door_open_cost(Coord door, uint_least8_t cost);

    uint_least8_t get_door_closed_cost(Coord door) const;
    void set_door_closed_cost(Coord door, uint_least8_t cost);

    // Gets/sets the traversal cost of a block. Changing between non-
    // PATH_COST_INFINITE values or from PATH_COST_INFINITE to other values is
    // constant time. Changing to PATH_COST_INFINITE is at worst linear in
    // the size of the map. Asserts cell is not a door.
    uint_least8_t get_cost(Coord cell);
    void set_cost(Coord cell, uint_least8_t cost);

    // Creates/removes a door. Asserts is/is not a door. Creating a door is
    // at worst linear time in entire map. Removing a door is constant time.
    void create_door(Coord cell);
    void remove_door(Coord cell);

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

    Map* map;
};

}  // namespace pathfinder
}  // namespace suncatcher

#endif  /* MAPMUTATOR_fce1b7b2c66b4d5f90254ebb435a21c5 */
