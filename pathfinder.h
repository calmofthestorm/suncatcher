#include <iosfwd>
#include <vector>

class Door {
  public:
    Door(size_t row, size_t col);

    bool get_open() const;
    void set_open(bool state);
    void toggle_open();
    std::pair<size_t, size_t> get_pos() const;
    void add_adjacent_component(int c);

  private:
    std::pair<size_t, size_t> pos;
    std::vector<int> adjacent_components;
    bool open;
};

class Map {
  public:
    Map(std::istream& infile);
    void print_components(std::ostream& os) const;

  private:
    std::vector<std::pair<size_t, size_t>> get_adjacent(
        const std::pair<size_t,
        size_t>& cell
      ) const;

    char& at(const std::pair<size_t, size_t>& cell);
    const char& at(const std::pair<size_t, size_t>& cell) const;

    const char& at(size_t row, size_t col) const;
    char& at(size_t row, size_t col);

    std::vector<std::vector<char>> data;
    std::vector<std::vector<int>> component;
    std::vector<Door> doors;
    std::pair<size_t, size_t> size;
};
