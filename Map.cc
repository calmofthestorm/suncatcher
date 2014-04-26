#include "Map.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <queue>
#include <stack>
#include <set>
#include <string>
#include <tuple>

#include "util.h"

namespace suncatcher {
namespace pathfinder {

using suncatcher::util::Grid;
using suncatcher::util::find_representative;
using suncatcher::util::manhattan;

MapBuilder::MapBuilder(const Coord& size, uint8_t cost)
: data(size, cost),
  doors() { }

void MapBuilder::add_door(const Coord& cell, bool open, uint8_t cost_open,
                          uint8_t cost_closed) {
  assert(doors.find(cell) == doors.end());
  doors[cell] = {open, cost_open, cost_closed, {}};
  data.at(cell) = open ? cost_open : cost_closed;
}

void Map::print_map(std::ostream& os, const std::vector<Coord>& path) const {
  // lol this is so bad...debugging only tho...
  std::set<Coord> in_path(path.begin(), path.end());
  size_t index = 0;
  for (uint16_t j = 0; j < size().row; ++j) {
    for (uint16_t i = 0; i < size().col; ++i) {
      auto door = doors.find({j, i});
      if (in_path.find({j, i}) != in_path.end()) {
        if (door == doors.end()) {
          os << '.';
        } else {
          ++index;
          os << "\033[1m.\033[0m";
        }
      } else if (door != doors.end()) {
        if (door->second.open) {
          os << index++;
        } else {
          os << "\033[1m" << index++ << "\033[0m";
        }
      } else {
        os << (is_opaque({j, i}) ? '*' : ' ');
      }
    }
    os << std::endl;
  }
}

bool Map::get_door(Coord cell) const {
  return doors.at(cell).open;
}

void Map::set_door(Coord cell, bool open) {
  doors.at(cell).open = open;
  data.at(cell) = open ? doors.at(cell).cost_open : doors.at(cell).cost_closed;
}

// TODO: evaluate if an incremental data struture (eg union find) does
// better than eager transitive closure in practice.
// TODO: evaluate if we can do this more efficiently.
void Map::update_equivalence(const Coord&, bool) {
  // if (new_state) {
  //   auto bridged = doors[find_door(pos)].get_adjacent_components();
  //   // Slight savings available with partially-incremental update.
  // }
  for (size_t i = 0; i < equivalent_components.size(); ++i) {
    equivalent_components[i] = i;
  }

  for (const auto& d : doors) {
    if (d.second.open) {
      auto bridged = d.second.adjacent_components;
      int class1 = find_representative(equivalent_components, bridged.front());
      for (auto it = bridged.begin() + 1; it < bridged.end(); ++it) {
        int class2 = find_representative(equivalent_components, *it);
        equivalent_components[class2] = class1;
      }
    }
  }

  // Transitive closure
  // TODO: faster; path compression
  for (size_t i = 0; i < equivalent_components.size(); ++i) {
    equivalent_components[i] = find_representative(equivalent_components, i);
  }
}

void Map::print_components(std::ostream& os) const {
  for (uint16_t j = 0; j < size().row; ++j) {
    for (uint16_t i = 0; i < size().col; ++i) {
      uint32_t c = component.at(j, i);
      if (c == COMPONENT_MULTIPLE) {
        os << 'd';
      } else if (!is_opaque({j, i})) {
        os << (char)(c + 'A');
      } else {
        os << '*';
      }
    }
    os << std::endl;
  }
}

void Map::print_equivalence_classes(std::ostream& os) const {
  for (uint16_t j = 0; j < size().row; ++j) {
    for (uint16_t i = 0; i < size().col; ++i) {
      if (data.at(j, i) == ' ') {
        int c = equivalent_components.at(component.at(j, i));
        if (c == -2) {
          os << ' ';
        } else if (c >= 0 && c < 25) {
          os << (char)(c + 'A');
        } else {
          os << ' ' << c << ' ';
        }
      } else {
        os << data.at(j, i);
      }
    }
    os << std::endl;
  }
}

bool Map::same_equivalence_class(Coord a, Coord b) const {
  // If either is an open door, then we must check multiple equivalence classes.
  uint_least32_t component_a = component.at(a);
  uint_least32_t component_b = component.at(b);
  assert(true);
  assert((doors.find(a) != doors.end()) == (component_a == COMPONENT_MULTIPLE));
  assert((doors.find(b) != doors.end()) == (component_b == COMPONENT_MULTIPLE));
  if (component_a != COMPONENT_MULTIPLE && component_b != COMPONENT_MULTIPLE) {
    // Fast path -- neither is a door.

    assert(component_a != COMPONENT_UNKNOWN && component_b != COMPONENT_UNKNOWN);
    assert(component_a < equivalent_components.size());
    assert(component_b < equivalent_components.size());
    return (find_representative(equivalent_components, component_a) ==
            find_representative(equivalent_components, component_b));
  } else {
    // Slow path -- one or more doors.

    if (component_a == COMPONENT_MULTIPLE && component_b == COMPONENT_MULTIPLE) {
      auto door_a = doors.find(a);
      auto door_b = doors.find(b);
      // Both are doors.
      for (const uint_least32_t& subcomponent_a : door_a->second.adjacent_components) {
        for (const uint_least32_t& subcomponent_b : door_b->second.adjacent_components) {
          if (find_representative(equivalent_components, subcomponent_a) ==
              find_representative(equivalent_components, subcomponent_b)) {
            return true;
          }
        }
      }
      return false;
    }

    // One door one non door.
    if (component_a == COMPONENT_MULTIPLE) {
      assert(component_b != COMPONENT_MULTIPLE);
      std::swap(component_a, component_b);
      std::swap(a, b);
    }

    assert(component_b == COMPONENT_MULTIPLE);
    assert(component_a != COMPONENT_MULTIPLE);

    component_a = find_representative(equivalent_components, component_a);
    auto door_b_adj = doors.find(b)->second.adjacent_components;
    for (const uint_least32_t& subcomponent_b : door_b_adj) {
      if (find_representative(equivalent_components, subcomponent_b) == component_a) {
        return true;
      }
    }
  return false;
  }
}

std::vector<Coord> Map::path(const Coord& src, const Coord& dst) const {
  Grid<int> expanded(size().row, size().col, 0);
  Grid<Coord> previous(size().row, size().col, {(uint16_t)-1, (uint16_t)-1});
  size_t num_expanded = 0;

  // A path exists iff they are in the same equivalence class and both are
  // passable squares.
  if (!is_passable(src) || !is_passable(dst) || !same_equivalence_class(src, dst)) {
    return std::vector<Coord>();
  }

  // Order intentional, if A is impassable there is no path from A to A.
  if (src == dst) {
    return std::vector<Coord>{src};
  }

  Grid<float> distance(size().row, size().col, INFINITY);
  distance.at(src) = 0;

  struct Entry {
    Coord pos;
    float cost;

    // Make pqueue work in proper order.
    bool operator< (const Entry& e) const {
      return cost > e.cost;
    }
  };
  std::priority_queue<Entry> fringe;
  fringe.push({src, manhattan(src, dst)});

  while (!fringe.empty()) {
    Entry cur = fringe.top();
    assert(is_passable(cur.pos));
    fringe.pop();
    expanded.at(cur.pos) = 1;
    ++num_expanded;

    if (cur.pos == dst) {
      // TODO: unnecessary copy
      std::vector<Coord> rval{dst};
      rval.reserve((distance.at(dst) + 1));
      while (rval.back() != src) {
        rval.push_back(previous.at(rval.back()));
      }
      std::cout << "dist is " << distance.at(dst) << std::endl;
      return rval;
    }

    for (const auto& next : data.get_adjacent(cur.pos)) {
      float cost = move_cost(cur.pos, next);
      float my_dist = distance.at(cur.pos) + cost;
      if (distance.at(next) > my_dist &&
          !expanded.at(next) &&
          cost != -1) {
        distance.at(next) = my_dist;
        fringe.push({next, manhattan(next, dst) + my_dist});
        previous.at(next) = cur.pos;
      }
    }
  }
  assert(0);
  return std::vector<Coord>();
}

Map::Map(MapBuilder&& builder)
: data(std::move(builder.data)),
  doors(std::move(builder.doors)) {

  rebuild_cache();
}

// TODO: evaluate whether a dynamic algorithm (eg union-find) makes sense here
void Map::rebuild_equivalence_classes() {
  for (size_t i = 0; i < equivalent_components.size(); ++i) {
    equivalent_components[i] = i;
  }

  for (const auto& door : doors) {
    if (door.second.open) {
      // TODO: O(n^2) because long-term plan is to use refcounts. Also n<=4;P
      for (auto it = door.second.adjacent_components.begin();
           it < door.second.adjacent_components.end();
           ++it) {
        for (auto jt = it + 1; jt < door.second.adjacent_components.end(); ++jt) {
          auto rep1 = find_representative(equivalent_components, *it);
          auto rep2 = find_representative(equivalent_components, *jt);
          equivalent_components[rep1] = rep2;
        }
      }
    }
  }
}

void Map::rebuild_cache() {
  component = Grid<uint32_t>(size(), COMPONENT_UNKNOWN);
  component.fill(COMPONENT_UNKNOWN);

  // Mark doors as having multiple components.
  for (const auto& door : doors) {
    component.at(door.first) = COMPONENT_MULTIPLE;
  }

  // Identify connected components, stopping at walls and doors. We also mark
  // doors and walls with the appropriate component value.
  uint16_t restart_row = 0;
  int index = 0;
  while (restart_row < size().row) {
    uint16_t restart_col = 0;
    while (restart_col < size().col) {
      if (component.at(restart_row, restart_col) == COMPONENT_UNKNOWN &&
          is_transparent({restart_row, restart_col})) {
        // Flood fill component.
        std::stack<Coord> todo;
        todo.push({restart_row, restart_col});
        component.at(restart_row, restart_col) = index++;

        while (!todo.empty()) {
          auto cur = todo.top();
          todo.pop();
          // Explore neighbors.
          for (const auto& n : data.get_adjacent(cur)) {
            if (is_transparent(n) && component.at(n) == COMPONENT_UNKNOWN) {
              todo.push(n);
              component.at(n) = component.at(cur);
            }
          }
        }
      } else if (component.at({restart_row, restart_col}) != COMPONENT_MULTIPLE &&
                 !is_transparent({restart_row, restart_col})) {
        component.at(restart_row, restart_col) = COMPONENT_IMPASSABLE;
      }
      ++restart_col;
    }
    ++restart_row;
  }

  // Identify all adjacent components for doors.
  for (auto& door : doors) {
    for (const auto& n : data.get_adjacent(door.first)) {
      int c = component.at(n.row, n.col);
      if (c >= 0 && std::find(
                door.second.adjacent_components.begin(),
                door.second.adjacent_components.end(),
                c
            ) == door.second.adjacent_components.end()) {
        door.second.adjacent_components.push_back(c);
      }
    }
  }

  equivalent_components.resize(index);
  rebuild_equivalence_classes();
}

}  // namespace pathfinder
}  // namespace suncatcher
