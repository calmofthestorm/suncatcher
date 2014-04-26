#ifndef PATHFINDER_2c83331d8b5849b28b5f40b38a444a7a
#define PATHFINDER_2c83331d8b5849b28b5f40b38a444a7a

// Pathfinder abstraction of the map. The Map class maintains a simplified
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
// made were three visiting in the future.

#include <iostream>
#include <map>
#include <vector>

#include "Grid.h"

namespace suncatcher {
namespace pathfinder {

const uint8_t PATH_COST_INFINITE = (uint8_t)-1;

const uint32_t COMPONENT_MULTIPLE = (uint32_t)-1;
const uint32_t COMPONENT_UNKNOWN = (uint32_t)-2;
const uint32_t COMPONENT_IMPASSABLE = (uint32_t)-3;

class MapBuilder;

class Map {
  public:
    Map(MapBuilder&& builder);

    void print_components(std::ostream& os) const;
    void print_equivalence_classes(std::ostream& os) const;
    void print_map(std::ostream& os, const std::vector<Coord>& path={}) const;

    std::vector<pathfinder::Coord> path(
        const pathfinder::Coord& src,
        const pathfinder::Coord& dst
      ) const ;

    bool same_equivalence_class(
        pathfinder::Coord src,
        pathfinder::Coord dst
        ) const ;

    // Returns the current cost to move from start to finish. The two must
    // be adjacent (including diagonal). Start must be passable.
    inline float move_cost(Coord start, Coord finish) const {
      assert(check_bounds(start));
      assert(check_bounds(finish));
      assert(std::abs(start.row - finish.row) <= 1 &&
            std::abs(start.col - finish.col) <= 1);
      assert(is_passable(start));
      if (start == finish) {
        return 0;
      }

      if (start.row == finish.row || start.col == finish.col) {
        return data.at(finish);
      }

      // Can only move diagonally if Manhattan squares are passable.
      if (is_passable({start.row, finish.col}) || is_passable({finish.row, start.col})) {
        return data.at(finish) * 1.4142135623730951;;
      } else {
        return -1;
      }
    }

    // True iff the cell is always passable to all movement modes and factions.
    // Thus, returns false for doors regardless of state.
    inline bool is_transparent(Coord cell) const {
      assert(check_bounds(cell));
      if (component.at(cell) == COMPONENT_MULTIPLE) {
        return false;
      } else {
        return data.at(cell) != PATH_COST_INFINITE;
      }
    }

    // True iff the cell is always impassable to all movement modes and factions.
    inline bool is_opaque(Coord cell) const {
      assert(check_bounds(cell));
      if (component.at(cell) == COMPONENT_MULTIPLE) {
        return !(doors.at(cell).open);
      } else {
        return data.at(cell) == PATH_COST_INFINITE;
      }
    }

    // True iff the cell is currently passable.
    inline bool is_passable(Coord cell) const {
      assert(check_bounds(cell));
      assert(component.at(cell) != COMPONENT_UNKNOWN);
      bool rval = (data.at(cell) != PATH_COST_INFINITE);
      #ifndef NDEBUG
      if (component.at(cell) == COMPONENT_MULTIPLE) {
        assert(rval == doors.at(cell).open);
      } else {
        assert((component.at(cell) != COMPONENT_IMPASSABLE) == rval);
      }
      #endif
      return rval;
    }

    bool get_door(Coord cell) const;
    void set_door(Coord cell, bool open);

    void update_equivalence(const pathfinder::Coord& pos, bool new_state);

    void rebuild_cache();
    void rebuild_equivalence_classes();

    inline Coord size() const { return data.size(); }

  // private:
    friend class MapBuilder;
    // Not necessarily a door; represents any point that is expected to change
    // between passable and impassible during normal play.
    struct Door {
      bool open;
      uint8_t cost_open; // cost to walk through when open
      uint8_t cost_closed; // cost to walk through when closed
      std::vector<uint_least32_t> adjacent_components;
    };

    suncatcher::util::Grid<uint_least8_t> data;
    suncatcher::util::Grid<uint_least32_t> component;
    std::vector<uint_least32_t> equivalent_components;
    std::map<pathfinder::Coord, Door> doors;
};

class MapBuilder {
  public:
    MapBuilder();
    MapBuilder(const Coord& size, uint8_t cost);

    // Set/get the cost of the specified cell. Can't do this to a door.
    inline const uint8_t& cost(const Coord& cell) const { return data.at(cell); }
    inline uint8_t& cost(const Coord& cell) {
      assert(doors.find(cell) == doors.end());
      return data.at(cell);
    }

    // Add a door to the given cell.
    void add_door(const Coord& cell, bool open, uint8_t cost_open,
                  uint8_t cost_closed);

  private:
    friend class Map;
    suncatcher::util::Grid<uint8_t> data;
    std::map<pathfinder::Coord, Map::Door> doors;
};

}  // namespace pathfinder
}  // namespace suncatcher

#endif  /* PATHFINDER_2c83331d8b5849b28b5f40b38a444a7a */
