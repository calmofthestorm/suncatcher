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

#include "suncatcher/util/util.hh"

namespace suncatcher {
namespace util {

template <typename T>
inline Grid<T>::Grid()
: my_size({0, 0, 0}),
  backing() { }


template <typename T>
inline Grid<T>::Grid(const Coord size_in, const T& val)
: my_size(size_in),
  backing(size_in.row * size_in.col * size_in.layer, val) { }


template <typename T>
inline void Grid<T>::fill(const T& val) {
  std::fill(backing.begin(), backing.end(), val);
}


template <typename T>
inline T& Grid<T>::at(const Coord cell) {
  assert(check_bounds(cell));
  return backing[cell.col + my_size.col * (cell.row + my_size.row * cell.layer)];
}


template <typename T>
inline const T& Grid<T>::at(const Coord cell) const {
  assert(check_bounds(cell));
  return backing[cell.col + my_size.col * (cell.row + my_size.row * cell.layer)];
}


template <typename T>
inline void Grid<T>::get_adjacent(
    const Coord cell,
    bool include_diagonals,
    std::vector<Coord>& neighbors
    ) const {

  util::get_adjacent(cell, include_diagonals, neighbors, my_size);
}


template <typename T>
inline bool Grid<T>::check_bounds(const Coord cell) const {
  return util::check_bounds(cell, my_size);
}


template <typename T>
inline const Coord& Grid<T>::size() const {
  return my_size;
}


template <typename T>
inline bool Grid<T>::operator==(const Grid<T>& other) const {
  return (other.size() == size() && other.backing == backing);
}


template <typename T>
inline const T& Grid<T>::operator[](const Coord cell) const {
  return at(cell);
}


template <typename T>
inline T& Grid<T>::operator[](const Coord cell) {
  return at(cell);
}



}  // namespace util
}  // namespace suncatcher

#endif  /* GRID_INL_93b27f51b9f84797b7b0b059118d798a */
