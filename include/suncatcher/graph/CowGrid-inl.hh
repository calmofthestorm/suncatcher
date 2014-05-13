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

#ifndef COWGRID_INL_fb44c918a6494e499f928618b60b067f
#define COWGRID_INL_fb44c918a6494e499f928618b60b067f


namespace suncatcher {
namespace graph {


template <typename T>
inline CowGrid<T>::CowGrid(
    const EuclideanCoord size_in_chunks,
    const EuclideanCoord chunk_size_i,
    const T& fill_value
  )
: chunk_size(chunk_size_i),
  full_size(
      size_in_chunks.row * chunk_size_i.row,
      size_in_chunks.col * chunk_size_i.col,
      size_in_chunks.layer * chunk_size_i.layer
    ),
  chunks(size_in_chunks, std::shared_ptr<util::Grid<T>>(nullptr)) {

  for (size_t i = 0; i < chunks.length; ++i) {
    chunks.backing[i] = std::make_shared<util::Grid<T>>(chunk_size, fill_value);
  }
}


template <typename T>
inline void CowGrid<T>::fill(const T& val) {
  for (size_t i = 0; i < chunks.length; ++i) {
    if (!chunks.backing[i].unique()) {
      chunks.backing[i] = std::make_shared<util::Grid<T>>(chunk_size, val);
    } else {
      chunks.backing[i]->fill(val);
    }
  }
}


template <typename T>
inline T& CowGrid<T>::at(const EuclideanCoord cell) {
  EuclideanCoord chunk_index{(uint16_t)(cell.row / chunk_size.row),
                    (uint16_t)(cell.col / chunk_size.col),
                    (uint16_t)(cell.layer / chunk_size.layer)};
  EuclideanCoord inner{(uint16_t)(cell.row % chunk_size.row),
              (uint16_t)(cell.col % chunk_size.col),
              (uint16_t)(cell.layer % chunk_size.layer)};
  check_bounds(cell);
  chunks.check_bounds(chunk_index);
  if (!chunks.at(chunk_index).unique()) {
    chunks.at(chunk_index)= std::make_shared<util::Grid<T>>(*chunks.at(chunk_index));
  }
  chunks.at(chunk_index)->check_bounds(inner);
  assert(chunks.at(chunk_index).unique());
  return chunks.at(chunk_index)->at(inner);
}


template <typename T>
inline const T& CowGrid<T>::at(const EuclideanCoord cell) const {
  EuclideanCoord chunk_index{(uint16_t)(cell.row / chunk_size.row),
                    (uint16_t)(cell.col / chunk_size.col),
                    (uint16_t)(cell.layer / chunk_size.layer)};
  EuclideanCoord inner{(uint16_t)(cell.row % chunk_size.row),
              (uint16_t)(cell.col % chunk_size.col),
              (uint16_t)(cell.layer % chunk_size.layer)};
  check_bounds(cell);
  chunks.check_bounds(chunk_index);
  chunks.at(chunk_index)->check_bounds(inner);
  return chunks.at(chunk_index)->at(inner);
}


template <typename T>
inline void CowGrid<T>::get_adjacent(
    const EuclideanCoord cell,
    bool include_diagonals,
    std::vector<EuclideanCoord>& neighbors
    ) const {

  util::get_adjacent(cell, include_diagonals, neighbors, full_size);
}


template <typename T>
inline bool CowGrid<T>::check_bounds(const EuclideanCoord cell) const {
  return util::check_bounds(cell, full_size);
}


template <typename T>
inline const EuclideanCoord& CowGrid<T>::size() const {
  return full_size;
}


template <typename T>
inline const T& CowGrid<T>::operator[](const EuclideanCoord cell) const {
  return at(cell);
}


template <typename T>
inline T& CowGrid<T>::operator[](const EuclideanCoord cell) {
  return at(cell);
}


template <typename T>
inline bool CowGrid<T>::operator==(const CowGrid<T>& other) const {
  if (full_size != other.full_size) {
    return false;
  }
  assert(chunk_size == other.chunk_size);
  for (size_t i = 0; i < chunks.length; ++i) {
    if (chunks.backing[i] != other.chunks.backing[i] &&
        !(*chunks.backing[i] == *other.chunks.backing[i])) {
      return false;
    }
  }
  return true;
}



}  // namespace graph
}  // namespace suncatcher

#endif  /* COWGRID_INL_fb44c918a6494e499f928618b60b067f */
