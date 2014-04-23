#include <iostream>
#include <vector>

struct Coord {
  size_t row, col;

  bool operator== (const Coord& other) const {
    return row == other.row && col == other.col;
  }

  Coord operator+ (const Coord& other) const {
    Coord r(*this);
    r.row += other.row;
    r.col += other.col;
    return r;
  }
};

std::ostream& operator<< (std::ostream& os, const Coord& c) {
  os << '(' << c.row << ", " << c.col << ')';
  return os;
}

class Door {
  public:
    Door(size_t row, size_t col);

    bool get_open() const;
    void set_open(bool state);
    void toggle_open();
    Coord get_pos() const;
    void add_adjacent_component(int c);

  private:
    Coord pos;
    std::vector<int> adjacent_components;
    bool open;
};

template <typename T>
class Grid {
  public:
    Grid() { }

    Grid(std::vector<std::vector<T>>&& backing_in)
    : backing(backing_in) { }

    const std::vector<std::vector<T>>& get_backing() const {
      return backing;
    }

    std::vector<std::vector<T>>& get_backing() {
      return backing;
    }

    T& at(const Coord& cell) {
      return backing[cell.row][cell.col];
    }

    const T& at(const Coord& cell) const {
      return backing[cell.row][cell.col];
    }

    const T& at(size_t row, size_t col) const {
      return backing[row][col];
    }

    T& at(size_t row, size_t col) {
      return backing[row][col];
    }

    std::vector<Coord> get_adjacent(const Coord& cell) const {

      static const std::vector<Coord> ADJ_DELTA{
        {0, (size_t)-1}, {0, 1}, {(size_t)-1, 0}, {1, 0}
      };

      std::vector<Coord> neighbors;

      for (const auto& delta : ADJ_DELTA) {
        auto n = delta + cell;
        if (n.row < backing.size() && n.col < backing[0].size()) {
          neighbors.push_back(n);
        }
      }

      return neighbors;
    }

  private:
    std::vector<std::vector<T>> backing;
};

class Map {
  public:
    Map(std::istream& infile);
    void print_components(std::ostream& os) const;
    std::vector<Coord> path(Coord src, Coord dst);

  private:

    Grid<char> data;
    Grid<int> component;
    std::vector<Door> doors;
    Coord size;
};
