#ifndef MAPIMPL_INL_1970f504553d4baeb7f8dc6f64630b3f
#define MAPIMPL_INL_1970f504553d4baeb7f8dc6f64630b3f

namespace suncatcher {
namespace pathfinder {

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
    const pathfinder::Coord cell,
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
