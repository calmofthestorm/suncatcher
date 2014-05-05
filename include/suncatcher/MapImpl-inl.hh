#ifndef MAPIMPL_INL_1970f504553d4baeb7f8dc6f64630b3f
#define MAPIMPL_INL_1970f504553d4baeb7f8dc6f64630b3f

namespace suncatcher {
namespace pathfinder {

inline bool MapImpl::is_transparent(Coord cell) const {
  assert(data.check_bounds(cell));
  return (!is_door(cell) && data.at(cell) != PATH_COST_INFINITE);
}


inline bool MapImpl::is_door(Coord cell) const {
  int_least32_t c = color.at(cell);
  return (c != COLOR_IMPASSABLE && is_door(c));
}


inline bool MapImpl::is_door(int_least32_t cell_color) const {
  return (cell_color < 0 && cell_color >= COLOR_LOWEST_VALID);
}


inline bool MapImpl::path_exists(Coord a, Coord b) const {
  return (is_passable(a) && is_passable(b) &&
          dynamic_component.equivalent(
              static_component.at(color.at(a)),
              static_component.at(color.at(b))
            )
          );
}


inline float MapImpl::move_cost(Coord start, Coord finish) const {
  assert(get_data().check_bounds(start));
  assert(get_data().check_bounds(finish));
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
      return get_data().at(finish);
    } else {
      return -1;
    }
  }

  if (start.row == finish.row || start.col == finish.col) {
    return get_data().at(finish);
  }

  assert(start.layer == finish.layer);
  // Can only move diagonally if Manhattan squares are passable.
  if (is_passable({start.row, finish.col, start.layer}) ||
      is_passable({finish.row, start.col, start.layer})) {
    return get_data().at(finish) * (float)1.4142135623730951;
  } else {
    return -1;
  }
}


inline bool MapImpl::is_passable(Coord cell) const {
  assert(get_data().check_bounds(cell));
  assert(get_color().at(cell) != COLOR_UNKNOWN);
  bool rval = (get_data().at(cell) != PATH_COST_INFINITE);
  #ifndef NDEBUG
    if (is_door(cell)) {
      assert(rval == doors.at(cell).open);
    } else {
      assert((get_color().at(cell) != COLOR_IMPASSABLE) == rval);
    }
    #endif
  return rval;
}


}  // namespace pathfinder
}  // namespace suncatcher

#endif  /* MAPIMPL_INL_1970f504553d4baeb7f8dc6f64630b3f */
