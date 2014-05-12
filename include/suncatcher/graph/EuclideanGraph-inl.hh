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

#ifndef EUCLIDEANGRAPH_INL_d2099117aab245828a839e6cde411290
#define EUCLIDEANGRAPH_INL_d2099117aab245828a839e6cde411290

namespace suncatcher {
namespace graph {


inline float EuclideanGraph::move_cost(
    const EuclideanCoord start,
    const EuclideanCoord finish
  ) const {

  assert(check_bounds(start));
  assert(check_bounds(finish));
  assert(std::abs(start.row - finish.row) <= 1 &&
      std::abs(start.col - finish.col) <= 1 &&
      std::abs(start.layer - finish.layer) <= 1);

  if (!is_passable(finish) || !is_passable(start)) {
    return -1;
  }

  if (start == finish) {
    return 0;
  }

  if (start.layer != finish.layer) {
    if (start.row == finish.row && start.col == finish.col) {
      return get_cost(finish);
    } else {
      return -1;
    }
  }

  if (start.row == finish.row || start.col == finish.col) {
    return get_cost(finish);
  }

  assert(start.layer == finish.layer);
  // Can only move diagonally if Manhattan squares are passable.
  if (is_passable({start.row, finish.col, start.layer}) ||
      is_passable({finish.row, start.col, start.layer})) {
    return get_cost(finish) * static_cast<float>(1.4142135623730951);
  } else {
    return -1;
  }
}


inline EuclideanCoordRange EuclideanGraph::domain() const {
  return EuclideanCoordRange(size());
}

}  // namespace graph
}  // namespace suncatcher

#endif  /* EUCLIDEANGRAPH_INL_d2099117aab245828a839e6cde411290 */
