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

#include "suncatcher/platform.hh"

#ifdef MICROPATHER_DELTA_TEST

#include "suncatcher/test/MPWrapper.hh"

#include "suncatcher/test/DeltaMap.hh"
#include "suncatcher/Path.hh"
#include "suncatcher/micropather/MicropatherGraph.hh"
#include "suncatcher/Coord.hh"

namespace suncatcher {
namespace test {

using suncatcher::test::DeltaMap;
using suncatcher::test::MicropatherGraph;
using suncatcher::pathfinder::Map;
using suncatcher::pathfinder::Path;
using suncatcher::pathfinder::Coord;

MPWrapper::MPWrapper(pathfinder::MapView view_i)
: view(view_i) {
  mpg.reset(new MicropatherGraph(view_i));
}

void MPWrapper::new_pather() {
  mp.reset(new micropather::MicroPather(mpg.get(), 4000000, 8, false));
}

Path MPWrapper::path(Coord start, Coord finish) {
  if (!view.check_bounds(start) ||
      !view.check_bounds(finish) ||
      !view.is_passable(start) || !view.is_passable(finish)) {
    return Path({}, -1);
  }
  if (start == finish) {
    return Path{{start}, 0};
  }
  // Paradoxically, keeping the pather around slows it down AND caches even
  // when manually cleared and constructed with disabled cache...
  new_pather();
  MP_VECTOR<void*> path_vc;
  float length = 279;
  mp->Solve(mpg->encode(start), mpg->encode(finish), &path_vc, &length);
  std::vector<Coord> real_path(path_vc.size());
  for (size_t i = 0; i < path_vc.size(); ++i) {
    real_path[i] = mpg->decode(path_vc[i]);
  }
  if (real_path.size() > 0) {
    return Path(std::move(real_path), length);
  } else {
    return Path{{}, -1};
  }
}

}  // namespace test
}  // namespace suncatcher

#endif
