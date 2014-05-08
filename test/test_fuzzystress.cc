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

#include <cassert>

#include "suncatcher/test/common.hh"

#include "suncatcher/Coord.hh"
#include "suncatcher/test/DeltaMap.hh"
#include "suncatcher/MapMutator.hh"
#include "suncatcher/util/util.hh"
#include "suncatcher/test/ResourceManager.hh"
#include "suncatcher/CoordRange.hh"


using namespace suncatcher::test;
using namespace suncatcher::pathfinder;

class SLOW2DFuzzDoorland : public MapTest<MAP_DOORLAND> {
  public:
    std::vector<Coord> coords, doors, nondoors;
    void SetUp() {
      srand(4); // Chosen by fair dice roll. Guaranteed to be random.
      ASSERT_TRUE(map.enable_delta) << "These tests requires export SLOWTEST=1, and tests nothing meaningful without it. As a safety measure we will fail now. You can run the test binary with --gtest_filter=-*SLOW* to run only tests that do not require it.";
      for (const auto& it : CoordRange(map.size())) {
        coords.push_back(it);
        if (map.is_door(it)) {
          doors.push_back(it);
        } else {
          nondoors.push_back(it);
        }
      }

      auto m = map.get_mutator();
      for (const Coord& c : doors) {
        m.set_door_open(c, rand() % 2);
      }
      map.mutate(m);
    }

    void check_random_paths(const std::vector<Coord>& cs) {
      for (size_t k = 0; k < 10; ++k) {
        map.path(cs[rand() % cs.size()], cs[rand() % cs.size()]);
      }
    }
};


TEST_F(SLOW2DFuzzDoorland, OpenCloseDoors) {
  for (size_t i = 0; i < 5000; ++i) {
    for (size_t j = 0; j < 2; ++j) {
      map.mutate(map.get_mutator().toggle_door_open(doors[rand() % doors.size()]));
    }

    for (size_t k = 0; k < 10; ++k) {
      Coord src = coords.at( rand() % coords.size() );
      Coord dest = coords.at( rand() % coords.size() );
      map.path(src, dest);
    }

    check_random_paths(coords);

    ASSERT_TRUE(map.path_exists({8, 12, 0}, {0, 0, 0}));
  }
}


TEST_F(SLOW2DFuzzDoorland, CreateRemoveWalls) {
  for (size_t i = 0; i < 10000; ++i) {
    for (size_t j = 0; j < 2; ++j) {
      auto c = nondoors[rand() % nondoors.size()];
      auto cost = map.get_data().at(c) == PATH_COST_INFINITE ? rand() % 200 : PATH_COST_INFINITE;
      map.mutate(map.get_mutator().set_cost(c, cost));
    }
    check_random_paths(coords);
  }
}
