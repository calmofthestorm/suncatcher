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

MapBuilder::MapBuilder(Coord size, uint_least8_t cost)
: data(size, cost),
  doors() { }

void MapBuilder::add_door(Coord cell, bool open, uint_least8_t cost_open,
                          uint_least8_t cost_closed) {
  assert(doors.find(cell) == doors.end());
  doors[cell] = {open, cost_open, cost_closed, {}};
  data.at(cell) = open ? cost_open : cost_closed;
}

Map::~Map() {
  assert(outstanding_mutators == 0);
}

void Map::print_map(std::ostream& os, const Path& path) const {
  // lol this is so bad...debugging only tho...
  std::set<Coord> in_path(path.get_path().begin(), path.get_path().end());
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
        os << (data.at(j, i) == PATH_COST_INFINITE ? '*' : ' ');
      }
    }
    os << std::endl;
  }
}

void Map::print_components(std::ostream& os) const {
  for (uint16_t j = 0; j < size().row; ++j) {
    for (uint16_t i = 0; i < size().col; ++i) {
      uint_least32_t c = component.at(j, i);
      auto door = doors.find({j, i});
      if (door != doors.end()) {
        if (doors.find({j, i})->second.open) {
          os << '_';
        } else {
          os << 'd';
        }
      } else if (data.at(j, i) != PATH_COST_INFINITE) {
        os << (char)(c + 'A');
      } else {
        os << '*';
      }
    }
    os << std::endl;
  }
  os << std::endl;
}

void Map::print_equivalence_classes(std::ostream& os) const {
  for (uint16_t j = 0; j < size().row; ++j) {
    for (uint16_t i = 0; i < size().col; ++i) {
      if (component.at(j, i) == COMPONENT_IMPASSABLE) {
        os << ' ';
      } else {
        uint_least32_t c = dynamic_component.lookup(component.at(j, i));
        if (c < 25) {
          os << (char)(c + 'A');
        } else {
          os << ' ' << c << ' ';
        }
      }
    }
    os << std::endl;
  }
}

bool Map::path_exists(Coord a, Coord b) const {
  return (is_passable(a) && is_passable(b) &&
          dynamic_component.equivalent(component.at(a), component.at(b)));
}

