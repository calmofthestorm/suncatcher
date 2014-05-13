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

#ifndef COWGRID_0de968617d4d4890833675ae6952afe5
#define COWGRID_0de968617d4d4890833675ae6952afe5

#include <memory>

#include "suncatcher/util/Grid.hh"
#include "suncatcher/graph/EuclideanCoord.hh"

namespace suncatcher {
namespace graph {

template <typename T>
class CowGrid {
  public:
    inline CowGrid() = default;

    inline CowGrid(
        const EuclideanCoord size_in_chunks,
        const EuclideanCoord chunk_size,
        const T& fill_value
      );

    inline void fill(const T& val);

    inline T& at(const EuclideanCoord cell);
    inline const T& at(const EuclideanCoord cell) const;

    inline T& operator[](const EuclideanCoord cell);
    inline const T& operator[](const EuclideanCoord cell) const;

    inline void get_adjacent(
        const EuclideanCoord cell,
        bool include_diagonals,
        std::vector<EuclideanCoord>& adj
      ) const;

    inline bool check_bounds(const EuclideanCoord cell) const;

    inline const EuclideanCoord& size() const;

    inline bool operator==(const CowGrid<T>& other) const;

  private:
    inline void dirty_chunk(const EuclideanCoord chunk);
    EuclideanCoord chunk_size, full_size;
    util::Grid<std::shared_ptr<util::Grid<T>>> chunks;
  
};

}  // namespace graph
}  // namespace suncatcher

#include "suncatcher/graph/CowGrid-inl.hh"

#endif  /* COWGRID_0de968617d4d4890833675ae6952afe5 */
