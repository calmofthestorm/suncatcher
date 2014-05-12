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

#ifndef EUCLIDEANPATHERSTATE_INL_8b224194ef3c4a01b8b7ebf0923acfca
#define EUCLIDEANPATHERSTATE_INL_8b224194ef3c4a01b8b7ebf0923acfca

#include <limits>

namespace suncatcher {
namespace graph {


const static uint8_t MAX_VERSION = std::numeric_limits<uint8_t>::max();

inline EuclideanPatherState::EuclideanPatherState(EuclideanCoord size)
: version(size, 0),
  expanded(size, false),
  distance(size, INFINITY),
  previous(size, Coord()),
  current(1) { }


inline bool EuclideanPatherState::get_expanded(EuclideanCoord cell) const {
  if (version[cell] < current || (version[cell] == MAX_VERSION && current == 0)) {
    return false;
  } else {
    return expanded[cell];
  }
}


inline void EuclideanPatherState::set_expanded(EuclideanCoord cell, bool value) {
  if (version[cell] < current || (version[cell] == MAX_VERSION && current == 0)) {
    distance[cell] = INFINITY;
    version[cell] = current;
  }
  expanded[cell] = value;
}


inline float EuclideanPatherState::get_distance(EuclideanCoord cell) const {
  if (version[cell] < current || (version[cell] == MAX_VERSION && current == 0)) {
    return INFINITY;
  } else {
    return distance[cell];
  }
}


inline void EuclideanPatherState::set_distance(EuclideanCoord cell, float value) {
  if (version[cell] < current || (version[cell] == MAX_VERSION && current == 0)) {
    expanded[cell] = false;
    version[cell] = current;
  }
  distance[cell] = value;
}


inline EuclideanCoord EuclideanPatherState::get_previous(EuclideanCoord cell) const {
  assert(!(version[cell] < current || (version[cell] == MAX_VERSION && current == 0)));
  return previous[cell];
}


inline void EuclideanPatherState::set_previous(EuclideanCoord cell, EuclideanCoord value) {
  if (version[cell] < current || (version[cell] == MAX_VERSION && current == 0)) {
    expanded[cell] = false;
    distance[cell] = INFINITY;
    version[cell] = current;
  }
  previous[cell] = value;
}


inline void EuclideanPatherState::clear() {
  if (++current == 0) {
    current = 1;
    version.fill(0);
    expanded.fill(false);
    distance.fill(INFINITY);
  }
}


}  // namespace graph
}  // namespace suncatcher



#endif  /* EUCLIDEANPATHERSTATE_INL_8b224194ef3c4a01b8b7ebf0923acfca */
