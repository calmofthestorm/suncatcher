#ifndef GRID_d99e6d99bd454e83ab91ad5a87f993dc
#define GRID_d99e6d99bd454e83ab91ad5a87f993dc

// A multidimensional array capable of being indexed by Coords or individual
// dimensions. Currently used primarily to represent map data for the
// pathfinding abstraction.

#include "Coord.h"

#include <vector>

namespace suncatcher {
namespace util {

template <typename T>
class Grid {
  public:
    Grid();

    Grid(uint16_t r, uint16_t c, const T& val);

    explicit Grid(std::vector<std::vector<T>>&& backing_in);

    const std::vector<std::vector<T>>& get_backing() const;

    std::vector<std::vector<T>>& get_backing();

    T& at(const suncatcher::pathfinder::Coord& cell);

    const T& at(const suncatcher::pathfinder::Coord& cell) const;

    const T& at(uint16_t row, uint16_t col) const;

    T& at(uint16_t row, uint16_t col);

    std::vector<suncatcher::pathfinder::Coord> get_adjacent(
        const suncatcher::pathfinder::Coord& cel
      ) const;

  private:
    std::vector<std::vector<T>> backing;
};

#include "Grid.tcc"

}  // namespace  util
}  // namespace suncatcher

#endif  /* GRID_d99e6d99bd454e83ab91ad5a87f993dc */
