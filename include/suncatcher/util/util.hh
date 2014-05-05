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

#ifndef UTIL_116226f556b04bd5945185d15b5ea706
#define UTIL_116226f556b04bd5945185d15b5ea706

#include <cstdint>
#include <cmath>
#include <vector>

namespace suncatcher {

namespace pathfinder {
  class Coord;
}  // namespace pathfinder

namespace util {

inline int_least32_t find_representative(
    const std::vector<int_least32_t>& parents,
    int_least32_t elem
  ) {
  while (elem != parents[elem]) {
    elem = parents[elem];
  }
  return elem;
}

inline float manhattan(const pathfinder::Coord& a, const pathfinder::Coord& b) {
  return (float)pow(pow(std::abs((float)a.row - (float)b.row), 2) + pow(std::abs((float)a.col - (float)b.col), 2), 0.5);
}

}  // namespace util
}  // namespace suncatcher

#endif  /* UTIL_116226f556b04bd5945185d15b5ea706 */
