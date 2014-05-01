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

MapBuilder::MapBuilder(Coord size, uint_least8_t default_cost)
: data(size, default_cost),
  dynamic_updates(true),
  doors() { }

void MapBuilder::add_door(Coord cell, bool open, uint_least8_t cost_open,
                          uint_least8_t cost_closed) {
  assert(doors.find(cell) == doors.end());
  doors[cell] = {open, cost_open, cost_closed};
  data.at(cell) = open ? cost_open : cost_closed;
}

Map::~Map() {
  assert(outstanding_mutators == 0);
}

void Map::print_map(std::ostream& os, const Path& path_to_show) const {
  std::set<Coord> in_path(path_to_show.get_path().begin(), path_to_show.get_path().end());
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

void Map::print_colors(std::ostream& os) const {
  for (uint16_t j = 0; j < size().row; ++j) {
    for (uint16_t i = 0; i < size().col; ++i) {
      int_least32_t c = color.at(j, i);
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

void Map::print_static_components(std::ostream& os) const {
  for (uint16_t j = 0; j < size().row; ++j) {
    for (uint16_t i = 0; i < size().col; ++i) {
      if (color.at(j, i) == COLOR_IMPASSABLE) {
        os << ' ';
      } else {
        int_least32_t c = static_component.at(color.at(j, i));
        char start = c < 0 ? 'Z' : 'A';
        if (c < 25) {
          os << (char)(c + start);
        } else {
          os << ' ' << c << ' ';
        }
      }
    }
    os << std::endl;
  }
}

void Map::print_dynamic_components(std::ostream& os) const {
  for (uint16_t j = 0; j < size().row; ++j) {
    for (uint16_t i = 0; i < size().col; ++i) {
      if (color.at(j, i) == COLOR_IMPASSABLE) {
        os << ' ';
      } else {
        int_least32_t c = dynamic_component.lookup(static_component.at(color.at(j, i)));
        char start = c < 0 ? 'Z' : 'A';
        if (c < 25) {
          os << (char)(c + start);
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
          dynamic_component.equivalent(
              static_component.at(color.at(a)),
              static_component.at(color.at(b))
            ));
}

Path Map::path(Coord src, Coord dst) const {
  Grid<uint_fast8_t> expanded(size(), 0);
  Grid<Coord> previous(size(), {(uint16_t)-1, (uint16_t)-1});
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

  Grid<float> distance(size(), INFINITY);
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
  dynamic_updates(builder.dynamic_updates) {

  clear_cache();
}


MapMutator Map::get_mutator() {
  return MapMutator(this, version);
}


// Caller's responsibility to set the newly transparent cell's cost AFTER
// the call.
void Map::wall_to_transparent(Coord cell) {
  assert(color.at(cell) == COLOR_IMPASSABLE);

  // Set its color to the first transparent neighbor (Manhattan) found.
  // For the rest, union the static components.
  for (const auto& n : data.get_adjacent(cell, false)) {
    if (is_transparent(n)) {
      if (color.at(cell) == COLOR_IMPASSABLE) {
        // Not yet chosen the former wall's new color
        color.at(cell) = color.at(n);
      } else {
        // Former wall has a color, merge them.
        static_component.union_sets(color.at(cell), color.at(n));
      }
    }
  }

  // Wall was surrounded by walls (or other doors, etc -- anything
  // that we can't merge with the new square). Allocate it a new
  // color, and thus a new static component and dynamic component.
  if (color.at(cell) == COLOR_IMPASSABLE) {
    dynamic_component.add_component(static_component[next_component_color]);
    color.at(cell) = next_component_color++;
  }
}


void Map::closed_door_to_open_door(Coord cell, DoorIter door_iter) {
  int_least32_t door_static_component = static_component.at(color.at(cell));
  for (const auto& n : data.get_adjacent(cell, false)) {
    if (color.at(n) != COLOR_IMPASSABLE &&
        (!is_door(n) || (doors.find(n)->second.open && n < cell))) {
      dynamic_component.add_edge(
          static_component.at(color.at(n)),
          door_static_component
        );
    }
  }
  door_iter->second.open = true;
}


void Map::closed_door_to_wall(
    Coord cell,
    DoorIter door_iter,
    uint8_t cost
  ) {
  color.at(cell) = COLOR_IMPASSABLE;
  doors.erase(door_iter);
  data.at(cell) = cost;
}


void Map::open_door_to_closed_door(Coord cell, DoorIter door_iter) {
  dynamic_component.isolate_component(static_component.at(color.at(cell)));
  door_iter->second.open = false;
}


void Map::transparent_to_wall(Coord cell) {
  // int_least32_t old_color = color.at(cell);
  // data.at(cell) = PATH_COST_INFINITE;
  // color.at(cell) = COLOR_IMPASSABLE;
  // for (const auto& seed : data.get_adjacent(cell, false)) {
  //   if (color.at(seed) == old_color && is_transparent(seed)) {
  //     std::stack<Coord> todo;
  //     todo.push(seed);
  //     dynamic_component.add_component(static_component[next_component_color]);
  //     color.at(seed) = next_component_color;
  //
  //     while (!todo.empty()) {
  //       auto cur = todo.top();
  //       todo.pop();
  //       // Explore neighbors, but only Manhattan adjacent ones.
  //       for (const auto& n : data.get_adjacent(cur, false)) {
  //         if (is_transparent(n) && color.at(n) == old_color) {
  //           todo.push(n);
  //           color.at(n) = next_component_color;
  //         }
  //       }
  //     }
  //     ++next_component_color;
  //   }
  // }
  //
  // // Recalculate dynamic components.
  // dynamic_component.isolate_component(static_component.at(old_color));
  // for (const auto& door : doors) {
  //   if (door.second.open) {
  //     int_least32_t door_static_component = static_component.at(color.at(cell));
  //     for (const auto& n : data.get_adjacent(door.first, false)) {
  //       if (color.at(n) != COLOR_IMPASSABLE &&
  //           (!is_door(n) || (doors.find(n)->second.open && n < door.first))) {
  //         dynamic_component.add_edge(
  //             static_component.at(color.at(n)),
  //             door_static_component
  //           );
  //       }
  //     }
  //   }
  // }
}


Map::DoorIter Map::wall_to_closed_door (
    Coord cell,
    uint8_t cost_closed,
    uint8_t cost_open
  ) {
  data.at(cell) = PATH_COST_INFINITE;
  dynamic_component.add_component(static_component[next_door_color]);
  color.at(cell) = next_door_color--;
  return doors.insert(std::make_pair(cell, Door{false, cost_closed, cost_open})).first;
}


// Changes the world immediately, doing all necessary computations.
void Map::mutate(MapMutator&& mutation) {
  assert(mutation.version == version);
  assert(mutation.map == this);
  ++version;
  bool dirty = false;
  for (const auto& it : mutation.mutations) {
    bool state = it.second.state;
    uint8_t cost = it.second.cost;
    Coord cell = it.first;
    auto door_iter = doors.find(cell);
    switch(it.second.kind) {
      case MapMutator::Mutation::Kind::CREATE_DOOR:
        assert(door_iter == doors.end());
        assert(cost != PATH_COST_INFINITE);
        assert(!is_door(cell));

        // Create a wall, if cell is not already one.
        if (data.at(cell) != PATH_COST_INFINITE) {
          transparent_to_wall(cell);
          dirty = true;
        }

        // Convert wall into closed door.
        door_iter = wall_to_closed_door(cell, PATH_COST_INFINITE, cost);

        // If necessary, open it.
        if (state) {
          closed_door_to_open_door(cell, door_iter);
        }

        doors[cell] = Door{state, cost, PATH_COST_INFINITE};
        if (state) {
          data.at(cell) = cost;
        } else {
          data.at(cell) = PATH_COST_INFINITE;
        }
        break;

      case MapMutator::Mutation::Kind::REMOVE_DOOR:
        assert(door_iter != doors.end());
        if (door_iter->second.open) {
          if (cost == PATH_COST_INFINITE) {
            // Open door to wall.

            // Close the door.
            open_door_to_closed_door(cell, door_iter);

            // Turn the closed door into a wall.
            closed_door_to_wall(cell, door_iter, cost);
          } else {
            // Open door to transparent.

            // Close it
            open_door_to_closed_door(cell, door_iter);

            // Turn it into a wall.
            closed_door_to_wall(cell, door_iter, cost);

            // Remove the wall.
            wall_to_transparent(cell);
            data.at(cell) = cost;
          }
        } else {
          if (cost == PATH_COST_INFINITE) {
            // Closed door to wall.
            // Nothing to do besides remove the door entry and update
            // the component.
            color.at(cell) = COLOR_IMPASSABLE;
            doors.erase(door_iter);
            data.at(cell) = cost;
          } else {
            // Closed door to transparent.
            // Turn it into a wall, then remove the wall.
            color.at(cell) = COLOR_IMPASSABLE;
            data.at(cell) = PATH_COST_INFINITE;
            doors.erase(door_iter);
            wall_to_transparent(cell);
            data.at(cell) = cost;
          }
        }
        break;

      case MapMutator::Mutation::Kind::SET_COST:
        assert(door_iter == doors.end());

        // Removing a wall.
        if (data.at(cell) == PATH_COST_INFINITE &&
            cost != PATH_COST_INFINITE) {
          wall_to_transparent(cell);
        } else if (data.at(cell) != PATH_COST_INFINITE &&
                   cost == PATH_COST_INFINITE) {
          // Building a wall. Flood fill required.
          transparent_to_wall(cell);
          dirty = true;
        }
        data.at(cell) = cost;
        // else -- either PATH_COST_INFINITE -> PATH_COST_INFINITE (nop)
        // or passable value to passable value -- colors are not affected
        // (even though path cost might be).
        break;

      case MapMutator::Mutation::Kind::UPDATE_DOOR:
        assert(door_iter != doors.end());
        door_iter->second.open ^= state;

        // Internally we use PATH_COST_INFINITE to represent the current value.
        // Only MapMutator::*door* and Map::mutate need to know this.
        if (cost != PATH_COST_INFINITE) {
          door_iter->second.cost_open = cost;
        }

        // Update map costs.
        if (door_iter->second.open) {
          data.at(cell) = door_iter->second.cost_open;
        } else {
          data.at(cell) = PATH_COST_INFINITE;
        }

        // Maintain links if door changed state.
        if (state) {
          if (door_iter->second.open) {
            closed_door_to_open_door(cell, door_iter);
          } else {
            open_door_to_closed_door(cell, door_iter);
          }
        }
        break;

      default:
        assert(0);

    }
  }

  if (dirty || !dynamic_updates) {
    clear_cache(); // TODO: lol dynamic is the whole point;)
  }
}

// Forces recomputation of all cached information.
void Map::clear_cache() {
  dynamic_component = decltype(dynamic_component)();

  // Temporary function for determining transparancy since doors have not
  // yet been set to appropriate colors.
  auto is_transparent_temporary = [this] (Coord cell) {
    if (color.at(cell) == COLOR_MULTIPLE) {
      return false;
    } else {
      return data.at(cell) != PATH_COST_INFINITE;
    }
  };

  color = Grid<int_least32_t>(size(), COLOR_UNKNOWN);
  color.fill(COLOR_UNKNOWN);

  // Temporarily mark doors as having multiple colors. We do this so we
  // can quickly detect if a cell is a door without having to search.
  for (const auto& door : doors) {
    color.at(door.first) = COLOR_MULTIPLE;
  }

  // Identify colors (components connected by Manhattan-adjacent transparant
  // squares), stopping at walls and doors. We also mark walls with the
  // appropriate color value (COLOR_IMPASSABLE)
  Coord restart{0, 0};
  int_least32_t index = 0;
  while (restart.row < size().row) {
    restart.col = 0;
    while (restart.col < size().col) {
      if (color.at(restart) == COLOR_UNKNOWN &&
          is_transparent_temporary(restart)) {
        // Flood fill colors.
        std::stack<Coord> todo;
        todo.push(restart);
        color.at(restart) = index;
        dynamic_component.add_component(static_component[index]);

        while (!todo.empty()) {
          auto cur = todo.top();
          todo.pop();
          // Explore neighbors, but only Manhattan adjacent ones.
          for (const auto& n : data.get_adjacent(cur, false)) {
            if (is_transparent_temporary(n) && color.at(n) == COLOR_UNKNOWN) {
              todo.push(n);
              color.at(n) = index;
            }
          }
        }
        ++index;
      } else if (color.at(restart) != COLOR_MULTIPLE &&
                 !is_transparent_temporary(restart)) {
        color.at(restart) = COLOR_IMPASSABLE;
      }
      ++restart.col;
    }
    ++restart.row;
  }
  next_component_color = index;

  // Each door is its own color.
  index = -1;
  for (const auto& door : doors) {
    color.at(door.first) = index;
    dynamic_component.add_component(static_component[index]);
    --index;
  }
  next_door_color = index;

  // Dynamically union all open doors with their neighbors.
  for (auto& door : doors) {
    if (door.second.open) {
      int_least32_t door_static_component = static_component.at(color.at(door.first));
      for (const auto& n : data.get_adjacent(door.first, false)) {
        if (color.at(n) != COLOR_IMPASSABLE &&
            (!is_door(n) || (doors.find(n)->second.open && n < door.first))) {
          dynamic_component.add_edge(
              static_component.at(color.at(n)),
              door_static_component
            );
        }
      }
    }
  }
}

MapBuilder::MapBuilder(std::istream& is) {
  std::vector<Coord> door_index_to_coords;

  std::string line;
  Coord size;
  is >> size.row >> size.col;
  std::getline(is, line);
  assert(is);
  *this = MapBuilder(size, 1);
  for (uint16_t row = 0; row < size.row; ++row) {
    std::getline(is, line);
    assert(is);
    assert(line.size() >= size.col);
    for (uint16_t col = 0; col < size.col; ++col) {
      Coord cell{row, col};
      cost(cell) = (line[col] == '*' ? suncatcher::pathfinder::PATH_COST_INFINITE : 1);
      if (line[col] == 'd') {
        add_door(cell, true, 1, suncatcher::pathfinder::PATH_COST_INFINITE);
        door_index_to_coords.push_back(cell);
      } else if (line[col] == 'D') {
        add_door(cell, false, 1, suncatcher::pathfinder::PATH_COST_INFINITE);
        door_index_to_coords.push_back(cell);
      }
    }
  }
}

}  // namespace pathfinder
}  // namespace suncatcher
