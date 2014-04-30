#ifndef GRID_d99e6d99bd454e83ab91ad5a87f993dc
#define GRID_d99e6d99bd454e83ab91ad5a87f993dc

// A multidimensional array capable of being indexed by Coords or individual
// dimensions. Currently used primarily to represent map data for the
// pathfinding abstraction.

#include <algorithm>
#include <cassert>
#include <vector>

#include "Coord.h"

namespace suncatcher {
namespace util {

//TODO: rewrite as a more cache-friendly backing, add 3D, and think about how
// to prevent excess capacity draining RAM we don't need.
template <typename T>
class Grid {
  public:
    inline Grid();

    inline Grid(const suncatcher::pathfinder::Coord size_in, const T& val);

    inline Grid(uint16_t r, uint16_t c, const T& val);

    inline void fill(const T& val);

    inline T& at(const suncatcher::pathfinder::Coord cell);
    inline const T& at(const suncatcher::pathfinder::Coord cell) const;

    inline T& at(uint16_t row, uint16_t col);
    inline const T& at(uint16_t row, uint16_t col) const;

    inline std::vector<suncatcher::pathfinder::Coord> get_adjacent(
        const suncatcher::pathfinder::Coord cell,
        bool include_diagonals=true
      ) const;

    inline bool check_bounds(const suncatcher::pathfinder::Coord cell) const;
    inline bool check_bounds(uint16_t row, uint16_t col) const;

    inline const suncatcher::pathfinder::Coord& size() const;

    inline bool operator==(const Grid<T>& other) const;

  private:
    suncatcher::pathfinder::Coord my_size;
    std::vector<T> backing;
};

}  // namespace util
}  // namespace suncatcher

#include "Grid-inl.h"

#endif  /* GRID_d99e6d99bd454e83ab91ad5a87f993dc */
