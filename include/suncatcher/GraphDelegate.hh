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
#include "suncatcher/graph/EuclideanCoordRange.hh"

// #define POLYMORPHIC_API
#ifdef POLYMORPHIC_API
  #include "suncatcher/graph/GraphInterface.hh"
#else
  #include "suncatcher/graph/EuclideanGraph.hh"
#endif

namespace suncatcher {

namespace test {
  class DeltaMap;
}  // namespace test


namespace pathfinder {

#ifdef POLYMORPHIC_API
  typedef graph::GraphDomain Domain;
#else
  typedef CoordRange Domain;
#endif

class GraphDelegate {
  public:
    inline GraphDelegate() { };

    #ifdef POLYMORPHIC_API
    inline GraphDelegate(std::unique_ptr<graph::GraphInterface> graph_i);
    #else
    inline GraphDelegate(graph::EuclideanGraph&& graph_i);
    #endif

    #ifdef POLYMORPHIC_API
    inline GraphDelegate(const GraphDelegate&);
    inline const GraphDelegate& operator=(const GraphDelegate&) const;
    #endif

    inline float move_cost(Coord start, Coord finish) const;

    inline uint_least8_t get_cost(Coord cell) const;

    inline void set_cost(const Coord cell, const uint_least8_t cost);

    inline int_least32_t get_color(Coord cell) const;

    inline void set_color(Coord cell, int_least32_t color);

    inline bool check_bounds(Coord cell) const;

    inline Coord size() const;

    inline void get_adjacent(
        const Coord cell,
        bool include_diagonals,
        std::vector<Coord>& adj
      ) const;

    inline void fill_color(int_least32_t fill);

    inline bool is_passable(Coord cell) const;

    inline Domain domain() const;

    inline GraphDelegate lazy_clone() const;


  private:
    friend class test::DeltaMap;

    #ifdef POLYMORPHIC_API
    // Don't want to force implementers to adhere to const restriction.
    mutable std::unique_ptr<graph::GraphInterface> graph;
    #else
    graph::EuclideanGraph graph;
    #endif
};

}  // namespace pathfinder
}  // namespace suncatcher

#include "suncatcher/GraphDelegate-inl.hh"

#endif  /* GRAPHDELEGATE_3219342b61d146a7af1e2a73e940cd56 */
