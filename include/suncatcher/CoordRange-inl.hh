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
namespace pathfinder {



inline CoordRange::CoordRange(Coord c)
: range_size(c) { }


inline Coord CoordRange::iterator::operator*() const {
  return pos;
}


inline CoordRange::iterator& CoordRange::iterator::operator++() {
  if (++pos.col == size.col) {
    pos.col = 0;
    if (++pos.row == size.row) {
      pos.row = 0;
      ++pos.layer;
    }
  }
  return *this;
}


inline CoordRange::iterator CoordRange::iterator::operator+(size_t offset) {
  size_t new_col = pos.col + offset;
  size_t new_row = pos.row + new_col / size.col;
  iterator rval(*this);
  rval.pos.col = new_col % size.col;
  rval.pos.row = new_row % size.row;
  new_row /= size.row;
  if (rval.pos.layer + new_row < rval.pos.layer) {
    return iterator(Coord(0, 0, size.layer), size);
  } else {
    rval.pos.layer += new_row / size.row;
    return rval;
  }
}


inline CoordRange::iterator::iterator(Coord pos_i, Coord size_i)
: pos(pos_i),
  size(size_i) { }


inline bool CoordRange::iterator::operator!=(
    const CoordRange::iterator& it
  ) const {
  return pos != it.pos || size != it.size;
}


inline CoordRange::iterator CoordRange::begin() {
  return iterator(Coord(0, 0, 0), range_size);
}


inline CoordRange::iterator CoordRange::end() {
  if (range_size.row == 0 || range_size.col == 0 || range_size.layer == 0) {
    return begin();
  } else {
    return iterator(Coord(0, 0, range_size.layer), range_size);
  }
}


inline size_t CoordRange::size() const {
  return range_size.row * range_size.col * range_size.layer;
}


inline Coord CoordRange::euclidean_size() const {
  return range_size;
}


}  // namespace pathfinder
}  // namespace suncatcher

#endif  /* COORDRANGE_517054c801a0440dbd2dd96476f6b6e1 */
