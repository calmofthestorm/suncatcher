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

#ifndef COORD_d0dcb74311134c19b0239c6c7238cfad
#define COORD_d0dcb74311134c19b0239c6c7238cfad

#include <cstdint>
#include <iostream>

#include <boost/functional/hash.hpp>
#include <limits>
#include <type_traits>

#include "suncatcher/platform.hh"

namespace suncatcher {
namespace graph {


// POD class representing a position on the pathfinding map abstraction.
class EuclideanCoord {
  public:
    inline EuclideanCoord()
    : row(0),
      col(0),
      layer(0) { }

    inline EuclideanCoord(uint16_t row_i, uint16_t col_i, uint16_t layer_i)
    : row(row_i),
      col(col_i),
      layer(layer_i) { }

    uint16_t row, col, layer;

    inline bool operator!= (const EuclideanCoord& other) const {
      return to_number() != other.to_number();
      return row != other.row || col != other.col || layer != other.layer;
    }

    inline bool operator== (const EuclideanCoord& other) const {
      return to_number() == other.to_number();
    }

    inline bool operator< (const EuclideanCoord& other) const {
      return to_number() < other.to_number();
    }

    inline EuclideanCoord operator+ (const EuclideanCoord& other) const {
      EuclideanCoord r(*this);
      r.row += other.row;
      r.col += other.col;
      r.layer += other.layer;
      return r;
    }

  private:
    inline uint64_t to_number() const {
      // Make sure my logic works here.
      static_assert(std::is_same<decltype(row), decltype(col)>::value, "");
      static_assert(std::is_same<decltype(row), decltype(layer)>::value, "");
      static_assert(sizeof(uint64_t) >= 3 * sizeof(decltype(row)), "");

      static const uint64_t coord_max = std::numeric_limits<decltype(row)>::max();
      return ((uint64_t)col + coord_max *
              ((uint64_t)row + coord_max * (uint64_t)layer));
    }
} ALIGNED_8;

inline std::ostream& operator<< (std::ostream& os, const EuclideanCoord& c) {
  os << '(' << c.row << ", " << c.col << ", " << c.layer << ')';
  return os;
}


}  // namespace pathfinder
}  // namespace suncatcher



namespace std {



template <>
struct hash<suncatcher::graph::EuclideanCoord> {
  size_t operator() (const suncatcher::graph::EuclideanCoord& key) const {
    size_t seed = 0;
    boost::hash_combine(seed, key.row);
    boost::hash_combine(seed, key.col);
    return seed;
  }
};



}


#endif  /* COORD_d0dcb74311134c19b0239c6c7238cfad */
