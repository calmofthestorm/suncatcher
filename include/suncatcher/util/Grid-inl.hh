// This file is part of Suncatcher
// Alex Roper <alex@aroper.net>
//
// Suncatcher is free software: you can redistribute it and/or modify it under
// the terms of version 3 of the GNU General Public License as published by the
// Free Software Foundation.
//
// Suncatcher is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// Suncatcher.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2014 Alex Roper

#ifndef GRID_INL_93b27f51b9f84797b7b0b059118d798a
#define GRID_INL_93b27f51b9f84797b7b0b059118d798a

#include <type_traits>

namespace suncatcher {
namespace util {

template <typename T>
inline Grid<T>::Grid()
: my_size({0, 0, 0}),
  backing() { }


template <typename T>
inline Grid<T>::Grid(const pathfinder::Coord size_in, const T& val)
: my_size(size_in),
  backing(size_in.row * size_in.col * size_in.layer, val) { }


template <typename T>
inline Grid<T>::Grid(uint16_t r, uint16_t c, uint16_t l, const T& val)
  : Grid({r, c, l}, val) { }


template <typename T>
inline void Grid<T>::fill(const T& val) {
  std::fill(backing.begin(), backing.end(), val);
}


template <typename T>
inline T& Grid<T>::at(const pathfinder::Coord cell) {
  return at(cell.row, cell.col, cell.layer);
}


template <typename T>
inline const T& Grid<T>::at(const pathfinder::Coord cell) const {
  return at(cell.row, cell.col, cell.layer);
}


template <typename T>
inline const T& Grid<T>::at(uint16_t row, uint16_t col, uint16_t layer) const {
  assert(check_bounds(row, col, layer));
  return backing[col + my_size.col * (row + my_size.row * layer)];
}


template <typename T>
inline T& Grid<T>::at(uint16_t row, uint16_t col, uint16_t layer) {
  assert(check_bounds(row, col, layer));
  return backing[col + my_size.col * (row + my_size.row * layer)];
}


//TODO: evaluate cache performance of ordering.
template <typename T>
inline std::vector<pathfinder::Coord> Grid<T>::get_adjacent(
    const pathfinder::Coord cell,
    bool include_diagonals
    ) const {

  using pathfinder::Coord;
  static_assert(
      std::is_same<decltype(Coord::row), decltype(Coord::col)>::value,
      ""
    );
  static_assert(
      std::is_same<decltype(Coord::row), decltype(Coord::layer)>::value,
      ""
    );

  const static decltype(Coord::row) n1(-1);

  const static std::vector<Coord> ADJ_DELTA{
    // Manhattan adjacent same layer.
    {0, n1, 0}, {n1, 0, 0}, {0, 1, 0}, {1, 0, 0},

    // Manhattan adjacent vertical and horizontal.
    {0, 0, 1}, {0, 0, n1},

    // Same layer diagonal.
    {1, 1, 0}, {n1, 1, 0}, {1, n1, 0}, {n1, n1, 0}
  };

  std::vector<Coord> neighbors;

  for (size_t i = 0; i < (size_t)(include_diagonals ? 10 : 6); ++i) {
    auto n = ADJ_DELTA[i] + cell;
    if (check_bounds(n)) {
      neighbors.push_back(n);
    }
  }

  return neighbors;
}


template <typename T>
inline bool Grid<T>::check_bounds(const pathfinder::Coord cell) const {
  return check_bounds(cell.row, cell.col, cell.layer);
}


template <typename T>
inline bool Grid<T>::check_bounds(
    uint16_t row,
    uint16_t col,
    uint16_t layer
  ) const {

  return (row < my_size.row && col < my_size.col && layer < my_size.layer);
}


template <typename T>
inline const pathfinder::Coord& Grid<T>::size() const {
  return my_size;
}


template <typename T>
inline bool Grid<T>::operator==(const Grid<T>& other) const {
  return (other.size() == size() && other.backing == backing);
}


template <typename T>
inline const T& Grid<T>::operator[](const pathfinder::Coord cell) const {
  return at(cell);
}


template <typename T>
inline T& Grid<T>::operator[](const pathfinder::Coord cell) {
  return at(cell);
}



}  // namespace util
}  // namespace suncatcher

#endif  /* GRID_INL_93b27f51b9f84797b7b0b059118d798a */
