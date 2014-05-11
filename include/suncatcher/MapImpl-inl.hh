// Pathfinder abstraction of the map. The MapImpl class maintains a simplified
// abstraction of the map, intended to support efficient pathfinding,
// reachability, and updates in a dynamic world.
//
// In an early version of this code, I ran a benchmark suite to determine
// whether using smaller integral types for better cache performance versus
// using native word types resulted in better performance. I found ~5%
// improvement on a moderate sample size (p < 0.01) to using smaller types,
// which makes sense given all the computations are simple and most of this
// algorithm is fetching the map from memory.
//
// The other concern is that predictive pathing in multiple threads could run
// into memory pressure.
//
// The code has undergone substantial refactoring since then, however, so this
// may need revisiting in the future.

#ifndef MAPIMPL_INL_1970f504553d4baeb7f8dc6f64630b3f
#define MAPIMPL_INL_1970f504553d4baeb7f8dc6f64630b3f

#include "suncatcher/MapBuilder.hh"

namespace suncatcher {
namespace pathfinder {

inline MapImpl::MapImpl(MapBuilder&& builder)
: graph(std::move(builder.graph)),
  doors(std::move(builder.doors)) {

  rebuild();
}


inline MapImpl::MapImpl(MapImpl&& other) {
  graph = std::move(other.graph);
  doors = std::move(other.doors);
  static_component = std::move(other.static_component);
  dynamic_component = std::move(other.dynamic_component);
  next_door_color = other.next_door_color;
  next_component_color = other.next_component_color;
}


inline MapImpl& MapImpl::operator=(MapImpl&& other) {
  graph = std::move(other.graph);
  doors = std::move(other.doors);
  static_component = std::move(other.static_component);
  dynamic_component = std::move(other.dynamic_component);
  next_door_color = other.next_door_color;
  next_component_color = other.next_component_color;
  return *this;
}


inline MapImpl::MapImpl(const MapImpl& other)
: graph(other.graph.lazy_clone()),
  doors(other.doors),
  static_component(other.static_component),
  dynamic_component(other.dynamic_component),
  next_component_color(other.next_component_color),
  next_door_color(other.next_door_color) { }


inline MapImpl& MapImpl::operator=(const MapImpl& other) {
  graph = other.graph.lazy_clone();
  doors = other.doors;
  static_component = other.static_component;
  dynamic_component = other.dynamic_component;
  next_component_color = other.next_component_color;
  next_door_color = other.next_door_color;
  return *this;
}


inline bool MapImpl::is_transparent(Coord cell) const {
  assert(graph.check_bounds(cell));
  return (!is_door(cell) && graph.get_cost(cell) != PATH_COST_INFINITE);
}


inline bool MapImpl::is_door(Coord cell) const {
  int_least32_t c = graph.get_color(cell);
  return (c != COLOR_IMPASSABLE && is_door(c));
}


inline bool MapImpl::is_door(int_least32_t cell_color) const {
  return (cell_color < 0 && cell_color >= COLOR_LOWEST_VALID);
}


inline bool MapImpl::path_exists(Coord a, Coord b) const {
  return (is_passable(a) && is_passable(b) &&
          (dynamic_component.at(graph.get_color(a)) ==
           dynamic_component.at(graph.get_color(b))));
}


inline float MapImpl::move_cost(Coord start, Coord finish) const {
  return graph.move_cost(start, finish);
}

inline void MapImpl::get_adjacent(
    const Coord cell,
    bool include_diagonals,
    std::vector<Coord>& adj
    ) const {

  graph.get_adjacent(cell, include_diagonals, adj);
}


inline bool MapImpl::is_passable(Coord cell) const {
  assert(graph.check_bounds(cell));
  assert(graph.get_color(cell) != COLOR_UNKNOWN);
  bool rval = graph.is_passable(cell);
  #ifndef NDEBUG
    if (is_door(cell)) {
      assert(rval == doors.at(cell).open);
    } else {
      assert((graph.get_color(cell) != COLOR_IMPASSABLE) == rval);
    }
    #endif
  return rval;
}


pathfinder::Domain MapImpl::domain() const {
  return graph.domain();
}


}  // namespace pathfinder
}  // namespace suncatcher

#endif  /* MAPIMPL_INL_1970f504553d4baeb7f8dc6f64630b3f */
