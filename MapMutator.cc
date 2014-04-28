#ifndef MAPMUTATOR_4687638a8f5c4d2fb43b15d6274b78c7
#define MAPMUTATOR_4687638a8f5c4d2fb43b15d6274b78c7

#include "MapMutator.h"
#include "Map.h"

namespace suncatcher {
namespace pathfinder {

MapMutator::MapMutator(Map* map_in)
: map(map_in),
  version(0) {
  if (map_in) {
    map_in->notify_mutator_created();
    version = map_in->version;
  }
}

MapMutator::~MapMutator() {
  if (map) {
    map->notify_mutator_destroyed();
  }
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
  assert(map->get_doors() != map->get_doors().cend());
  assert(mutations.find(door) == mutations.end() ||
         mutations.at(door).kind == Mutation::Kind::UPDATE_DOOR);
  mutations[door] = {Mutation::Kind::UPDATE_DOOR, true,
                     (uint_least8_t)PATH_COST_INFINITE};
  return *this;
}

MapMutator& MapMutator::set_door_open_cost(Coord door, uint_least8_t cost) {
  assert(map);
  assert(map->get_doors() != map->get_doors().cend());
  assert(door_it != map->get_doors().cend());
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
