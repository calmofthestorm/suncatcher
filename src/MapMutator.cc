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

#ifndef MAPMUTATOR_4687638a8f5c4d2fb43b15d6274b78c7
#define MAPMUTATOR_4687638a8f5c4d2fb43b15d6274b78c7

#include "suncatcher/MapMutator.hh"
#include "suncatcher/MapView.hh"
#include "suncatcher/MapBuilder.hh"

namespace suncatcher {
namespace pathfinder {



MapMutator::MapMutator()
: view(MapView()) { }


MapMutator::MapMutator(MapView view_in)
: view(view_in) { }


MapMutator& MapMutator::set_door_open(Coord door, bool state) {
  auto door_it = view.get_doors().find(door);
  assert(door_it != view.get_doors().cend());
  assert(mutations.find(door) == mutations.end() ||
         mutations.at(door).kind == Mutation::Kind::UPDATE_DOOR);
  state ^= door_it->second.open;
  mutations[door] = {Mutation::Kind::UPDATE_DOOR, state,
                    (uint_least8_t)PATH_COST_INFINITE};
  return *this;
}


MapMutator& MapMutator::toggle_door_open(Coord door) {
  assert(view.get_doors().find(door) != view.get_doors().cend());
  assert(mutations.find(door) == mutations.end() ||
         mutations.at(door).kind == Mutation::Kind::UPDATE_DOOR);
  mutations[door] = {Mutation::Kind::UPDATE_DOOR, true,
                     (uint_least8_t)PATH_COST_INFINITE};
  return *this;
}


MapMutator& MapMutator::set_door_open_cost(Coord door, uint_least8_t cost) {
  assert(view.get_doors().find(door) != view.get_doors().cend());
  assert(view.get_doors().find(door) != view.get_doors().cend());
  assert(cost != PATH_COST_INFINITE);
  assert(mutations.find(door) == mutations.end() ||
         mutations.at(door).kind == Mutation::Kind::UPDATE_DOOR);
  mutations[door] = {Mutation::Kind::UPDATE_DOOR, false, cost};
  return *this;
}


MapMutator& MapMutator::set_cost(Coord cell, uint_least8_t cost) {
  assert(view.get_doors().find(cell) == view.get_doors().cend());
  mutations[cell] = {Mutation::Kind::SET_COST, false, cost};
  return *this;
}


MapMutator& MapMutator::create_door(
    Coord cell,
    bool open,
    uint_least8_t open_cost
  ) {

  assert(open_cost != PATH_COST_INFINITE);
  assert(view.get_doors().find(cell) == view.get_doors().cend());
  mutations[cell] = {Mutation::Kind::CREATE_DOOR, open, open_cost};

  return *this;
}


MapMutator& MapMutator::remove_door(Coord cell, uint_least8_t new_cost) {
  assert(view.get_doors().find(cell) != view.get_doors().cend());
  mutations[cell] = {Mutation::Kind::REMOVE_DOOR, false, new_cost};

  return *this;
}


MapView MapMutator::execute(bool incremental) const {
  return MapView(std::make_shared<MapImpl>(*this, incremental));
}



}  // namespace pathfinder
}  // namespace suncatcher


#endif  /* MAPMUTATOR_4687638a8f5c4d2fb43b15d6274b78c7 */
