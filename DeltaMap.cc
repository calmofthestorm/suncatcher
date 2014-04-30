#include "DeltaMap.h"

#include "Map.h"


namespace {
  using suncatcher::pathfinder::Coord;
  using suncatcher::pathfinder::Path;
  using suncatcher::pathfinder::MapMutator;
  using suncatcher::pathfinder::MapBuilder;
  using suncatcher::pathfinder::Map;
}  // anonymous namespace


namespace suncatcher {
namespace test {



DeltaMutator::DeltaMutator(MapMutator&& m1_i, MapMutator&& m2_i)
: m1(std::move(m1_i)),
  m2(std::move(m2_i)) { }


DeltaMutator& DeltaMutator::set_door_open_cost(Coord door, uint_least8_t cost) {
  m1.set_door_open_cost(door, cost);
  m2.set_door_open_cost(door, cost);
  return *this;
}


DeltaMutator& DeltaMutator::set_door_open(Coord door, bool open) {
  m1.set_door_open(door, open);
  m2.set_door_open(door, open);
  return *this;
}


DeltaMutator& DeltaMutator::toggle_door_open(Coord door) {
  m1.toggle_door_open(door);
  m2.toggle_door_open(door);
  return *this;
}


DeltaMutator& DeltaMutator::set_cost(Coord cell, uint_least8_t cost) {
  m1.set_cost(cell, cost);
  m2.set_cost(cell, cost);
  return *this;
}


DeltaMutator& DeltaMutator::create_door(Coord cell, bool open, uint_least8_t open_cost) {
  m1.create_door(cell, open, open_cost);
  m2.create_door(cell, open, open_cost);
  return *this;
}


DeltaMutator& DeltaMutator::remove_door(Coord cell, uint_least8_t new_cost) {
  m1.remove_door(cell, new_cost);
  m2.remove_door(cell, new_cost);
  return *this;
}


DeltaMap::DeltaMap(MapBuilder mb) {
  mb.enable_dynamic_updates(true);
  optimized_map.reset(new Map(MapBuilder(mb)));
  mb.enable_dynamic_updates(false);
  simple_map.reset(new Map(std::move(mb)));
}


void DeltaMap::check_invariant() {
  
}


bool DeltaMap::is_door(Coord start) const {
  return optimized_map->is_door(start);
}


bool DeltaMap::path_exists(Coord start, Coord finish) const {
  return optimized_map->path_exists(start, finish);
}


Path DeltaMap::path(Coord start, Coord finish) const {
  return optimized_map->path(start, finish);
}


DeltaMutator DeltaMap::get_mutator() {
  return DeltaMutator(optimized_map->get_mutator(), simple_map->get_mutator());
}


void DeltaMap::mutate(DeltaMutator&& mutator) {
  optimized_map->mutate(std::move(mutator.m1));
  simple_map->mutate(std::move(mutator.m2));
  check_invariant();
}


const std::map<const Coord, suncatcher::pathfinder::Door>& DeltaMap::get_doors() const {
  return optimized_map->get_doors();
}



const util::Grid<uint8_t>& DeltaMap::get_data() const {
  return optimized_map->get_data();
}



}  // namespace test
}  // namespace suncatcher
