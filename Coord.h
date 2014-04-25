#ifndef COORD_d0dcb74311134c19b0239c6c7238cfad
#define COORD_d0dcb74311134c19b0239c6c7238cfad

// POD class representing a position on the pathfinding map abstraction.
// 
#include <cstdint>
#include <iostream>

namespace suncatcher {
namespace pathfinder {

struct Coord {
  uint16_t row, col;

  inline bool operator!= (const Coord& other) const {
    return row != other.row || col != other.col;
  }

  inline bool operator== (const Coord& other) const {
    return row == other.row && col == other.col;
  }

  inline bool operator< (const Coord& other) const {
    return (row < other.row || (row == other.row && col < other.col));
  }

  inline Coord operator+ (const Coord& other) const {
    Coord r(*this);
    r.row += other.row;
    r.col += other.col;
    return r;
  }
};

inline std::ostream& operator<< (std::ostream& os, const pathfinder::Coord& c) {
  os << '(' << c.row << ", " << c.col << ')';
  return os;
}

}  // namespace pathfinder
}  // namespace suncatcher

#endif  /* COORD_d0dcb74311134c19b0239c6c7238cfad */
