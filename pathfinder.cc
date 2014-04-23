#include "pathfinder.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <tuple>

Door::Door(size_t row, size_t col)
: pos({row, col}),
  open(true) { }

const std::vector<int>& Door::get_adjacent_components() const {
  return adjacent_components;
}

void Door::add_adjacent_component(int c) {
  if (std::find(adjacent_components.begin(), adjacent_components.end(), c) == 
      adjacent_components.end()) {
    adjacent_components.push_back(c);
  }
}

Coord Door::get_pos() const {
  return pos;
}

void Door::set_open(bool state) {
  open = state;
}

bool Door::get_open() const {
  return open;
}

size_t Map::find_door(const Coord& c) const {
  auto it = std::find_if(
      doors.begin(),
      doors.end(),
      [c] (const Door& d) { return d.get_pos() == c; }
    );
  return it == doors.end() ? -1 : it - doors.begin();
}

void Map::print_map(std::ostream& os) const {
  for (size_t j = 0; j < size.row; ++j) {
    for (size_t i = 0; i < size.col; ++i) {
      char cell = data.at(j, i);
      if (cell == 'd' || cell == '_') {
        auto d = doors.begin() + find_door({j, i});
        if (d->get_open()) {
          std::cout << (d - doors.begin());
        } else {
          std::cout << "\033[1m" << (d - doors.begin()) << "\033[0m";
        }
      } else {
        std::cout << cell;
      }
    }
    std::cout << std::endl;
  }
}

namespace {

int find_representative(const std::vector<int>& parents, int elem) {
  while (elem != parents[elem]) {
    elem = parents[elem];
  }
  return elem;
}

}  // anonymous namespace

// TODO: evaluate if an incremental data struture (eg union find) does
// better than eager transitive closure in practice.
// TODO: evaluate if we can do this more efficiently.
void Map::update_equivalence(const Coord& pos, bool new_state) {

  // if (new_state) {
  //   auto bridged = doors[find_door(pos)].get_adjacent_components();
  //   // Slight savings available with partially-incremental update.
  // }
  for (size_t i = 0; i < equivalent_components.size(); ++i) {
    equivalent_components[i] = i;
  }

  for (const auto& d : doors) {
    if (d.get_open()) {
      auto bridged = d.get_adjacent_components();
      int class1 = find_representative(equivalent_components, bridged.front());
      for (auto it = bridged.begin() + 1; it < bridged.end(); ++it) {
        int class2 = find_representative(equivalent_components, *it);
        equivalent_components[class2] = class1;
      }
    }
  }

  // Transitive closure
  //TODO: faster
  for (size_t i = 0; i < equivalent_components.size(); ++i) {
    equivalent_components[i] = find_representative(equivalent_components, i);
  }
}

void Map::print_components(std::ostream& os) const {
  for (size_t j = 0; j < size.row; ++j) {
    for (size_t i = 0; i < size.col; ++i) {
      if (data.at(j, i) == ' ') {
        int c = component.at(j, i);
        if (c == -2) {
          std::cout << ' ';
        } else {
          std::cout << (char)(c + 'A');
        }
      } else {
        std::cout << data.at(j, i);
      }
    }
    std::cout << std::endl;
  }
}

void Map::print_equivalence_classes(std::ostream& os) const {
  for (size_t j = 0; j < size.row; ++j) {
    for (size_t i = 0; i < size.col; ++i) {
      if (data.at(j, i) == ' ') {
        int c = equivalent_components.at(component.at(j, i));
        if (c == -2) {
          std::cout << ' ';
        } else if (c >= 0 && c < 25) {
          std::cout << (char)(c + 'A');
        } else {
          std::cout << ' ' << c << ' ';
        }
      } else {
        std::cout << data.at(j, i);
      }
    }
    std::cout << std::endl;
  }
}

std::vector<Coord> Map::path(Coord src, Coord dst) {
  std::vector<Coord> rval;

  if (src == dst || 
      (equivalent_components[component.at(src)] !=
       equivalent_components[component.at(dst)])) {
    return rval;
  }
}

Map::Map(std::istream& is) {
  size.row = 0;
  size.col = 0;
  std::string line;

  // Read in map.
  while (getline(is, line)) {
    size.col = std::max(line.size(), size.col);
    ++size.row;
    data.get_backing().push_back(std::vector<char>(line.begin(), line.end()));
  }

  // Make it a square if necessary.
  for (auto& line : data.get_backing()) {
    line.resize(size.col, '*');
  }

  // Create doors, validate map, and set up structures for flood fill.
  component.get_backing().resize(size.row, std::vector<int>(size.col, -2));
  for (size_t j = 0;j < size.row; ++j) {
    for (size_t i = 0; i < size.col; ++i) {
      switch (data.at(j, i)) {
        case 'd':
          doors.push_back(Door(j, i));
          data.at(j, i) = '_';
          break;

        case ' ':
          break;

        case '*':
          component.at(j, i) = -1;
          break;

        default:
          assert(0);
      }
    }
  }

  // Identify connected components, stopping at walls and doors.
  size_t restart_row = 0;
  int index = 0;
  while (restart_row < size.row) {
    size_t restart_col = 0;
    while (restart_col < size.col) {
      if (component.at(restart_row, restart_col) == -2 && data.at(restart_row, restart_col) == ' ') {
        // Flood fill component.
        std::stack<Coord> todo;
        todo.push({restart_row, restart_col});
        component.at(restart_row, restart_col) = index++;

        while (!todo.empty()) {
          auto cur = todo.top();
          todo.pop();
          // Explore neighbors.
          for (const auto& n : data.get_adjacent(cur)) {
            if (data.at(n) == ' ' && component.at(n.row, n.col) == -2) {
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
  for (Door& door : doors) {
    for (const auto& n : data.get_adjacent(door.get_pos())) {
      if (component.at(n.row, n.col) >= 0) {
        door.add_adjacent_component(component.at(n.row, n.col));
      }
    }
  }

  // Set up equivalence classes that dynamically update in response to doors.
  // Think union find light. All doors start open.
  equivalent_components.resize(index, 0);

  std::cout << "Map loaded with " << index << " components!" << std::endl;
  while (1) {
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    print_components(std::cout);
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    print_equivalence_classes(std::cout);
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    print_map(std::cout);

    char bees;
    std::cin >> bees;
    if (bees >= '0' && bees <= '9') {
      Door& d = doors[bees - '0'];
      bool state = !d.get_open();
      d.set_open(state);
      data.at(d.get_pos()) = state ? '_' : 'd';

      update_equivalence(d.get_pos(), state);
    }
  }
}

int main(int argc, char** argv) {
  std::ifstream is("map.txt");
  Map my_map(is);
  return 0;
}
