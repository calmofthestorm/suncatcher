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

#include <cstdlib>

#include "suncatcher/test/DeltaMap.hh"

#include "suncatcher/MapView.hh"
#include "suncatcher/util/UnionFind.hh"
#include "suncatcher/Coord.hh"

namespace {
  using suncatcher::Coord;
  using suncatcher::pathfinder::Path;
  using suncatcher::pathfinder::MapMutator;
  using suncatcher::pathfinder::MapBuilder;
  using suncatcher::pathfinder::MapView;
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


DeltaMutator::DeltaMutator(MapMutator&& m1_i, MapMutator&& m2_i, bool slow_i)
: m1(std::move(m1_i)),
  m2(std::move(m2_i)),
  enable_delta(slow_i){ }


DeltaMutator::DeltaMutator(DeltaMutator&& other) {
  std::swap(other.m1, m1);
  std::swap(other.m2, m2);
}


DeltaMutator& DeltaMutator::operator=(DeltaMutator&& other) {
  std::swap(other.m1, m1);
  std::swap(other.m2, m2);
  return *this;
}


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
  if (enable_delta) {
    m2.remove_door(cell, new_cost);
  }
  return *this;
}


DeltaMap::DeltaMap()
: optimized_map(),
  simple_map() { }


DeltaMap::DeltaMap(MapView view)
: optimized_map(view),
  simple_map(view) {
  char* slow = std::getenv("SLOWTEST");
  enable_delta = !slow || !strcmp("1", slow);
}


void DeltaMap::check_invariant() const {
  #ifdef POLYMORPHIC_API
  for (const auto& cell : simple_map.map->graph.domain()) {
    assert(simple_map.map->graph.get_cost(cell) ==
           optimized_map.map->graph.get_cost(cell));
  }
  #else
  assert(simple_map.map->graph.graph.data == optimized_map.map->graph.graph.data);
  #endif
  assert(simple_map.get_doors() == optimized_map.get_doors());

  std::map<int_least32_t, int_least32_t> cmapping, smapping, dmapping;
  for (const Coord& c : simple_map.map->graph.domain()) {
    // Check color. Colors may vary provided that it's many-to-one from
    // optimized to simple.
    int_least32_t scolor = simple_map.map->graph.get_color(c);
    int_least32_t ocolor = optimized_map.map->graph.get_color(c);
    if (scolor == COLOR_IMPASSABLE) {
      assert(ocolor == scolor);
    } else {
      auto corresponding = cmapping.find(ocolor);
      if (corresponding != cmapping.end()) {
        assert(corresponding->second == scolor);
      } else {
        // Door base color may differ, but doorliness should be preserved.
        assert(simple_map.is_door(scolor) == optimized_map.is_door(ocolor));
        cmapping[ocolor] = scolor;
      }
    }

    if (scolor != COLOR_IMPASSABLE) {
      // Attempt to find an isomorphism for static components.
      int_least32_t sstatic = simple_map.map->static_component.at(scolor);
      int_least32_t ostatic = optimized_map.map->static_component.at(ocolor);
      auto corresponding = smapping.find(ostatic);
      if (corresponding != smapping.end()) {
        assert(corresponding->second == sstatic);
      } else {
        smapping[ostatic] = sstatic;
      }

      // Attempt to find an isomorphism for dynamic components.
      int_least32_t sdynamic = simple_map.map->dynamic_component.at(scolor);
      int_least32_t odynamic = optimized_map.map->dynamic_component.at(ocolor);
      corresponding = dmapping.find(odynamic);
      if (corresponding != dmapping.end()) {
        assert(corresponding->second == sdynamic);
      } else {
        dmapping[odynamic] = sdynamic;
      }
    }
  }
}

bool DeltaMap::is_door(Coord start) const {
  return optimized_map.is_door(start);
}


bool DeltaMap::path_exists(Coord start, Coord finish) const {
  bool opt = optimized_map.path_exists(start, finish);
  if (enable_delta) {
    bool sim = simple_map.path_exists(start, finish);
    assert(opt == sim);
  }
  return opt;
}


Path DeltaMap::path(Coord start, Coord finish) const {
  Path opt = optimized_map.path(start, finish);
  if (enable_delta) {
    Path sim = simple_map.path(start, finish);
    assert(opt == sim);
  }
  return opt;
}


DeltaMutator DeltaMap::get_mutator() {
  return DeltaMutator(
      MapMutator(optimized_map),
      MapMutator(simple_map),
      enable_delta
    );
}


void DeltaMap::mutate(const DeltaMutator& mutator) {
  optimized_map = mutator.m1.execute();
  if (enable_delta) {
    simple_map = mutator.m2.execute(false);
    check_invariant();
  } else {
    simple_map = optimized_map;
  }
}


const std::map<const Coord, pathfinder::Door>& DeltaMap::get_doors() const {
  return optimized_map.get_doors();
}

uint_least8_t DeltaMap::get_cost(Coord cell) const {
  return optimized_map.map->graph.get_cost(cell);
}


void DeltaMap::clear_cache() {
  optimized_map = simple_map;
}


pathfinder::Domain DeltaMap::domain() const {
  return simple_map.map->graph.domain();
}


}  // namespace test
}  // namespace suncatcher
