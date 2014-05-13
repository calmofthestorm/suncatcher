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
  length(0),
  backing(nullptr) { }


template <typename T>
inline Grid<T>::Grid(const graph::EuclideanCoord size_in, const T& val)
: my_size(size_in),
  length(size_in.row * size_in.col * size_in.layer),
  backing(new T[size_in.row * size_in.col * size_in.layer]) {

  fill(val);
}


template <typename T>
inline Grid<T>::Grid(const graph::EuclideanCoord chunks, const graph::EuclideanCoord chunk_size, const T& val)
: Grid(graph::EuclideanCoord(chunks.row * chunk_size.row, chunks.col * chunk_size.col, chunks.layer * chunk_size.layer), val) { }


template <typename T>
inline Grid<T>& Grid<T>::operator=(const Grid<T>& other) {
  my_size = other.my_size;
  if (length != other.length) {
    length = other.length;
    backing.reset(new T[other.length]);
  }

  for (size_t i = 0; i < length; ++i) {
    backing[i] = other.backing[i];
  }
  return *this;
}


template <typename T>
inline Grid<T>::Grid(const Grid<T>& other)
: my_size(other.my_size),
  length(other.length),
  backing(new T[other.length]) {

  for (size_t i = 0; i < length; ++i) {
    backing[i] = other.backing[i];
  }
}


template <typename T>
inline void Grid<T>::fill(const T& val) {
  for (size_t i = 0; i < length; ++i) {
    backing[i] = val;
  }
}


template <typename T>
inline T& Grid<T>::at(const graph::EuclideanCoord cell) {
  check_bounds(cell);
  size_t index = cell.col + my_size.col * (cell.row + my_size.row * cell.layer);
  return backing[index];
}


template <typename T>
inline const T& Grid<T>::at(const graph::EuclideanCoord cell) const {
  check_bounds(cell);
  size_t index = cell.col + my_size.col * (cell.row + my_size.row * cell.layer);
  return backing[index];
}


//TODO: evaluate cache performance of ordering.
template <typename T>
inline void Grid<T>::get_adjacent(
    const graph::EuclideanCoord cell,
    bool include_diagonals,
    std::vector<graph::EuclideanCoord>& neighbors
    ) const {

  neighbors.clear();
  neighbors.reserve(10);

  static_assert(
      std::is_same<decltype(graph::EuclideanCoord::row), decltype(graph::EuclideanCoord::col)>::value,
      ""
    );
  static_assert(
      std::is_same<decltype(graph::EuclideanCoord::row), decltype(graph::EuclideanCoord::layer)>::value,
      ""
    );

  const static decltype(graph::EuclideanCoord::row) n1(-1);

  const static std::vector<graph::EuclideanCoord> ADJ_DELTA{
    // Manhattan adjacent same layer.
    {0, n1, 0}, {n1, 0, 0}, {0, 1, 0}, {1, 0, 0},

    // Manhattan adjacent vertical and horizontal.
    {0, 0, 1}, {0, 0, n1},

    // Same layer diagonal.
    {1, 1, 0}, {n1, 1, 0}, {1, n1, 0}, {n1, n1, 0}
  };


  for (size_t i = 0; i < (size_t)(include_diagonals ? 10 : 6); ++i) {
    auto n = ADJ_DELTA[i] + cell;
    if (check_bounds(n)) {
      neighbors.push_back(n);
    }
  }
}


template <typename T>
inline bool Grid<T>::check_bounds(const graph::EuclideanCoord cell) const {
  return (cell.row < my_size.row && cell.col < my_size.col && cell.layer < my_size.layer);
}


template <typename T>
inline const graph::EuclideanCoord& Grid<T>::size() const {
  return my_size;
}


template <typename T>
inline bool Grid<T>::operator==(const Grid<T>& other) const {
  if (other.size() != size()) {
    return false;
  }
  for (size_t i = 0; i < length; ++i) {
    if (other.backing[i] != backing[i]) {
      return false;
    }
  }
  return true;
}


template <typename T>
inline const T& Grid<T>::operator[](const graph::EuclideanCoord cell) const {
  return at(cell);
}


template <typename T>
inline T& Grid<T>::operator[](const graph::EuclideanCoord cell) {
  return at(cell);
}



}  // namespace util
}  // namespace suncatcher

#endif  /* GRID_INL_93b27f51b9f84797b7b0b059118d798a */
