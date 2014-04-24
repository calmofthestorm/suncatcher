#include <iostream>
#include <vector>

struct Coord {
  uint16_t row, col;

  bool operator!= (const Coord& other) const {
    return row != other.row || col != other.col;
  }

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
    Door(uint16_t row, uint16_t col);

    bool get_open() const;
    void set_open(bool state);
    void toggle_open();
    Coord get_pos() const;
    void add_adjacent_component(int c);
    const std::vector<int>& get_adjacent_components() const;

  private:
    Coord pos;
    std::vector<int> adjacent_components;
    bool open;
};

template <typename T>
class Grid {
  public:
    Grid() { }

    Grid(uint16_t r, uint16_t c, const T& val) {
      backing.clear();
      backing.resize(r, std::vector<T>(c, val));
    }

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

    const T& at(uint16_t row, uint16_t col) const {
      return backing[row][col];
    }

    T& at(uint16_t row, uint16_t col) {
      return backing[row][col];
    }

    std::vector<Coord> get_adjacent(const Coord& cell) const {

      static const std::vector<Coord> ADJ_DELTA{
        {0, (uint16_t)-1}, {0, 1}, {(uint16_t)-1, 0}, {1, 0},
        {(uint16_t)-1, (uint16_t)-1}, {1, 1},
        {(uint16_t)-1, 1}, {1, (uint16_t)-1},
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
    void print_equivalence_classes(std::ostream& os) const;
    void print_map(std::ostream& os) const;
    std::vector<Coord> path(Coord src, Coord dst);

    void update_equivalence(const Coord& pos, bool new_state);

    const Coord& get_size() const { return size; }

  protected:
    size_t find_door(const Coord& c) const;

    Grid<char> data;
    Grid<int> component;
    std::vector<int> equivalent_components;
    std::vector<Door> doors;
    Coord size;
};
