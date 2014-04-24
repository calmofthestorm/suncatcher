#include "pathfinder.h"
#include "micropather.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <tuple>

#include <boost/date_time/posix_time/posix_time.hpp>

int timer() {
  static auto begin = std::chrono::high_resolution_clock::now();
  auto tmp = std::chrono::high_resolution_clock::now();
  int rval = std::chrono::duration_cast<std::chrono::microseconds>(tmp - begin).count();
  begin = tmp;
  return rval;
}

using namespace micropather;

namespace {

int find_representative(const std::vector<int>& parents, int elem) {
  while (elem != parents[elem]) {
    elem = parents[elem];
  }
  return elem;
}

double manhattan(const Coord& a, const Coord& b) {
  return std::abs((double)a.row - (double)b.row) + std::abs((double)a.col - (double)b.col);
  // return pow(pow(std::abs((double)a.row - (double)b.row), 2) + pow(std::abs((double)a.col - (double)b.col), 2), 0.5);
}

}  // anonymous namespace


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
  //TODO: faster; path compression
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
  Grid<int> expanded(size.row, size.col, 0);
  Grid<Coord> previous(size.row, size.col, {(size_t)-1, (size_t)-1});
  Grid<double> fuzz(size.row, size.col, 0);
  int fu = 1;
  for (auto& row : fuzz.get_backing()) {
    for (auto& col : row) {
      col += fu++ * 0.00001;
    }
  }
  size_t num_expanded = 0;

  // A path exists iff they are in the same equivalence class.
  if (equivalent_components[component.at(src)] !=
      equivalent_components[component.at(dst)]) {
    return std::vector<Coord>();
  }

  if (src == dst) {
    return std::vector<Coord>{src};
  }

  Grid<double> distance(size.row, size.col, INFINITY);
  distance.at(src) = 0;

  struct Entry {
    Coord pos;
    double cost;

    // Make pqueue work in proper order.
    bool operator< (const Entry& e) const {
      return cost > e.cost;
    }
  };
  std::priority_queue<Entry> fringe;
  fringe.push({src, 0});
  fringe.push({src, 5});
  assert(fringe.top().cost == 0);
  fringe.pop();
  fringe.pop();
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
      // TODO: assumes all traversal costs are 1.
      std::vector<Coord> rval(distance.at(dst) + 1);
      auto cur = dst;
      return rval;
    }

    for (const auto& next : data.get_adjacent(cur.pos)) {
      // if (distance.at(next) > 1 + distance.at(cur.pos) && (
      double my_dist = distance.at(cur.pos) + 1;
      if (cur.pos.row != next.row && cur.pos.col != next.col) {
        my_dist += 0.414;
      }
      if (distance.at(next) > 1 + distance.at(cur.pos) &&
          !expanded.at(next) && (
            data.at(next) == ' ' ||
            data.at(next) == '_')) {
        distance.at(next) = 1 + distance.at(cur.pos);
        fringe.push({next, manhattan(next, dst) + distance.at(next) * 0.999});
        previous.at(next) = cur.pos;
      }
    }
  }
  assert(0);
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
}

class MPMap : public Graph, Map {
  public:
    MPMap(std::istream& is)
    : Map(is) { }

    virtual float LeastCostEstimate( void* stateStart, void* stateEnd ) const {
      return manhattan(decode(stateStart), decode(stateEnd));
    }

    virtual void AdjacentCost( void* state, MP_VECTOR< micropather::StateCost > *adjacent ) const {
      auto cur = decode(state);
      for (const auto& n : data.get_adjacent(cur)) {
        float cost = (n.row == cur.row || n.col == cur.col) ? 1 : 1.414;
        if (data.at(n) != '*') {
          adjacent->push_back({encode(n), cost});
        }
      }
    }

    virtual void PrintStateInfo( void* state ) const { }

    void* encode(const Coord& c) const {
      uintptr_t encoded = (c.row * get_size().row + c.col);
      return (void*)encoded;
    }

    Coord decode(void* c) const {
      uintptr_t encoded = (uintptr_t)c;
      size_t width = get_size().row;
      return {encoded / width, encoded % width};
    }
};

int main(int argc, char** argv) {
  assert(argc == 4);
  std::ifstream is(argv[1]);
  MPMap my_map(is);
  Map* ma = (Map*)(&my_map);

  std::unique_ptr<MicroPather> mp(new MicroPather(&my_map, 4000000, 8, false));
  while (1) {
    MP_VECTOR<void*> pa;
    float co = 23;
    std::cout << "starting" << std::endl;
    Coord start{1, 1};
    Coord finish{atoi(argv[2]), atoi(argv[3])};

    ma->path(start, finish);
    mp->Solve((void*)(uintptr_t)(start.row * ma->get_size().row + start.col), (void*)(uintptr_t)(finish.row * ma->get_size().row + finish.col), &pa, &co);
    mp->Reset();

    timer();
    ma->path(start, finish);
    std::cout << timer() << std::endl;
    mp->Solve((void*)(uintptr_t)(start.row * ma->get_size().row + start.col), (void*)(uintptr_t)(finish.row * ma->get_size().row + finish.col), &pa, &co);
    std::cout << timer() << std::endl;

    // for (const auto& it : pa) {
    //   cout << '(' << ((it / size.row)) << ", " << ((it % size.row)) << ')' << std::endl;
    // }

    return 0;
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
    // print_components(std::cout);
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    // print_equivalence_classes(std::cout);
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    // print_map(std::cout);

    // char bees;
    // std::cin >> bees;
    // if (bees >= '0' && bees <= '9') {
    //   Door& d = doors[bees - '0'];
    //   bool state = !d.get_open();
    //   d.set_open(state);
    //   data.at(d.get_pos()) = state ? '_' : 'd';
    //
    //   update_equivalence(d.get_pos(), state);
    // }
  }


  return 0;
}
