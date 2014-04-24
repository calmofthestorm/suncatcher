template <typename T>
Grid<T>::Grid() { }

template <typename T>
Grid<T>::Grid(uint16_t r, uint16_t c, const T& val) {
  backing.clear();
  backing.resize(r, std::vector<T>(c, val));
}

template <typename T>
Grid<T>::Grid(std::vector<std::vector<T>>&& backing_in)
  : backing(backing_in) { }

template <typename T>
const std::vector<std::vector<T>>& Grid<T>::get_backing() const {
  return backing;
}

template <typename T>
std::vector<std::vector<T>>& Grid<T>::get_backing() {
  return backing;
}

template <typename T>
T& Grid<T>::at(const suncatcher::pathfinder::Coord& cell) {
  return backing[cell.row][cell.col];
}

template <typename T>
const T& Grid<T>::at(const suncatcher::pathfinder::Coord& cell) const {
  return backing[cell.row][cell.col];
}

template <typename T>
const T& Grid<T>::at(uint16_t row, uint16_t col) const {
  return backing[row][col];
}

template <typename T>
T& Grid<T>::at(uint16_t row, uint16_t col) {
  return backing[row][col];
}

template <typename T>
std::vector<suncatcher::pathfinder::Coord> Grid<T>::get_adjacent(
    const suncatcher::pathfinder::Coord& cell
  ) const {

  static const std::vector<suncatcher::pathfinder::Coord> ADJ_DELTA{
    {0, (uint16_t)-1}, {0, 1}, {(uint16_t)-1, 0}, {1, 0},
      {(uint16_t)-1, (uint16_t)-1}, {1, 1},
      {(uint16_t)-1, 1}, {1, (uint16_t)-1},
  };

  std::vector<suncatcher::pathfinder::Coord> neighbors;

  for (const auto& delta : ADJ_DELTA) {
    auto n = delta + cell;
    if (n.row < backing.size() && n.col < backing[0].size()) {
      neighbors.push_back(n);
    }
  }

  return neighbors;
}
