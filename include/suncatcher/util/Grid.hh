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

// A multidimensional array capable of being indexed by Coords or individual
// dimensions. Currently used primarily to represent map data for the
// pathfinding abstraction.

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <vector>

#include "suncatcher/Coord.hh"

namespace suncatcher {
namespace util {

//TODO: rewrite as a more cache-friendly backing, add 3D, and think about how
// to prevent excess capacity draining RAM we don't need.
template <typename T>
class Grid {
  public:
    inline Grid();

    inline Grid(const Coord size_in, const T& val);

    inline Grid(uint16_t row, uint16_t col, uint16_t layer, const T& val);

    inline void fill(const T& val);

    inline T& at(const Coord cell);
    inline const T& at(const Coord cell) const;

    inline T& at(uint16_t row, uint16_t col, uint16_t layer);
    inline const T& at(uint16_t row, uint16_t col, uint16_t layer) const;

    inline const T& operator[](const Coord cell) const;
    inline T& operator[](const Coord cell);

    inline void get_adjacent(
        const Coord cell,
        bool include_diagonals,
        std::vector<Coord>& adj
      ) const;

    inline bool check_bounds(const Coord cell) const;
    inline bool check_bounds(uint16_t row, uint16_t col, uint16_t layer) const;

    inline const Coord& size() const;

    inline bool operator==(const Grid<T>& other) const;

  private:
    Coord my_size;
    std::vector<T> backing;
};

}  // namespace util
}  // namespace suncatcher

#include "suncatcher/util/Grid-inl.hh"

#endif  /* GRID_d99e6d99bd454e83ab91ad5a87f993dc */
