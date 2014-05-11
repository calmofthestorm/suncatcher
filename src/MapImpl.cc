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

#include "suncatcher/MapImpl.hh"

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

#include "suncatcher.hh"
#include "suncatcher/util/util.hh"
#include "suncatcher/MapBuilder.hh"
#include "suncatcher/MapMutator.hh"
#include "suncatcher/Coord.hh"

namespace suncatcher {
namespace pathfinder {

using suncatcher::util::Grid;
using suncatcher::util::find_representative;
using suncatcher::util::manhattan;


MapImpl::MapImpl(const MapMutator& mutation, bool incremental)
: MapImpl(*mutation.view.map) {


  for (const auto& it : mutation.mutations) {
    bool state = it.second.state;
    uint_least8_t cost = it.second.cost;
    Coord cell = it.first;
    auto door_iter = doors.find(cell);
    switch (it.second.kind) {
      case MapMutator::Mutation::Kind::CREATE_DOOR:
        incremental_create_door(cell, state, cost);
        break;

      case MapMutator::Mutation::Kind::REMOVE_DOOR:
        incremental_remove_door(cell, state, cost);
        break;

      case MapMutator::Mutation::Kind::SET_COST:
        incremental_set_cost(cell, state, cost);
        break;

      case MapMutator::Mutation::Kind::UPDATE_DOOR:
        incremental_update_door(cell, state, cost);
        break;

      default:
        assert(0);
    }
  }

  if (!incremental) {
    rebuild();
  }
}


// Caller's responsibility to set the newly transparent cell's cost AFTER
// the call.
void MapImpl::incremental_wall_to_transparent(Coord cell) {
  assert(graph.get_color(cell) == COLOR_IMPASSABLE);

  // Set its color to the first transparent neighbor (Manhattan) found.
  // For the rest, union the static components.
  std::vector<Coord> adjacent;
  graph.get_adjacent(cell, false, adjacent);
  for (const auto& n : adjacent) {
    if (is_transparent(n)) {
      if (graph.get_color(cell) == COLOR_IMPASSABLE) {
        // Not yet chosen the former wall's new color
        graph.set_color(cell, graph.get_color(n));
      } else {
        // Former wall has a color, merge them.
        static_component.union_sets(graph.get_color(cell), graph.get_color(n));
        dynamic_component.union_sets(graph.get_color(cell), graph.get_color(n));
      }
    }
  }

  // Wall was surrounded by walls (or other doors, etc -- anything
  // that we can't merge with the new square). Allocate it a new
  // color, and thus a new static component and dynamic component.
  if (graph.get_color(cell) == COLOR_IMPASSABLE) {
    static_component[next_component_color];
    dynamic_component[next_component_color];
    graph.set_color(cell, next_component_color++);
  }

  for (const auto& n : adjacent) {
    if (is_passable(n)) {
      dynamic_component.union_sets(graph.get_color(cell), graph.get_color(n));
    }
  }
}


void MapImpl::incremental_closed_door_to_open_door(Coord cell, DoorIter door_iter) {
  int_least32_t door_static_component = static_component.at(graph.get_color(cell));
  std::vector<Coord> adjacent;
  graph.get_adjacent(cell, false, adjacent);
  for (const auto& n : adjacent) {
    if (is_passable(n)) {
      dynamic_component.union_sets(
          static_component.at(graph.get_color(n)),
          door_static_component
        );
    }
  }
  door_iter->second.open = true;
}


void MapImpl::incremental_closed_door_to_wall(
    Coord cell,
    DoorIter door_iter,
    uint_least8_t cost
  ) {
  graph.set_color(cell, COLOR_IMPASSABLE);
  doors.erase(door_iter);
  graph.set_cost(cell, cost);
}


void MapImpl::incremental_open_door_to_closed_door(Coord cell, DoorIter door_iter) {
  door_iter->second.open = false;
  graph.set_cost(door_iter->first, PATH_COST_INFINITE);
  incremental_regenerate_dynamic_components();
}


void MapImpl::incremental_regenerate_dynamic_components() {
  // Recalculate dynamic components.
  dynamic_component = static_component;
  for (const auto& door : doors) {
    if (door.second.open) {
      std::vector<Coord> adjacent;
      graph.get_adjacent(door.first, false, adjacent);
      for (const auto& n : adjacent) {
        if (is_passable(n)) {
          dynamic_component.union_sets(
              graph.get_color(n),
              graph.get_color(door.first)
            );
        }
      }
    }
  }
}


void MapImpl::incremental_transparent_to_wall(Coord cell) {
  int_least32_t old_static_component = static_component.at(graph.get_color(cell));
  graph.set_cost(cell, PATH_COST_INFINITE);
  graph.set_color(cell, COLOR_IMPASSABLE);
  std::vector<Coord> adjacent, neighbors;
  graph.get_adjacent(cell, false, adjacent);
  for (const auto& seed : adjacent) {
    if (is_transparent(seed) && 
        static_component.at(graph.get_color(seed)) == old_static_component) {
      std::stack<Coord> todo;
      todo.push(seed);
      dynamic_component[next_component_color];
      static_component[next_component_color];
      graph.set_color(seed, next_component_color);

      while (!todo.empty()) {
        auto cur = todo.top();
        todo.pop();
        // Explore neighbors, but only Manhattan adjacent ones.
        graph.get_adjacent(cur, false, neighbors);
        for (const auto& n : neighbors) {
          if (is_transparent(n) && static_component.at(graph.get_color(n)) == old_static_component) {
            todo.push(n);
            graph.set_color(n, next_component_color);
          }
        }
      }
      ++next_component_color;
    }
  }
  incremental_regenerate_dynamic_components();
}

MapImpl::DoorIter MapImpl::incremental_wall_to_closed_door (
    Coord cell,
    uint_least8_t cost_closed,
    uint_least8_t cost_open
  ) {
  graph.set_cost(cell, PATH_COST_INFINITE);
  dynamic_component[next_door_color];
  static_component[next_door_color];
  graph.set_color(cell, next_door_color--);
  return doors.insert(std::make_pair(
        cell,
        Door{false, cost_closed, cost_open})
      ).first;
}


void MapImpl::incremental_create_door(Coord cell, bool state, uint_least8_t cost) {
  assert(doors.find(cell) == doors.end());
  assert(cost != PATH_COST_INFINITE);
  assert(!is_door(cell));

  // Create a wall, if cell is not already one.
  if (graph.get_cost(cell) != PATH_COST_INFINITE) {
    incremental_transparent_to_wall(cell);
  }

  // Convert wall into closed door.
  auto door_iter = incremental_wall_to_closed_door(
      cell,
      PATH_COST_INFINITE,
      cost
    );

  // If necessary, open it.
  if (state) {
    incremental_closed_door_to_open_door(cell, door_iter);
  }

  doors[cell] = Door{state, cost, PATH_COST_INFINITE};
  if (state) {
    graph.set_cost(cell, cost);
  } else {
    graph.set_cost(cell, PATH_COST_INFINITE);
  }
}


void MapImpl::incremental_remove_door(Coord cell, bool state, uint_least8_t cost) {
  auto door_iter = doors.find(cell);
  assert(door_iter != doors.end());
  if (door_iter->second.open) {
    if (cost == PATH_COST_INFINITE) {
      // Goal: Open door to wall.

      // Close the door.
      incremental_open_door_to_closed_door(cell, door_iter);

      // Turn the closed door into a wall.
      incremental_closed_door_to_wall(cell, door_iter, cost);
    } else {
      // Goal: Open door to transparent.

      // Close it
      incremental_open_door_to_closed_door(cell, door_iter);

      // Turn it into a wall.
      incremental_closed_door_to_wall(cell, door_iter, cost);

      // Remove the wall.
      incremental_wall_to_transparent(cell);
      graph.set_cost(cell, cost);
    }
  } else {
    if (cost == PATH_COST_INFINITE) {
      // Closed door to wall.
      // Nothing to do besides remove the door entry and update
      // the component.
      graph.set_color(cell, COLOR_IMPASSABLE);
      doors.erase(door_iter);
      graph.set_cost(cell, cost);
    } else {
      // Closed door to transparent.
      // Turn it into a wall, then remove the wall.
      graph.set_color(cell, COLOR_IMPASSABLE);
      graph.set_cost(cell, PATH_COST_INFINITE);
      doors.erase(door_iter);
      incremental_wall_to_transparent(cell);
      graph.set_cost(cell, cost);
    }
  }
}


void MapImpl::incremental_set_cost(Coord cell, bool state, uint_least8_t cost) {
  assert(doors.find(cell) == doors.end());

  if (graph.get_cost(cell) == PATH_COST_INFINITE &&
      cost != PATH_COST_INFINITE) {
    // Removing a wall.
    incremental_wall_to_transparent(cell);
  } else if (graph.get_cost(cell) != PATH_COST_INFINITE &&
      cost == PATH_COST_INFINITE) {
    // Building a wall. Flood fill required.
    incremental_transparent_to_wall(cell);
  }
  graph.set_cost(cell, cost);
  // else -- either PATH_COST_INFINITE -> PATH_COST_INFINITE (nop)
  // or passable value to passable value -- colors are not affected
  // (even though path cost might be).
}


void MapImpl::incremental_update_door(Coord cell, bool state, uint_least8_t cost) {
  auto door_iter = doors.find(cell);
  assert(door_iter != doors.end());
  door_iter->second.open ^= state;

  // Internally we use PATH_COST_INFINITE to represent the current value.
  // Only MapMutator::*door* and Map::mutate need to know this.
  if (cost != PATH_COST_INFINITE) {
    door_iter->second.cost_open = cost;
  }

  // Update map costs.
  if (door_iter->second.open) {
    graph.set_cost(cell, door_iter->second.cost_open);
  } else {
    graph.set_cost(cell, PATH_COST_INFINITE);
  }

  // Maintain links if door changed state.
  if (state) {
    if (door_iter->second.open) {
      incremental_closed_door_to_open_door(cell, door_iter);
    } else {
      incremental_open_door_to_closed_door(cell, door_iter);
    }
  }
}


// Forces recomputation of all cached information.
void MapImpl::rebuild() {
  dynamic_component = decltype(dynamic_component)();
  static_component = decltype(static_component)();

  // Temporary function for determining transparancy since doors have not
  // yet been set to appropriate colors.
  auto is_transparent_temporary = [this] (Coord cell) {
    if (graph.get_color(cell) == COLOR_MULTIPLE) {
      return false;
    } else {
      return graph.get_cost(cell) != PATH_COST_INFINITE;
    }
  };

  graph.fill_color(COLOR_UNKNOWN);

  // Temporarily mark doors as having multiple colors. We do this so we
  // can quickly detect if a cell is a door without having to search.
  for (const auto& door : doors) {
    graph.set_color(door.first, COLOR_MULTIPLE);
  }

  // Identify colors (components connected by Manhattan-adjacent transparant
  // squares), stopping at walls and doors. We also mark walls with the
  // appropriate color value (COLOR_IMPASSABLE)
  int_least32_t index = 0;
  std::vector<Coord> adjacent;
  for (const Coord& restart : graph.domain()) {
    if (graph.get_color(restart) == COLOR_UNKNOWN &&
        is_transparent_temporary(restart)) {
      // Flood fill colors.
      std::stack<Coord> todo;
      todo.push(restart);
      graph.set_color(restart, index);
      static_component[index];
      dynamic_component[index];

      while (!todo.empty()) {
        auto cur = todo.top();
        todo.pop();
        // Explore neighbors, but only Manhattan adjacent ones.
        graph.get_adjacent(cur, false, adjacent);
        for (const auto& n : adjacent) {
          if (is_transparent_temporary(n) && graph.get_color(n) == COLOR_UNKNOWN) {
            todo.push(n);
            graph.set_color(n, index);
          }
        }
      }
      ++index;
    } else if (graph.get_color(restart) != COLOR_MULTIPLE &&
        !is_transparent_temporary(restart)) {
      graph.set_color(restart, COLOR_IMPASSABLE);
    }
  }
  next_component_color = index;

  // Each door is its own color.
  index = -1;
  for (const auto& door : doors) {
    graph.set_color(door.first, index);
    static_component[index];
    dynamic_component[index];
    --index;
  }
  next_door_color = index;
  incremental_regenerate_dynamic_components();
}


void MapImpl::print_map(std::ostream& os, bool number_doors, const Path& path_to_show) const {
  std::set<Coord> in_path(
      path_to_show.get_path().begin(),
      path_to_show.get_path().end()
    );
  size_t index = 0;
  for (const Coord& c : graph.domain()) {
    auto door = doors.find(c);
    if (c.col == 0) {
      if (c.row == 0) {
        os << std::endl;
      }
      os << std::endl;
    }
    if (in_path.find(c) != in_path.end()) {
      if (door == doors.end()) {
        os << '.';
      } else {
        ++index;
        os << "\033[1m.\033[0m";
      }
    } else if (door != doors.end()) {
      if (!door->second.open) {
        os << "\033[1m";
      }
      if (number_doors) {
        os << index++;
      } else {
        os << 'd';
      }
      if (!door->second.open) {
        os << "\033[0m";
      }
    } else {
      os << (graph.get_cost(c) == PATH_COST_INFINITE ? '*' : ' ');
    }
  }
  os << std::endl;
}


void MapImpl::print_colors(std::ostream& os) const {
  for (const Coord& coord : graph.domain()) {
    int_least32_t c = graph.get_color(coord);
    auto door = doors.find(coord);
    if (coord.col == 0) {
      if (coord.row == 0) {
        os << std::endl;
      }
      os << std::endl;
    }
    if (door != doors.end()) {
      if (doors.find(coord)->second.open) {
        os << '_';
      } else {
        os << 'd';
      }
    } else if (graph.get_cost(coord) != PATH_COST_INFINITE) {
      os << (char)(c + 'A');
    } else {
      os << '*';
    }
  }
  os << std::endl;
}


void MapImpl::print_static_components(std::ostream& os) const {
  for (const Coord& coord : graph.domain()) {
    if (coord.col == 0) {
      if (coord.row == 0) {
        os << std::endl;
      }
      os << std::endl;
    }
    if (graph.get_color(coord) == COLOR_IMPASSABLE) {
      os << '*';
    } else {
      int_least32_t c = static_component.at(graph.get_color(coord));
      char start = c < 0 ? 'z' : 'A';
      if (c < 25) {
        os << (char)(c + start);
      } else {
        os << ' ' << c << ' ';
      }
    }
  }
  os << std::endl;
}


void MapImpl::print_dynamic_components(std::ostream& os) const {
  for (const Coord& coord : graph.domain()) {
    if (coord.col == 0) {
      if (coord.row == 0) {
        os << std::endl;
      }
      os << std::endl;
    }
    if (graph.get_color(coord) == COLOR_IMPASSABLE) {
      os << '*';
    } else {
      int_least32_t c = dynamic_component.at(graph.get_color(coord));
      char start = c < 0 ? 'z' : 'A';
      if (c < 25) {
        os << (char)(c + start);
      } else {
        os << ' ' << c << ' ';
      }
    }
  }
  os << std::endl;
}


}  // namespace pathfinder
}  // namespace suncatcher
