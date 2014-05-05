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

namespace suncatcher {
namespace pathfinder {

using suncatcher::util::Grid;
using suncatcher::util::find_representative;
using suncatcher::util::manhattan;



MapImpl::MapImpl(MapBuilder&& builder)
: data(std::move(builder.data)),
  doors(std::move(builder.doors)) {

  rebuild();
}


MapImpl::MapImpl(const MapMutator& mutation, bool incremental) {
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
}


// Caller's responsibility to set the newly transparent cell's cost AFTER
// the call.
void MapImpl::incremental_wall_to_transparent(Coord cell) {
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


void MapImpl::incremental_closed_door_to_open_door(Coord cell, DoorIter door_iter) {
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


void MapImpl::incremental_closed_door_to_wall(
    Coord cell,
    DoorIter door_iter,
    uint_least8_t cost
  ) {
  color.at(cell) = COLOR_IMPASSABLE;
  doors.erase(door_iter);
  data.at(cell) = cost;
}


void MapImpl::incremental_open_door_to_closed_door(Coord cell, DoorIter door_iter) {
  dynamic_component.isolate_component(static_component.at(color.at(cell)));
  door_iter->second.open = false;
}


void MapImpl::incremental_transparent_to_wall(Coord cell) {
  int_least32_t old_color = color.at(cell);
  data.at(cell) = PATH_COST_INFINITE;
  color.at(cell) = COLOR_IMPASSABLE;
  for (const auto& seed : data.get_adjacent(cell, false)) {
    if (color.at(seed) == old_color && is_transparent(seed)) {
      std::stack<Coord> todo;
      todo.push(seed);
      dynamic_component.add_component(static_component[next_component_color]);
      color.at(seed) = next_component_color;

      while (!todo.empty()) {
        auto cur = todo.top();
        todo.pop();
        // Explore neighbors, but only Manhattan adjacent ones.
        for (const auto& n : data.get_adjacent(cur, false)) {
          if (is_transparent(n) && color.at(n) == old_color) {
            todo.push(n);
            color.at(n) = next_component_color;
          }
        }
      }
      ++next_component_color;
    }
  }

  // Recalculate dynamic components.
  dynamic_component.isolate_component(static_component.at(old_color));
  for (const auto& door : doors) {
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


MapImpl::DoorIter MapImpl::incremental_wall_to_closed_door (
    Coord cell,
    uint_least8_t cost_closed,
    uint_least8_t cost_open
  ) {
  data.at(cell) = PATH_COST_INFINITE;
  dynamic_component.add_component(static_component[next_door_color]);
  color.at(cell) = next_door_color--;
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
  if (data.at(cell) != PATH_COST_INFINITE) {
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
    data.at(cell) = cost;
  } else {
    data.at(cell) = PATH_COST_INFINITE;
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
      incremental_wall_to_transparent(cell);
      data.at(cell) = cost;
    }
  }
}


void MapImpl::incremental_set_cost(Coord cell, bool state, uint_least8_t cost) {
  assert(doors.find(cell) == doors.end());

  if (data.at(cell) == PATH_COST_INFINITE &&
      cost != PATH_COST_INFINITE) {
    // Removing a wall.
    incremental_wall_to_transparent(cell);
  } else if (data.at(cell) != PATH_COST_INFINITE &&
      cost == PATH_COST_INFINITE) {
    // Building a wall. Flood fill required.
    incremental_transparent_to_wall(cell);
  }
  data.at(cell) = cost;
  // else -- either PATH_COST_INFINITE -> PATH_COST_INFINITE (nop)
  // or passable value to passable value -- colors are not affected
  // (even though path cost might be).
}


void MapImpl::incremental_update_door(Coord cell, bool state, uint_least8_t cost) {
  auto door_iter = doors.find(cell);
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
      incremental_closed_door_to_open_door(cell, door_iter);
    } else {
      incremental_open_door_to_closed_door(cell, door_iter);
    }
  }
}


// Forces recomputation of all cached information.
void MapImpl::rebuild() {
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

  color = Grid<int_least32_t>(get_size(), COLOR_UNKNOWN);
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
  while (restart.row < get_size().row) {
    restart.col = 0;
    while (restart.col < get_size().col) {
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


void MapImpl::print_map(std::ostream& os, const Path& path_to_show) const {
  std::set<Coord> in_path(
      path_to_show.get_path().begin(),
      path_to_show.get_path().end()
    );
  size_t index = 0;
  for (uint16_t j = 0; j < get_size().row; ++j) {
    for (uint16_t i = 0; i < get_size().col; ++i) {
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


void MapImpl::print_colors(std::ostream& os) const {
  for (uint16_t j = 0; j < get_size().row; ++j) {
    for (uint16_t i = 0; i < get_size().col; ++i) {
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


void MapImpl::print_static_components(std::ostream& os) const {
  for (uint16_t j = 0; j < get_size().row; ++j) {
    for (uint16_t i = 0; i < get_size().col; ++i) {
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


void MapImpl::print_dynamic_components(std::ostream& os) const {
  for (uint16_t j = 0; j < get_size().row; ++j) {
    for (uint16_t i = 0; i < get_size().col; ++i) {
      if (color.at(j, i) == COLOR_IMPASSABLE) {
        os << ' ';
      } else {
        int_least32_t c = dynamic_component.lookup(
            static_component.at(color.at(j, i))
          );
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



}  // namespace pathfinder
}  // namespace suncatcher