Path Map::path(Coord src, Coord dst) const {
  Grid<int> expanded(size().row, size().col, 0);
  Grid<Coord> previous(size().row, size().col, {(uint16_t)-1, (uint16_t)-1});
  size_t num_expanded = 0;

  if (!data.check_bounds(src) || !data.check_bounds(dst)) {
    return Path({}, -1);
  }

  // A path exists iff they are in the same equivalence class and both are
  // passable squares.
  if (!is_passable(src) || !is_passable(dst) || !path_exists(src, dst)) {
    return Path(std::vector<Coord>(), -1);
  }

  // Order intentional, if A is impassable there is no path from A to A.
  if (src == dst) {
    return Path(std::vector<Coord>{src}, 0);
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
      std::reverse(rval.begin(), rval.end());
      return Path(std::move(rval), distance.at(dst));
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
  return Path(std::vector<Coord>(), -1);
}

Map::Map(MapBuilder&& builder)
: version(0),
  outstanding_mutators(0),
  data(std::move(builder.data)),
  doors(std::move(builder.doors)),
  door_base_component(0) {

  clear_cache();
}


MapMutator Map::get_mutator() {
  return MapMutator(this);
}


// Changes the world immediately, doing all necessary computations.
void Map::mutate(MapMutator&& mutation) {
  assert(mutation.version == version);
  ++version;
  for (const auto& it : mutation.mutations) {
    auto door_iter = doors.find(it.first);
    switch(it.second.kind) {
      case MapMutator::Mutation::Kind::CREATE_DOOR:
        assert(door_iter == doors.end());
        assert(it.second.cost != PATH_COST_INFINITE);
        doors[it.first] = Door{it.second.state, it.second.cost, PATH_COST_INFINITE, {}};
        if (it.second.state) {
          data.at(it.first) = it.second.cost;
        } else {
          data.at(it.first) = PATH_COST_INFINITE;
        }
        break;

      case MapMutator::Mutation::Kind::REMOVE_DOOR:
        assert(door_iter != doors.end());
        doors.erase(door_iter);
        data.at(it.first) = it.second.cost;
        break;

      case MapMutator::Mutation::Kind::SET_COST:
        assert(door_iter == doors.end());
        data.at(it.first) = it.second.cost;
        break;

      case MapMutator::Mutation::Kind::UPDATE_DOOR:
        assert(door_iter != doors.end());
        door_iter->second.open ^= it.second.state;
        // Internally we use PATH_COST_INFINITE to represent the current value.
        if (it.second.cost != PATH_COST_INFINITE) {
          door_iter->second.cost_open = it.second.cost;
        }
        if (door_iter->second.open) {
          data.at(it.first) = door_iter->second.cost_open;
        } else {
          data.at(it.first) = PATH_COST_INFINITE;
        }
        break;

      default:
        assert(0);

    }
  }
  clear_cache(); // TODO: lol dynamic is the whole point;)
}

// Forces recomputation of all cached information.
void Map::clear_cache() {
  dynamic_component = decltype(dynamic_component)();
  auto is_transparent = [this] (Coord cell) {
    if (component.at(cell) == COMPONENT_MULTIPLE) {
      return false;
    } else {
      return data.at(cell) != PATH_COST_INFINITE;
    }
  };

  component = Grid<uint_least32_t>(size(), COMPONENT_UNKNOWN);
  component.fill(COMPONENT_UNKNOWN);

  // Temporarily mark doors as having multiple components. We do this so we
  // can quickly detect if a cell is a door without having to search.
  for (const auto& door : doors) {
    component.at(door.first) = COMPONENT_MULTIPLE;
  }

  // Identify connected components, stopping at walls and doors. We also mark
  // doors and walls with the appropriate component value.
  uint16_t restart_row = 0;
  uint_least32_t index = 0;
  while (restart_row < size().row) {
    uint16_t restart_col = 0;
    while (restart_col < size().col) {
      if (component.at(restart_row, restart_col) == COMPONENT_UNKNOWN &&
          is_transparent({restart_row, restart_col})) {
        // Flood fill component.
        std::stack<Coord> todo;
        todo.push({restart_row, restart_col});
        dynamic_component.add_component(component.at(restart_row, restart_col) = index++);

        while (!todo.empty()) {
          auto cur = todo.top();
          todo.pop();
          // Explore neighbors, but only Manhattan adjacent ones.
          for (const auto& n : data.get_adjacent(cur, false)) {
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

  // Each door is its own connected component.
  door_base_component = index;
  for (const auto& door : doors) {
    dynamic_component.add_component(component.at(door.first) = index++);
  }

  #ifndef NDEBUG
  for (size_t j = 0;j < size().row; ++j) {
    for (size_t i = 0; i < size().col; ++i) {
      assert(
          component.at(j, i) < index ||
          component.at(j, i) == COMPONENT_IMPASSABLE
        );
    }
  }
  #endif

  // Dynamically union all open doors with their neighbors.
  for (auto& door : doors) {
    door.second.adjacent_components.clear();
    if (door.second.open) {
      uint_least32_t door_component = component.at(door.first);
      for (const auto& n : data.get_adjacent(door.first, false)) {
        if (component.at(n) != COMPONENT_IMPASSABLE &&
            (!is_door(n) || doors.find(n)->second.open)) {
          dynamic_component.add_edge(component.at(n), door_component);
        }
      }
    }
  }
}

MapBuilder::MapBuilder(std::istream& is) {
  std::vector<Coord> door_index_to_coords;

  std::string line;
  size_t rows, cols;
  is >> rows >> cols;
  std::getline(is, line);
  assert(is);
  *this = MapBuilder({(uint16_t)rows, (uint16_t)cols}, 1);
  for (uint16_t row = 0; row < rows; ++row) {
    std::getline(is, line);
    assert(is);
    assert(line.size() >= cols);
    for (uint16_t col = 0; col < cols; ++col) {
      cost({row, col}) = (line[col] == '*' ? suncatcher::pathfinder::PATH_COST_INFINITE : 1);
      if (line[col] == 'd') {
        add_door({row, col}, true, 1, suncatcher::pathfinder::PATH_COST_INFINITE);
        door_index_to_coords.push_back({row, col});
      } else if (line[col] == 'D') {
        add_door({row, col}, false, 1, suncatcher::pathfinder::PATH_COST_INFINITE);
        door_index_to_coords.push_back({row, col});
      }
    }
  }
}

}  // namespace pathfinder
}  // namespace suncatcher
