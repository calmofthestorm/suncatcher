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

// MapImpl delegates the actual graph geometry to this class. It is a thin
// wrapper around either a pointer to a GraphInterface (if compiled with the
// polymorphic API) or a EuclideanGraph if not. Since geometry queries are
// in flood fill and pathing tight loops, inlining amounts to a significant
// speedup if possible.
//
// The purpose of this class is to support the POLYMORPHIC_API compile option,
// to avoid needing #ifdefs in any other classes.

#ifndef GRAPHDELEGATE_3219342b61d146a7af1e2a73e940cd56
#define GRAPHDELEGATE_3219342b61d146a7af1e2a73e940cd56

#include "suncatcher.hh"
#include "suncatcher/Coord.hh"
#include "suncatcher/util/Grid.hh"
#include "suncatcher/EuclideanGraph.hh"

// #define POLYMORPHIC_API
namespace suncatcher {

namespace test {
  class DeltaMap;
}  // namespace test

namespace pathfinder {



class GraphInterface {
  public:
    virtual float move_cost(Coord start, Coord finish) = 0;

    virtual uint_least8_t get_cost(Coord cell) = 0;
    virtual void set_cost(Coord cell, uint_least8_t cost) = 0;

    virtual int_least32_t get_color(Coord cell) = 0;
    virtual void set_color(Coord cell, int_least32_t color) = 0;

    virtual Coord size() = 0;
    virtual bool check_bounds(Coord cell) = 0;

    virtual  std::vector<pathfinder::Coord> get_adjacent(
        const pathfinder::Coord cell,
        bool include_diagonals = true
      ) = 0;

    virtual void fill_color(int_least32_t fill) = 0;

    virtual void is_passable(Coord cell) = 0;
};



class GraphDelegate {
  public:
    #ifdef POLYMORPHIC_API
    inline GraphDelegate(std::unique_ptr<GraphInterface> graph_i)
    #else
    inline GraphDelegate(graph::EuclideanGraph&& graph_i)
    #endif
    : graph(std::move(graph_i)) { }

    inline float move_cost(Coord start, Coord finish) const {
      #ifdef POLYMORPHIC_API
      return graph->move_cost(start, finish);
      #else
      return graph.move_cost(start, finish);
      #endif
    }

    inline uint_least8_t get_cost(Coord cell) const {
      #ifdef POLYMORPHIC_API
      return graph->get_cost(cell);
      #else
      return graph.get_cost(cell);
      #endif
    }

    inline void set_cost(const Coord cell, const uint_least8_t cost) {
      #ifdef POLYMORPHIC_API
      graph->set_cost(cell, cost);
      #else
      graph.set_cost(cell, cost);
      #endif
    }

    inline int_least32_t get_color(Coord cell) const {
      #ifdef POLYMORPHIC_API
      return graph->get_color(cell);
      #else
      return graph.get_color(cell);
      #endif
    }

    inline void set_color(Coord cell, int_least32_t color) {
      #ifdef POLYMORPHIC_API
      graph->set_color(cell, color);
      #else
      graph.set_color(cell, color);
      #endif
    }

    inline bool check_bounds(Coord cell) const {
      #ifdef POLYMORPHIC_API
      return graph->check_bounds(cell);
      #else
      return graph.check_bounds(cell);
      #endif
    }


    inline Coord size() const {
      #ifdef POLYMORPHIC_API
      return graph->size();
      #else
      return graph.size();
      #endif
    }

    inline std::vector<pathfinder::Coord> get_adjacent(
        const pathfinder::Coord cell,
        bool include_diagonals = true
      ) const {
        #ifdef POLYMORPHIC_API
        return graph->get_adjacent(cell, include_diagonals);
        #else
        return graph.get_adjacent(cell, include_diagonals);
        #endif
      }

    inline void fill_color(int_least32_t fill) {
      #ifdef POLYMORPHIC_API
      return graph->fill_color(fill);
      #else
      return graph.fill_color(fill);
      #endif
    }


    inline bool is_passable(Coord cell) const {
      #ifdef POLYMORPHIC_API
      return graph->is_passable(cell);
      #else
      return graph.is_passable(cell);
      #endif
    }


  private:
    friend class test::DeltaMap;
    #ifdef POLYMORPHIC_API
    // Don't want to force implementers to adhere to const restriction.
    mutable std::unique_ptr<GraphInterface> graph;
    #else
    graph::EuclideanGraph graph;
    #endif
};



}  // namespace pathfinder
}  // namespace suncatcher

#endif  /* GRAPHDELEGATE_3219342b61d146a7af1e2a73e940cd56 */
