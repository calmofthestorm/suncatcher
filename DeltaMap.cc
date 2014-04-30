#include "DeltaMap.h"

#include "Map.h"
#include "UnionFind.h"

namespace {
  using suncatcher::pathfinder::Coord;
  using suncatcher::pathfinder::Path;
  using suncatcher::pathfinder::MapMutator;
  using suncatcher::pathfinder::MapBuilder;
  using suncatcher::pathfinder::Map;
}  // anonymous namespace


namespace suncatcher {
namespace test {



template <typename T>
const T& union_find_lookup_no_compress(const util::UnionFind<T>& uf, const T& elem) {
  size_t key = uf.handle_to_id.at(elem);
  while (uf.parent[key] != key) {
    key = uf.parent[key];
  }
  return uf.id_to_handle[key];
}


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


void DeltaMap::check_invariant() const {
  assert(simple_map->version == optimized_map->version);
  assert(simple_map->outstanding_mutators == optimized_map->outstanding_mutators);
  assert(simple_map->data == optimized_map->data);
  assert(simple_map->doors == optimized_map->doors);

  // Colors may vary provided that it's isomorphic.
  assert(simple_map->color.size() == optimized_map->color.size());
  std::map<uint_least32_t, uint_least32_t> cmapping, dmapping;
  for (size_t j = 0; j < simple_map->color.size().row; ++j) {
    for (size_t i = 0; i < simple_map->color.size().col; ++i) {
      uint_least32_t scolor = simple_map->color.at(j, i);
      uint_least32_t ocolor = optimized_map->color.at(j, i);
      if (scolor == pathfinder::COLOR_IMPASSABLE) {
        assert(ocolor == scolor);
      } else {
        auto c = cmapping.find(ocolor);
        if (c != cmapping.end()) {
          assert(c->second == scolor);
        } else {
          // Door base color may differ, but doorliness should be preserved.
          assert(simple_map->is_door(scolor) == optimized_map->is_door(ocolor));
          cmapping[ocolor] = scolor;
        }
      }
    }
  }

  // Similar idea for dynamic components.
  for (const auto& it : cmapping) {
    uint_least32_t sdcomponent = union_find_lookup_no_compress(
        simple_map->dynamic_component.uf,
        it.second
      );

    uint_least32_t odcomponent = union_find_lookup_no_compress(
        optimized_map->dynamic_component.uf,
        it.first
      );

    auto c = dmapping.find(odcomponent);
    if (c != dmapping.end()) {
      assert(c->second == sdcomponent);
    } else  {
      cmapping[odcomponent] = sdcomponent;
    }
  }
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
