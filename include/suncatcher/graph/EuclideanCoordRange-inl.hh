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

#ifndef COORDRANGE_517054c801a0440dbd2dd96476f6b6e1
#define COORDRANGE_517054c801a0440dbd2dd96476f6b6e1

namespace suncatcher {
namespace graph {

inline EuclideanCoordRange::const_iterator::const_iterator(EuclideanCoord start, EuclideanCoord size_i)
: pos(start),
  size(size_i) { }


inline EuclideanCoordRange::const_iterator::self_type EuclideanCoordRange::const_iterator::operator++() {
  if (++pos.col == size.col) {
    pos.col = 0;
    if (++pos.row == size.row) {
      pos.row = 0;
      ++pos.layer;
    }
  }
  return *this;
}


inline EuclideanCoordRange::const_iterator::self_type EuclideanCoordRange::const_iterator::operator++(int) {
  EuclideanCoordRange::const_iterator::self_type old = *this;
  ++(*this);
  return old;
}


inline const EuclideanCoordRange::const_iterator::value_type& EuclideanCoordRange::const_iterator::operator*() const {
  return pos;
}


inline bool EuclideanCoordRange::const_iterator::operator==(const EuclideanCoordRange::const_iterator::self_type& rhs) const {
  return pos == rhs.pos && size == rhs.size;
}


inline bool EuclideanCoordRange::const_iterator::operator!=(const EuclideanCoordRange::const_iterator::self_type& rhs) const {
  return !(rhs == *this);
}


inline EuclideanCoordRange::EuclideanCoordRange(EuclideanCoord c)
: range_size(c) { }


inline EuclideanCoordRange::const_iterator EuclideanCoordRange::begin() const {
  return const_iterator(EuclideanCoord(0, 0, 0), range_size);
}


inline EuclideanCoordRange::const_iterator EuclideanCoordRange::end() const {
  if (range_size.row == 0 || range_size.col == 0 || range_size.layer == 0) {
    return begin();
  } else {
    return const_iterator(EuclideanCoord(0, 0, range_size.layer), range_size);
  }
}


inline size_t EuclideanCoordRange::size() const {
  return range_size.row * range_size.col * range_size.layer;
}


inline EuclideanCoord EuclideanCoordRange::euclidean_size() const {
  return range_size;
}


inline EuclideanCoord EuclideanCoordRange::get_coord_by_index(size_t index) const {
  size_t layer = index / (range_size.row * range_size.col);
  index %= range_size.row * range_size.col;
  size_t row = index / range_size.col;
  size_t col = index % range_size.col;
  return EuclideanCoord((uint16_t)row, (uint16_t)col, (uint16_t)layer);
}


inline size_t EuclideanCoordRange::get_index_by_coord(EuclideanCoord cell) const {
  size_t acc = cell.layer;
  acc *= range_size.row;
  acc += cell.row;
  acc *= range_size.col;
  acc += cell.col;
  return acc;
}


}  // namespace graph
}  // namespace suncatcher

#endif  /* COORDRANGE_517054c801a0440dbd2dd96476f6b6e1 */
