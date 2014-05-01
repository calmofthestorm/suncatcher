#ifndef COORD_d0dcb74311134c19b0239c6c7238cfad
#define COORD_d0dcb74311134c19b0239c6c7238cfad

#include <cstdint>
#include <iostream>

#include <boost/functional/hash.hpp>

#include "UnionFind.h"

namespace suncatcher {
namespace pathfinder {

// POD class representing a position on the pathfinding map abstraction.
class Coord {
  public:
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

namespace std {

template <>
struct hash<suncatcher::pathfinder::Coord> {
  size_t operator () (const suncatcher::pathfinder::Coord& key) const {
    size_t seed = 0;
    boost::hash_combine(seed, key.row);
    boost::hash_combine(seed, key.col);
    return seed;
  }
};

}



#endif  /* COORD_d0dcb74311134c19b0239c6c7238cfad */
