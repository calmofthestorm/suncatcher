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

#include "suncatcher/graph/EuclideanCoord.hh"

namespace suncatcher {
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

inline float manhattan(const graph::EuclideanCoord& a, const graph::EuclideanCoord& b) {
  return (float)pow(pow(std::abs((float)a.row - (float)b.row), 2) + pow(std::abs((float)a.col - (float)b.col), 2), 0.5);
}


inline bool check_bounds(const graph::EuclideanCoord cell, const graph::EuclideanCoord size)  {
  return (cell.row < size.row && cell.col < size.col && cell.layer < size.layer);
}


inline void get_adjacent(
    const graph::EuclideanCoord cell,
    bool include_diagonals,
    std::vector<graph::EuclideanCoord>& neighbors,
    const graph::EuclideanCoord size
    ) {

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
    if (check_bounds(n, size)) {
      neighbors.push_back(n);
    }
  }
}


}  // namespace util
}  // namespace suncatcher

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique(Args&& ...args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#endif  /* UTIL_116226f556b04bd5945185d15b5ea706 */
