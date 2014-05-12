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

#ifndef EUCLIDEANGRAPH_272b6e8aad5b4590a75bf12e43dc1adc
#define EUCLIDEANGRAPH_272b6e8aad5b4590a75bf12e43dc1adc

#include "suncatcher.hh"
#include "suncatcher/platform.hh"
#include "suncatcher/graph/CowGrid.hh"
#include "suncatcher/graph/EuclideanCoord.hh"
#include "suncatcher/graph/EuclideanCoordRange.hh"

namespace suncatcher {

namespace test {
  class DeltaMap;
}  // namespace tes

namespace graph {

class EuclideanGraphBuilder;

class EuclideanGraph {
  public:
    EuclideanGraph();

    EuclideanGraph(EuclideanGraphBuilder&& builder);

    EuclideanGraph(
        EuclideanCoord chunks,
        EuclideanCoord chunk_size,
        uint8_t cost_i,
        int_least32_t color_i
      );


    inline float move_cost(
        const EuclideanCoord start,
        const EuclideanCoord finish
      ) const;

    inline uint_least8_t get_cost(const EuclideanCoord cell) const {
      return data.at(cell);
    }

    inline void set_cost(
        const EuclideanCoord cell,
        const uint_least8_t new_cost
      ) { data.at(cell) = new_cost; }

    inline int_least32_t get_color(const EuclideanCoord cell) const {
      return color.at(cell);
    }

    inline void set_color(
        const EuclideanCoord cell,
        const int_least32_t new_color
      ) { color.at(cell) = new_color; }

    inline EuclideanCoord size() const { return data.size(); }

    inline void fill_color(int_least32_t fill) { color.fill(fill); }

    inline  bool check_bounds(const EuclideanCoord cell) const {
      return data.check_bounds(cell);
    }

    inline void get_adjacent(
        const EuclideanCoord cell,
        bool include_diagonals,
        std::vector<EuclideanCoord>& adj
      ) const { data.get_adjacent(cell, include_diagonals, adj); }

    inline bool is_passable(EuclideanCoord cell) const {
      return (get_cost(cell) != PATH_COST_INFINITE);
    }

    inline EuclideanCoordRange domain() const;


  private:
    friend class test::DeltaMap;
    #ifdef EuclideanGridUseCOW
      // The map is divided into "colors" -- areas connected by purely transparent
      // terrain along Manhattan directions. Colors are handles into static
      // components (thus, we can union two colors in constant time at the price
      // of a small indirect lookup table). Typically there will be anywhere from
      // 1-5 colors per door on the map, plus one for each isolated walled off
      // region.
      CowGrid<int_least32_t> color;

      // Cost to traverse terrain.
      CowGrid<uint_least8_t> data;
    #else
      util::Grid<int_least32_t> color;
      util::Grid<uint_least8_t> data;
    #endif

};


}  // namespace graph
}  // namespace suncatcher

#include "suncatcher/graph/EuclideanGraph-inl.hh"

#endif  /* EUCLIDEANGRAPH_272b6e8aad5b4590a75bf12e43dc1adc */
