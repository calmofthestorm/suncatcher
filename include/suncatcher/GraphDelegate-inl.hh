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

#ifndef GRAPHDELEGATE_INL_bcfb2873bfc84d09b4df62ce541630f8
#define GRAPHDELEGATE_INL_bcfb2873bfc84d09b4df62ce541630f8

namespace suncatcher {
namespace pathfinder {

#ifdef POLYMORPHIC_API
inline GraphDelegate::GraphDelegate(std::unique_ptr<graph::GraphInterface> graph_i)
#else
inline GraphDelegate::GraphDelegate(graph::EuclideanGraph&& graph_i)
#endif
: graph(std::move(graph_i)) { }

inline float GraphDelegate::move_cost(Coord start, Coord finish) const {
  #ifdef POLYMORPHIC_API
  return graph->move_cost(start, finish);
  #else
  return graph.move_cost(start, finish);
  #endif
}

inline uint_least8_t GraphDelegate::get_cost(Coord cell) const {
  #ifdef POLYMORPHIC_API
  return graph->get_cost(cell);
  #else
  return graph.get_cost(cell);
  #endif
}

inline void GraphDelegate::set_cost(const Coord cell, const uint_least8_t cost) {
  #ifdef POLYMORPHIC_API
  graph->set_cost(cell, cost);
  #else
  graph.set_cost(cell, cost);
  #endif
}

inline int_least32_t GraphDelegate::get_color(Coord cell) const {
  #ifdef POLYMORPHIC_API
  return graph->get_color(cell);
  #else
  return graph.get_color(cell);
  #endif
}

inline void GraphDelegate::set_color(Coord cell, int_least32_t color) {
  #ifdef POLYMORPHIC_API
  graph->set_color(cell, color);
  #else
  graph.set_color(cell, color);
  #endif
}

inline bool GraphDelegate::check_bounds(Coord cell) const {
  #ifdef POLYMORPHIC_API
  return graph->check_bounds(cell);
  #else
  return graph.check_bounds(cell);
  #endif
}


inline Coord GraphDelegate::size() const {
  #ifdef POLYMORPHIC_API
  return graph->size();
  #else
  return graph.size();
  #endif
}

inline std::vector<pathfinder::Coord> GraphDelegate::get_adjacent(
    const pathfinder::Coord cell,
    bool include_diagonals
  ) const {
    #ifdef POLYMORPHIC_API
    return graph->get_adjacent(cell, include_diagonals);
    #else
    return graph.get_adjacent(cell, include_diagonals);
    #endif
  }

inline void GraphDelegate::fill_color(int_least32_t fill) {
  #ifdef POLYMORPHIC_API
  return graph->fill_color(fill);
  #else
  return graph.fill_color(fill);
  #endif
}


inline bool GraphDelegate::is_passable(Coord cell) const {
  #ifdef POLYMORPHIC_API
  return graph->is_passable(cell);
  #else
  return graph.is_passable(cell);
  #endif
}


inline Domain GraphDelegate::domain() const {
  #ifdef POLYMORPHIC_API
  return graph->domain();
  #else
  return graph.domain();
  #endif
}


inline GraphDelegate GraphDelegate::lazy_clone() const {
  #ifdef POLYMORPHIC_API
  return graph->lazy_clone();
  #else
  return GraphDelegate(graph::EuclideanGraph(graph));
  #endif
}



}  // namespace pathfinder
}  // namespace suncatcher

#endif  /* GRAPHDELEGATE_INL_bcfb2873bfc84d09b4df62ce541630f8 */
