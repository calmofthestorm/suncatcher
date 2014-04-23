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

void Door::add_adjacent_component(int c) {
  if (std::find(adjacent_components.begin(), adjacent_components.end(), c) == 
      adjacent_components.end()) {
    adjacent_components.push_back(c);
  }
}

Coord Door::get_pos() const {
  return pos;
}

void Map::print_components(std::ostream& os) const {
  for (size_t j = 0; j < size.row; ++j) {
    for (size_t i = 0; i < size.col; ++i) {
      if (data.at(j, i) == ' ') {
        int c = component.at(j, i);
        if (c == -2) {
          std::cout << ' ';
        } else if (c < 10) {
          std::cout << component.at(j, i);
        } else {
          std::cout << (char)(c + 'A' - 10);
        }
      } else {
        std::cout << data.at(j, i);
      }
    }
    std::cout << std::endl;
  }
}

std::vector<Coord> Map::path(Coord src, Coord dst) {
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

  std::cout << "Map loaded with " << index << " components!" << std::endl;
  print_components(std::cout);
}

int main(int argc, char** argv) {
  std::ifstream is("map.txt");
  Map my_map(is);
  return 0;
}
