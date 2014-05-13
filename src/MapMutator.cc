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


#include "suncatcher/MapMutator.hh"
#include "suncatcher/MapView.hh"
#include "suncatcher/MapBuilder.hh"

namespace suncatcher {
namespace pathfinder {



MapMutator::MapMutator()
: view(MapView()) { }


MapMutator::MapMutator(MapView view_in)
: view(view_in) { }


MapMutator& MapMutator::make_normal(Coord cell, uint_least8_t cost) {
  mutations.push_back({cell, false, false, cost});
  return *this;
}


MapMutator& MapMutator::make_door(
    Coord cell,
    bool open,
    uint_least8_t open_cost
  ) {

  assert(open_cost != PATH_COST_INFINITE);
  mutations.push_back({cell, true, open, open_cost});
  return *this;
}


MapView MapMutator::execute(bool incremental) const {
  return MapView(std::make_shared<MapImpl>(*this, incremental));
}



}  // namespace pathfinder
}  // namespace suncatcher
