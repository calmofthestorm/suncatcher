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

#ifndef GRID_d99e6d99bd454e83ab91ad5a87f993dc
#define GRID_d99e6d99bd454e83ab91ad5a87f993dc

// TODO: move to graph ns since this is euclidean

// A multidimensional array capable of being indexed by Coords or individual
// dimensions. Currently used primarily to represent map data for the
// pathfinding abstraction.

#include <algorithm>
#include <memory>
#include <cassert>
#include <cstdint>
#include <vector>

#include "suncatcher/graph/EuclideanCoord.hh"

namespace suncatcher {

namespace graph {
  template <typename T>class CowGrid;
}  // namespace graph

namespace util {

//TODO: rewrite as a more cache-friendly backing, add 3D, and think about how
// to prevent excess capacity draining RAM we don't need.
template <typename T>
class Grid {
  public:
    inline Grid();

    inline Grid(const Grid& other);
    inline Grid& operator=(const Grid& other);

    inline Grid(const graph::EuclideanCoord size_in, const T& val);
    inline Grid(const graph::EuclideanCoord chunks, const graph::EuclideanCoord chunk_size, const T& val);

    inline void fill(const T& val);

    inline T& at(const graph::EuclideanCoord cell);
    inline const T& at(const graph::EuclideanCoord cell) const;

    inline T& operator[](const graph::EuclideanCoord cell);
    inline const T& operator[](const graph::EuclideanCoord cell) const;

    inline void get_adjacent(
        const graph::EuclideanCoord cell,
        bool include_diagonals,
        std::vector<graph::EuclideanCoord>& adj
      ) const;

    inline bool check_bounds(const graph::EuclideanCoord cell) const;

    inline const graph::EuclideanCoord& size() const;

    inline bool operator==(const Grid<T>& other) const;

  // private:
    friend class graph::CowGrid<T>;
    graph::EuclideanCoord my_size;
    size_t length;
    std::unique_ptr<T[]> backing;
};

}  // namespace util
}  // namespace suncatcher

#include "suncatcher/util/Grid-inl.hh"

#endif  /* GRID_d99e6d99bd454e83ab91ad5a87f993dc */
