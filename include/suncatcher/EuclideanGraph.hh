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
#include "suncatcher/Coord.hh"
#include "suncatcher/util/Grid.hh"

namespace suncatcher {

namespace test {
  class DeltaMap;
}  // namespace tes

namespace graph {



class EuclideanGraph {
  public:
    EuclideanGraph();
    EuclideanGraph(util::Grid<uint_least8_t>&& data_i);
    EuclideanGraph(pathfinder::Coord size, uint_least8_t cost_i, int_least32_t color_i);

    inline float move_cost(const pathfinder::Coord start, const pathfinder::Coord finish) const {
      assert(check_bounds(start));
      assert(check_bounds(finish));
      assert(std::abs(start.row - finish.row) <= 1 &&
             std::abs(start.col - finish.col) <= 1 &&
             std::abs(start.layer - finish.layer) <= 1);
      assert(is_passable(start));

      if (!is_passable(finish)) {
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

    inline uint_least8_t get_cost(const pathfinder::Coord cell) const {
      return data.at(cell);
    }

    inline void set_cost(const pathfinder::Coord cell, const uint_least8_t new_cost) {
      data.at(cell) = new_cost;
    }

    inline int_least32_t get_color(const pathfinder::Coord cell) const {
      return color.at(cell);
    }

    inline void set_color(const pathfinder::Coord cell, const int_least32_t new_color) {
      color.at(cell) = new_color;
    }

    inline pathfinder::Coord size() const {
      return data.size();
    }

    inline void fill_color(int_least32_t fill) { color.fill(fill); }

    inline  bool check_bounds(const pathfinder::Coord cell) const {
      return data.check_bounds(cell);
    }

    inline std::vector<pathfinder::Coord> get_adjacent(
        const pathfinder::Coord cell,
        bool include_diagonals = true
      ) const {
        return data.get_adjacent(cell, include_diagonals);
      }

    inline bool is_passable(pathfinder::Coord cell) const {
      return (get_cost(cell) != PATH_COST_INFINITE);
    }


  private:
    friend class test::DeltaMap;
    // The map is divided into "colors" -- areas connected by purely transparent
    // terrain along Manhattan directions. Colors are handles into static
    // components (thus, we can union two colors in constant time at the price
    // of a small indirect lookup table). Typically there will be anywhere from
    // 1-5 colors per door on the map, plus one for each isolated walled off
    // region.
    util::Grid<int_least32_t> color;

    // Cost to traverse terrain.
    util::Grid<uint_least8_t> data;
};



}  // namespace graph
}  // namespace suncatcher

#endif  /* EUCLIDEANGRAPH_272b6e8aad5b4590a75bf12e43dc1adc */
