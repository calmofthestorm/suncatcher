#include "Map.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <tuple>

#include "util.h"

namespace suncatcher {
namespace pathfinder {

const uint32_t COMPONENT_MULTIPLE = (uint32_t)-1;
const uint32_t COMPONENT_UNKNOWN = (uint32_t)-2;
const uint32_t COMPONENT_IMPASSABLE = (uint32_t)-3;

using suncatcher::util::Grid;
using suncatcher::util::find_representative;
using suncatcher::util::manhattan;

void Map::print_map(std::ostream& os) const {
  size_t index = 0;
  for (uint16_t j = 0; j < size.row; ++j) {
    for (uint16_t i = 0; i < size.col; ++i) {
      char cell = data.at(j, i);
      if (cell == 'd' || cell == '_') {
        auto d = doors.at({j, i});
        if (d.open) {
          os << index++;
        } else {
          os << "\033[1m" << index++ << "\033[0m";
        }
      } else {
        os << cell;
      }
    }
    os << std::endl;
  }
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
  for (uint16_t j = 0; j < size.row; ++j) {
    for (uint16_t i = 0; i < size.col; ++i) {
      if (data.at(j, i) == ' ') {
        int c = component.at(j, i);
        if (c == -2) {
          os << ' ';
        } else {
          os << (char)(c + 'A');
        }
      } else {
        os << data.at(j, i);
      }
    }
    os << std::endl;
  }
}

void Map::print_equivalence_classes(std::ostream& os) const {
  for (uint16_t j = 0; j < size.row; ++j) {
    for (uint16_t i = 0; i < size.col; ++i) {
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

bool Map::is_passable(const pathfinder::Coord& cell) const {
  bool rval = data.check_bounds(cell) && (data.at(cell) != '*');
  assert((component.at(cell) != COMPONENT_IMPASSABLE) == rval);
  return rval;
}

std::vector<Coord> Map::path(const Coord& src, const Coord& dst) const {
  Grid<int> expanded(size.row, size.col, 0);
  Grid<Coord> previous(size.row, size.col, {(uint16_t)-1, (uint16_t)-1});
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

  Grid<float> distance(size.row, size.col, INFINITY);
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
    fringe.pop();
    expanded.at(cur.pos) = 1;
    ++num_expanded;

    if (cur.pos == dst) {
      // size_t j = 0;
      // for (const auto& row : expanded.get_backing()) {
      //   size_t i = 0;
      //   for (const auto& col : row) {
      //     if (col == 1) {
      //       std::cout << '.';
      //       // std::cout << ' ' << distance.at(j, i) + manhattan(dst, {j, i})<< ' ';
      //     } else {
      //       std::cout << data.at(j, i);
      //     }
      //     ++i;
      //   }
      //   ++j;
      //   std::cout << std::endl;
      // }
      // std::cout << num_expanded << std::endl;
      // std::cout << std::endl;
      // std::cout << std::endl;
      // std::cout << std::endl;
      // std::cout << std::endl;
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
      // if (distance.at(next) > 1 + distance.at(cur.pos) && (
      float my_dist = distance.at(cur.pos) + 1;
      if (cur.pos.row != next.row && cur.pos.col != next.col) {
        my_dist += 0.4142135623730951;
      }
      if (distance.at(next) > my_dist &&
          !expanded.at(next) && (
            data.at(next) == ' ' ||
            data.at(next) == '_')) {
        distance.at(next) = my_dist;
        fringe.push({next, manhattan(next, dst) + my_dist});
        previous.at(next) = cur.pos;
      }
    }
  }
  assert(0);
}

Map::Map(std::istream& is) {
  size.row = 0;
  size.col = 0;

  // Read in map.
  {
    std::string line;
    std::vector<std::vector<uint_least8_t>> temp_map;
    while (getline(is, line)) {
      size.col = std::max((uint_least16_t)line.size(), (uint_least16_t)size.col);
      ++size.row;
      temp_map.push_back(std::vector<uint_least8_t>(line.begin(), line.end()));
    }

    // Make it a rectangle if necessary.
    for (auto& line : temp_map) {
      line.resize(size.col, '*');
    }

    Grid<uint_least8_t> bees(std::move(temp_map), size);
    std::swap(data, bees);
  }

  // Create doors, validate map, and set up structures for flood fill.
  {
    component.resize(size.row, size.col, COMPONENT_UNKNOWN);
    for (uint16_t j = 0;j < size.row; ++j) {
      for (uint16_t i = 0; i < size.col; ++i) {
        switch (data.at(j, i)) {
          case 'd':
            doors[{j, i}].open = true;
            data.at(j, i) = '_';
            component.at(j, i) = COMPONENT_MULTIPLE;
            break;

          case ' ':
            break;

          case '*':
            component.at(j, i) = COMPONENT_IMPASSABLE;
            break;

          default:
            assert(0);
        }
      }
    }
  }

  // Identify connected components, stopping at walls and doors.
  uint16_t restart_row = 0;
  int index = 0;
  while (restart_row < size.row) {
    uint16_t restart_col = 0;
    while (restart_col < size.col) {
      if (component.at(restart_row, restart_col) == COMPONENT_UNKNOWN &&
          data.at(restart_row, restart_col) == ' ') {
        // Flood fill component.
        std::stack<Coord> todo;
        todo.push({restart_row, restart_col});
        component.at(restart_row, restart_col) = index++;

        while (!todo.empty()) {
          auto cur = todo.top();
          todo.pop();
          // Explore neighbors.
          for (const auto& n : data.get_adjacent(cur)) {
            if (data.at(n) == ' ' && component.at(n.row, n.col) == COMPONENT_UNKNOWN) {
              todo.push(n);
              component.at(n.row, n.col) = component.at(cur.row, cur.col);
            }
          }
        }
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

  // Set up equivalence classes that dynamically update in response to doors.
  // Think union find light. All doors start open.
  equivalent_components.resize(index, 0);

  std::cout << "Map loaded with " << index << " components!" << std::endl;
}

}  // namespace pathfinder
}  // namespace suncatcher
