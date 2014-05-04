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

#ifndef MAP_INL_c67ecba94fae4db889e83c228d138cd1
#define MAP_INL_c67ecba94fae4db889e83c228d138cd1

namespace suncatcher {
namespace pathfinder {

inline float Map::move_cost(Coord start, Coord finish) const {
  assert(data.check_bounds(start));
  assert(data.check_bounds(finish));
  assert(std::abs(start.row - finish.row) <= 1 &&
        std::abs(start.col - finish.col) <= 1);
  assert(is_passable(start));
  if (!is_passable(finish)) {
    return -1;
  }

  if (start == finish) {
    return 0;
  }

  if (start.row == finish.row || start.col == finish.col) {
    return data.at(finish);
  }

  // Can only move diagonally if Manhattan squares are passable.
  if (is_passable({start.row, finish.col}) || is_passable({finish.row, start.col})) {
    return data.at(finish) * (float)1.4142135623730951;
  } else {
    return -1;
  }
}

inline bool Map::is_passable(Coord cell) const {
  assert(data.check_bounds(cell));
  assert(color.at(cell) != COLOR_UNKNOWN);
  bool rval = (data.at(cell) != PATH_COST_INFINITE);
  #ifndef NDEBUG
    if (is_door(cell)) {
      assert(rval == doors.at(cell).open);
    } else {
      assert((color.at(cell) != COLOR_IMPASSABLE) == rval);
    }
    #endif
  return rval;
}

inline bool Map::is_transparent(Coord cell) const {
  assert(data.check_bounds(cell));
  return (!is_door(cell) && data.at(cell) != PATH_COST_INFINITE);
}

inline bool Map::is_door(Coord cell) const {
  int_least32_t c = color.at(cell);
  return (c != COLOR_IMPASSABLE && is_door(c));
}


inline bool Map::is_door(int_least32_t cell_color) const {
  return (cell_color < 0 && cell_color >= COLOR_LOWEST_VALID);
}



}  // namespace pathfinder
}  // namespace suncatcher

#endif  /* MAP_INL_c67ecba94fae4db889e83c228d138cd1 */
