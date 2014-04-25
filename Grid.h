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
    inline Grid()
    : backing(),
      my_size({0, 0}) { }

    inline Grid(const suncatcher::pathfinder::Coord& size_in, const T& val) {
      resize(size_in, val);
    }

    inline Grid(uint16_t r, uint16_t c, const T& val) {
      resize(r, c, val);
    }

    explicit inline Grid(
        std::vector<std::vector<T>>&& backing_in,
        const suncatcher::pathfinder::Coord& size_in
      )
      : backing(backing_in),
        my_size(size_in) { }

    inline void fill(const T& val) {
      for (auto& row : backing) {
        std::fill(row.begin(), row.end(), val);
      }
    }

    inline void resize(const suncatcher::pathfinder::Coord& size_in, const T& val) {
      my_size = size_in;
      backing.clear();
      backing.resize(my_size.row, std::vector<T>(my_size.col, val));
    }

    inline void resize(uint16_t r, uint16_t c, const T& val) {
      backing.clear();
      backing.resize(r, std::vector<T>(c, val));
      my_size = {r, c};
    }

    inline T& at(const suncatcher::pathfinder::Coord& cell) {
      assert(check_bounds(cell));
      return backing[cell.row][cell.col];
    }

    inline const T& at(const suncatcher::pathfinder::Coord& cell) const {
      assert(check_bounds(cell));
      return backing[cell.row][cell.col];
    }

    inline const T& at(uint16_t row, uint16_t col) const {
      assert(check_bounds(row, col));
      return backing[row][col];
    }

    inline T& at(uint16_t row, uint16_t col) {
      assert(check_bounds(row, col));
      return backing[row][col];
    }

    //TODO: evaluate cache performance of ordering.
    inline std::vector<suncatcher::pathfinder::Coord> get_adjacent(
        const suncatcher::pathfinder::Coord& cell
        ) const {
      static const std::vector<suncatcher::pathfinder::Coord> ADJ_DELTA{
        {0, (uint16_t)-1}, {(uint16_t)-1, 0}, {1, 0},
          {(uint16_t)-1, (uint16_t)-1}, {1, 1},
          {(uint16_t)-1, 1}, {1, (uint16_t)-1}, {0, 1}
      };

      std::vector<suncatcher::pathfinder::Coord> neighbors;

      for (const auto& delta : ADJ_DELTA) {
        auto n = delta + cell;
        if (check_bounds(n)) {
          neighbors.push_back(n);
        }
      }

      return neighbors;
    }

    inline bool check_bounds(const suncatcher::pathfinder::Coord& cell) const {
      return (cell.row < my_size.row && cell.col < my_size.col);
    }

    inline bool check_bounds(uint16_t row, uint16_t col) const {
      return (row < my_size.row && col < my_size.col);
    }

    inline const suncatcher::pathfinder::Coord& size() const {
      return my_size;
    }

  private:
    std::vector<std::vector<T>> backing;
    suncatcher::pathfinder::Coord my_size;
};

}  // namespace util
}  // namespace suncatcher

#endif  /* GRID_d99e6d99bd454e83ab91ad5a87f993dc */
