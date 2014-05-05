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

#ifndef MAP_INL_c67ecba94fae4db889e83c228d138cd1
#define MAP_INL_c67ecba94fae4db889e83c228d138cd1

#include "MapImpl.hh"

namespace suncatcher {
namespace pathfinder {



inline bool MapView::is_door(Coord cell) const {
  return map->is_door(cell);
}


inline bool MapView::is_door(int_least32_t cell_color) const {
  return map->is_door(cell_color);
}


inline Coord MapView::size() const {
  return map->get_size();
}


inline const std::map<const Coord, Door>& MapView::get_doors() const {
  return map->get_doors();
}


inline const util::Grid<uint_least8_t>& MapView::get_data() const {
  return map->get_data();
}


inline bool MapView::is_passable(Coord cell) const {
  return map->is_passable(cell);
}


inline bool MapView::path_exists(Coord src, Coord dst) const {
  return map->path_exists(src, dst);
}


inline uint8_t MapView::move_cost(Coord src, Coord dst) const {
  return map->move_cost(src, dst);
}


}  // namespace pathfinder
}  // namespace suncatcher

#endif  /* MAP_INL_c67ecba94fae4db889e83c228d138cd1 */