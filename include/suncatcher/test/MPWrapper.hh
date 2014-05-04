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

#ifndef MPWRAPPER_616e1e92191d445dac0fe08e40c6429b
#define MPWRAPPER_616e1e92191d445dac0fe08e40c6429b

#include "suncatcher/Path.hh"
#include "suncatcher/Coord.hh"
#include "suncatcher/micropather/MicropatherGraph.hh"

namespace micropather {
  class MicroPather;
}  // namespace  Micropather

namespace suncatcher {

namespace pathfinder {
  class Map;
}  // namespace pathfinder

namespace test {

class DeltaMap;

class MPWrapper {
  public:
    MPWrapper(const DeltaMap* map);

    void new_pather();

    pathfinder::Path path(pathfinder::Coord start, pathfinder::Coord finish);

  private:
    const pathfinder::Map* my_map;
    std::unique_ptr<pathfinder::MicropatherGraph> mpg;
    std::unique_ptr<micropather::MicroPather> mp;
};

}  // namespace test
}  // namespace suncatcher

#endif  /* MPWRAPPER_616e1e92191d445dac0fe08e40c6429b */
