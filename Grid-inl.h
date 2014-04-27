#ifndef GRID_INL_93b27f51b9f84797b7b0b059118d798a
#define GRID_INL_93b27f51b9f84797b7b0b059118d798a

namespace suncatcher {
namespace util {

template <typename T>
inline Grid<T>::Grid()
: my_size({0, 0}),
  backing() { }

template <typename T>
inline Grid<T>::Grid(const suncatcher::pathfinder::Coord size_in, const T& val)
: my_size(size_in),
  backing(size_in.row * size_in.col, val) { }

template <typename T>
inline Grid<T>::Grid(uint16_t r, uint16_t c, const T& val)
  : Grid({r, c}, val) { }

template <typename T>
inline void Grid<T>::fill(const T& val) {
  std::fill(backing.begin(), backing.end(), val);
}

template <typename T>
inline T& Grid<T>::at(const suncatcher::pathfinder::Coord cell) {
  return at(cell.row, cell.col);
}

template <typename T>
inline const T& Grid<T>::at(const suncatcher::pathfinder::Coord cell) const {
  return at(cell.row, cell.col);
}

template <typename T>
inline const T& Grid<T>::at(uint16_t row, uint16_t col) const {
  assert(check_bounds(row, col));
  return backing[row * my_size.col + col];
}

template <typename T>
inline T& Grid<T>::at(uint16_t row, uint16_t col) {
  assert(check_bounds(row, col));
  return backing[row * my_size.col + col];
}

//TODO: evaluate cache performance of ordering.
template <typename T>
inline std::vector<suncatcher::pathfinder::Coord> Grid<T>::get_adjacent(
    const suncatcher::pathfinder::Coord cell
    ) const {
  const static std::vector<suncatcher::pathfinder::Coord> ADJ_DELTA{
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

template <typename T>
inline bool Grid<T>::check_bounds(const suncatcher::pathfinder::Coord cell) const {
  return check_bounds(cell.row, cell.col);
}

template <typename T>
inline bool Grid<T>::check_bounds(uint16_t row, uint16_t col) const {
  return (row < my_size.row && col < my_size.col);
}

template <typename T>
inline const suncatcher::pathfinder::Coord& Grid<T>::size() const {
  return my_size;
}

}  // namespace util
}  // namespace suncatcher

#endif  /* GRID_INL_93b27f51b9f84797b7b0b059118d798a */
