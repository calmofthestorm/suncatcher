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

class Map {
  public:
    explicit Map(std::istream& infile);
    void print_components(std::ostream& os) const;
    void print_equivalence_classes(std::ostream& os) const;
    void print_map(std::ostream& os) const;

    std::vector<pathfinder::Coord> path(
        const pathfinder::Coord& src,
        const pathfinder::Coord& dst
      ) const ;

    bool same_equivalence_class(
        pathfinder::Coord src,
        pathfinder::Coord dst
      ) const ;

    bool is_passable(const pathfinder::Coord& cell) const;

    void update_equivalence(const pathfinder::Coord& pos, bool new_state);

    const pathfinder::Coord& get_size() const { return size; }

  // private:
    // Not necessarily a door; represents any point that is expected to change
    // between passable and impassible during normal play.
    struct Door {
      bool open;
      std::vector<uint_least32_t> adjacent_components;
    };

    suncatcher::util::Grid<uint_least8_t> data;
    suncatcher::util::Grid<uint_least32_t> component;
    std::vector<uint_least32_t> equivalent_components;
    std::map<pathfinder::Coord, Door> doors;
    Coord size;
};

}  // namespace pathfinder
}  // namespace suncatcher

#endif  /* PATHFINDER_2c83331d8b5849b28b5f40b38a444a7a */
