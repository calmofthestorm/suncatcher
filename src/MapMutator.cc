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
#include "suncatcher/Map.hh"

namespace suncatcher {
namespace pathfinder {



MapMutator::MapMutator(Map* map_in, size_t version_in)
: map(map_in),
  version(0) {
  if (map_in) {
    map_in->notify_mutator_created();
    version = version_in;
  }
}


MapMutator::~MapMutator() NOEXCEPT {
  if (map) {
    map->notify_mutator_destroyed();
  }
}


MapMutator& MapMutator::operator=(MapMutator&& other) NOEXCEPT {
  std::swap(map, other.map);
  std::swap(version, other.version);
  std::swap(mutations, other.mutations);
  return *this;
}


MapMutator::MapMutator(MapMutator&& other) NOEXCEPT {
  map = nullptr;
  std::swap(map, other.map);
  std::swap(version, other.version);
  std::swap(mutations, other.mutations);
}


MapMutator& MapMutator::set_door_open(Coord door, bool state) {
  assert(map);
  auto door_it = map->get_doors().find(door);
  assert(door_it != map->get_doors().cend());
  assert(mutations.find(door) == mutations.end() ||
         mutations.at(door).kind == Mutation::Kind::UPDATE_DOOR);
  state ^= door_it->second.open;
  mutations[door] = {Mutation::Kind::UPDATE_DOOR, state,
                    (uint_least8_t)PATH_COST_INFINITE};
  return *this;
}

MapMutator& MapMutator::toggle_door_open(Coord door) {
  assert(map);
  assert(map->get_doors().find(door) != map->get_doors().cend());
  assert(mutations.find(door) == mutations.end() ||
         mutations.at(door).kind == Mutation::Kind::UPDATE_DOOR);
  mutations[door] = {Mutation::Kind::UPDATE_DOOR, true,
                     (uint_least8_t)PATH_COST_INFINITE};
  return *this;
}

MapMutator& MapMutator::set_door_open_cost(Coord door, uint_least8_t cost) {
  assert(map);
  assert(map->get_doors().find(door) != map->get_doors().cend());
  assert(map->get_doors().find(door) != map->get_doors().cend());
  assert(cost != PATH_COST_INFINITE);
  assert(mutations.find(door) == mutations.end() ||
         mutations.at(door).kind == Mutation::Kind::UPDATE_DOOR);
  mutations[door] = {Mutation::Kind::UPDATE_DOOR, false, cost};
  return *this;
}

MapMutator& MapMutator::set_cost(Coord cell, uint_least8_t cost) {
  assert(map);
  assert(map->get_doors().find(cell) == map->get_doors().cend());
  mutations[cell] = {Mutation::Kind::SET_COST, false, cost};
  return *this;
}

MapMutator& MapMutator::create_door(Coord cell, bool open, uint_least8_t open_cost) {
  assert(map);
  assert(open_cost != PATH_COST_INFINITE);
  assert(map->get_doors().find(cell) == map->get_doors().cend());
  mutations[cell] = {Mutation::Kind::CREATE_DOOR, open, open_cost};

  return *this;
}

MapMutator& MapMutator::remove_door(Coord cell, uint_least8_t new_cost) {
  assert(map);
  assert(map->get_doors().find(cell) != map->get_doors().cend());
  mutations[cell] = {Mutation::Kind::REMOVE_DOOR, false, new_cost};

  return *this;
}

}  // namespace pathfinder
}  // namespace suncatcher


#endif  /* MAPMUTATOR_4687638a8f5c4d2fb43b15d6274b78c7 */
