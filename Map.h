#ifndef PATHFINDER_2c83331d8b5849b28b5f40b38a444a7a
#define PATHFINDER_2c83331d8b5849b28b5f40b38a444a7a

// Pathfinder abstraction of the map. The Map class maintains a simplified
// abstraction of the map, intended to support efficient pathfinding,
// reachability, and updates in a dynamic world.

#include <iostream>
#include <map>
#include <vector>

#include "Grid.h"

namespace suncatcher {
namespace pathfinder {

class Map {
  public:
    explicit Map(std::istream& infile);
    void print_components(std::ostream& os) const;
    void print_equivalence_classes(std::ostream& os) const;
    void print_map(std::ostream& os) const;
    std::vector<pathfinder::Coord> path(
        pathfinder::Coord src,
        pathfinder::Coord dst
      );

    void update_equivalence(const pathfinder::Coord& pos, bool new_state);

    const pathfinder::Coord& get_size() const { return size; }

  // private:
    // Not necessarily a door; represents any point that is expected to change
    // between passable and impassible during normal play.
    struct Door {
      bool open;
      std::vector<int> adjacent_components;
    };

    suncatcher::util::Grid<char> data;
    suncatcher::util::Grid<int> component;
    std::vector<int> equivalent_components;
    std::map<pathfinder::Coord, Door> doors;
    Coord size;
};

}  // namespace pathfinder
}  // namespace suncatcher

#endif  /* PATHFINDER_2c83331d8b5849b28b5f40b38a444a7a */
