// This file is part of Suncatcher
// Alex Roper <alex@aroper.net>
//
// Suncatcher is free software: you can redistribute it and/or modify it under
// the terms of version 3 of the GNU General Public License as published by the
// Free Software Foundation.
//
// Suncatcher is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// Suncatcher.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2014 Alex Roper

#ifndef PATH_a67c1237fbfa4e72b1668f3194e3ae50
#define PATH_a67c1237fbfa4e72b1668f3194e3ae50

#include <cstdlib>
#include <iosfwd>
#include <vector>

#include "suncatcher/Coord.hh"

const float EPSILON = (float)0.0001;

namespace suncatcher {
namespace pathfinder {

class Path {
  public:
    inline Path()
    : path(),
      length(-1) { }

    inline Path(std::vector<Coord>&& path_i, float length_i)
    : path(path_i),
      length(length_i) { }

    inline const std::vector<Coord>& get_path() const { return path; }

    inline float get_length() const { return length; }

    inline bool operator== (const Path& other) const {
      return other.path == path && std::abs(other.length - length) < EPSILON;
    }

    explicit inline operator bool() {
      return length != -1;
    }

  private:
    friend inline std::ostream& operator<< (std::ostream& os, const Path& path);
    std::vector<Coord> path;
    float length;
};

inline std::ostream& operator<< (std::ostream& os, const Path& path) {
  os << '{';
  for (const auto& it : path.path) {
    os << it << ", ";
  }
  os << "}:" << path.length;
  return os;
}

}  // namespace pathfinder
}  // namespace suncatcher

#endif  /* PATH_a67c1237fbfa4e72b1668f3194e3ae50 */
