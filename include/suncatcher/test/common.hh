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

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <stack>
#include <string>
#include <tuple>

#include <gtest/gtest.h>

#include "suncatcher/platform.hh"

#include "suncatcher/micropather/micropather.hh"
#include "suncatcher/micropather/MicropatherGraph.hh"

#include "suncatcher/Coord.hh"
#include "suncatcher/test/DeltaMap.hh"
#include "suncatcher.hh"
#include "suncatcher/MapMutator.hh"
#include "suncatcher/util/util.hh"
#include "suncatcher/test/ResourceManager.hh"
#include "suncatcher/test/MPWrapper.hh"

namespace suncatcher {
namespace test {

extern const char MAP_OPEN[];
extern const char MAP_SMALL_OPEN[];
extern const char MAP_CROSS[];
extern const char MAP_MAIN[];
extern const char MAP_DOORLAND[];
extern const char MAP_MICRO[];
extern const char MAP_CUBE3[];
extern const char MAP_CUBE5[];
extern const char MAP_OCTANTS[];
extern const char MAP_CHECKERDOOR[];
extern const char MAP_CHECKERDOOR64[];
extern const char MAP_HARDNINE[];
extern const char MAP_HARDERNINE[];

using suncatcher::util::manhattan;
using suncatcher::pathfinder::Map;
using suncatcher::test::DeltaMap;
using suncatcher::pathfinder::MapMutator;
using suncatcher::pathfinder::MapBuilder;
using suncatcher::pathfinder::Path;
using suncatcher::pathfinder::Coord;
using suncatcher::PATH_COST_INFINITE;

#ifdef MICROPATHER_DELTA_TEST
using suncatcher::test::MicropatherGraph;
#endif

template <const char* MAP>
class MapTest : public ::testing::Test {
  public:
    MapTest()
    : map(ResourceManager::get_map(MAP)) {
      char* slow = std::getenv("SLOWTEST");
      enable_micropather = (!slow || !strcmp("1", slow));
    }

    #ifdef MICROPATHER_DELTA_TEST
      MPWrapper get_micropather() {
        MPWrapper ok(map.get_simple());
        return std::move(ok);
      }
    #endif

  protected:
    bool enable_micropather;
    suncatcher::test::DeltaMap map;
};

}  // namespace test
}  // namespace suncatcher
