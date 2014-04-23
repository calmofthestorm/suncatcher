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

std::pair<size_t, size_t> Door::get_pos() const {
  return pos;
}

const char& Map::at(size_t row, size_t col) const {
  return data[row][col];
}

char& Map::at(size_t row, size_t col) {
  return data[row][col];
}

const char& Map::at(const std::pair<size_t, size_t>& cell) const {
  return data[cell.first][cell.second];
}

char& Map::at(const std::pair<size_t, size_t>& cell) {
  return data[cell.first][cell.second];
}

void Map::print_components(std::ostream& os) const {
  for (size_t j = 0; j < size.first; ++j) {
    for (size_t i = 0; i < size.second; ++i) {
      if (at(j, i) == ' ') {
        int c = component[j][i];
        if (c == -2) {
          std::cout << ' ';
        } else if (c < 10) {
          std::cout << component[j][i];
        } else {
          std::cout << (char)(c + 'A' - 10);
        }
      } else {
        std::cout << at(j, i);
      }
    }
    std::cout << std::endl;
  }
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
}

std::vector<std::pair<size_t, size_t>> Map::get_adjacent(
    const std::pair<size_t,
    size_t>& cell
  ) const {
  static const std::vector<std::pair<size_t, size_t>> ADJ_DELTA{
      {0, -1}, {0, 1}, {-1, 0}, {1, 0}
    };

  std::vector<std::pair<size_t, size_t>> neighbors;

  for (const auto& delta : ADJ_DELTA) {
    size_t n_row = delta.first + cell.first;
    size_t n_col = delta.second + cell.second;
    if (n_row < size.first && n_col < size.second) {
      neighbors.push_back({n_row, n_col});
    }
  }

  return neighbors;
}

Map::Map(std::istream& is) {

  size.first = 0;
  size.second = 0;
  std::string line;

  // Read in map.
  while (getline(is, line)) {
    size.second = std::max(line.size(), size.second);
    ++size.first;
    data.push_back(std::vector<char>(line.begin(), line.end()));
  }

  // Make it a square if necessary.
  for (auto& line : data) {
    line.resize(size.first, '*');
  }

  // Create doors, validate map, and set up structures for flood fill.
  component.resize(size.first, std::vector<int>(size.second, -2));
  for (size_t j = 0;j < size.first; ++j) {
    for (size_t i = 0; i < size.second; ++i) {
      switch (at(j, i)) {
        case 'd':
          doors.push_back(Door(j, i));
          at(j, i) = '_';
          break;

        case ' ':
          break;

        case '*':
          component[j][i] = -1;
          break;

        default:
          assert(0);
      }
    }
  }

  // Identify connected components, stopping at walls and doors.
  size_t restart_row = 0;
  int index = 0;
  while (restart_row < size.first) {
    size_t restart_col = 0;
    while (restart_col < size.second) {
      if (restart_row == 69 && restart_col == 64) {
        std::cout << "here" << std::endl;
      }
      if (component[restart_row][restart_col] == -2 && at(restart_row, restart_col) == ' ') {
        // Flood fill component.
        std::stack<std::pair<size_t, size_t>> todo;
        todo.push({restart_row, restart_col});
        component[restart_row][restart_col] = index++;

        while (!todo.empty()) {
          auto cur = todo.top();
          todo.pop();
          // Explore neighbors.
          for (const auto& n : get_adjacent(cur)) {
            if (at(n) == ' ' && component[n.first][n.second] == -2) {
              todo.push(n);
              component[n.first][n.second] = component[cur.first][cur.second];
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
    for (const auto& n : get_adjacent(door.get_pos())) {
      if (component[n.first][n.second] >= 0) {
        door.add_adjacent_component(component[n.first][n.second]);
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
