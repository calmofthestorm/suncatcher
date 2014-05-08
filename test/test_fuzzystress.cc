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

void scrambler(
    std::vector<Coord>& coords,
    std::vector<Coord>& doors,
    std::vector<Coord>& nondoors,
    DeltaMap& map
  );


void check_random_paths(std::vector<Coord>& cs, DeltaMap& map);


void choose_one_legal_mutation(DeltaMap& map, Coord c);
void choose_one_legal_mutation_anywhere(DeltaMap& map);


void scrambler(
    std::vector<Coord>& coords,
    std::vector<Coord>& doors,
    std::vector<Coord>& nondoors,
    DeltaMap& map
  ) {

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


void check_random_paths(std::vector<Coord>& cs, DeltaMap& map) {
  for (size_t k = 0; k < 10; ++k) {
    map.path(cs[rand() % cs.size()], cs[rand() % cs.size()]);
  }
}


void choose_one_legal_mutation_anywhere(DeltaMap& map) {
  Coord cell(
      rand() % map.size().row,
      rand() % map.size().col,
      rand() % map.size().layer
    );
  choose_one_legal_mutation(map, cell);
}


void choose_one_legal_mutation(DeltaMap& map, Coord cell) {
  std::vector<decltype(map.get_mutator())> legal;
  uint8_t cost = rand() % 250;
  if (map.is_door(cell)) {
    legal.push_back(std::move(map.get_mutator().toggle_door_open(cell)));
    legal.push_back(std::move(map.get_mutator().set_door_open(cell, cost)));
    legal.push_back(std::move(map.get_mutator().remove_door(cell, cost)));
    legal.push_back(std::move(map.get_mutator().remove_door(cell, PATH_COST_INFINITE)));
  } else {
    legal.push_back(std::move(map.get_mutator().create_door(cell, true, cost)));
    legal.push_back(std::move(map.get_mutator().create_door(cell, false, cost)));
    if (map.get_cost(cell) == PATH_COST_INFINITE) {
      legal.push_back(std::move(map.get_mutator().set_cost(cell, cost)));
    } else {
      legal.push_back(std::move(map.get_mutator().set_cost(cell, PATH_COST_INFINITE)));
    }
  }

  map.mutate(legal.at(rand() % legal.size()));
}


class SLOW2DFuzzDoorland : public MapTest<MAP_DOORLAND> {
  public:
    std::vector<Coord> coords, doors, nondoors;
    void SetUp() {
      scrambler(coords, doors, nondoors, map);
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

    check_random_paths(coords, map);

    ASSERT_TRUE(map.path_exists({8, 12, 0}, {0, 0, 0}));
  }
}


TEST_F(SLOW2DFuzzDoorland, CreateRemoveWalls) {
  for (size_t i = 0; i < 10000; ++i) {
    for (size_t j = 0; j < 2; ++j) {
      auto c = nondoors[rand() % nondoors.size()];
      auto cost = map.get_cost(c) == PATH_COST_INFINITE ? rand() % 200 : PATH_COST_INFINITE;
      map.mutate(map.get_mutator().set_cost(c, cost));
    }
    check_random_paths(coords, map);
  }
}


TEST_F(SLOW2DFuzzDoorland, AllMutationsAnywhere) {
  for (size_t i = 0; i < 10000; ++i) {
    for (size_t j = 0; j < 2; ++j) {
      choose_one_legal_mutation_anywhere(map);
    }
    check_random_paths(coords, map);
  }
}


class SLOW2DFuzzCheckerdoor64 : public MapTest<MAP_CHECKERDOOR64> {
  public:
    std::vector<Coord> coords, doors, nondoors;
    void SetUp() {
      scrambler(coords, doors, nondoors, map);
    }
};


TEST_F(SLOW2DFuzzCheckerdoor64, OpenCloseDoors) {
  for (size_t i = 0; i < 5000; ++i) {
    for (size_t j = 0; j < 2; ++j) {
      map.mutate(map.get_mutator().toggle_door_open(doors[rand() % doors.size()]));
    }

    for (size_t k = 0; k < 10; ++k) {
      Coord src = coords.at( rand() % coords.size() );
      Coord dest = coords.at( rand() % coords.size() );
      map.path(src, dest);
    }

    check_random_paths(coords, map);
  }
}


TEST_F(SLOW2DFuzzCheckerdoor64, CreateRemoveWalls) {
  for (size_t i = 0; i < 10000; ++i) {
    for (size_t j = 0; j < 2; ++j) {
      auto c = nondoors[rand() % nondoors.size()];
      auto cost = map.get_cost(c) == PATH_COST_INFINITE ? rand() % 200 : PATH_COST_INFINITE;
      map.mutate(map.get_mutator().set_cost(c, cost));
    }
    check_random_paths(coords, map);
  }
}


TEST_F(SLOW2DFuzzCheckerdoor64, AllMutationsAnywhere) {
  for (size_t i = 0; i < 10000; ++i) {
    for (size_t j = 0; j < 2; ++j) {
      choose_one_legal_mutation_anywhere(map);
    }
    check_random_paths(coords, map);
  }
}


class SLOW2DFuzzHardNine: public MapTest<MAP_HARDNINE> {
  public:
    std::vector<Coord> coords, doors, nondoors;
    void SetUp() {
      scrambler(coords, doors, nondoors, map);
    }
};


class SLOW2DFuzzHarderNine: public MapTest<MAP_HARDERNINE> {
  public:
    std::vector<Coord> coords, doors, nondoors;
    void SetUp() {
      scrambler(coords, doors, nondoors, map);
    }
};


TEST_F(SLOW2DFuzzHardNine, AnythingInteresting) {
  std::vector<Coord> interesting, changing;
  for (uint16_t i = 1; i < 8; i += 2) {
    for (uint16_t j = 1; j < 8; j += 2) {
      interesting.push_back({i, j, 0});
    }
  }
  for (uint16_t i = 1; i < 8; i += 2) {
    for (uint16_t j = 2; j < 7; j += 2) {
      changing.push_back({i, j, 0});
      changing.push_back({j, i, 0});
    }
  }
  interesting.insert(interesting.end(), changing.begin(), changing.end());

  for (size_t i = 0; i < 10000; ++i) {
    for (size_t j = 0; j < 2; ++j) {
      choose_one_legal_mutation(map, changing.at(rand() % changing.size()));
    }
    check_random_paths(interesting, map);
  }
}


TEST_F(SLOW2DFuzzHarderNine, AnythingInteresting) {
  std::vector<Coord> interesting, changing;
  for (uint16_t i = 1; i < 6; i += 2) {
    for (uint16_t j = 1; j < 6; j += 2) {
      for (uint16_t k = 1; k < 6; k += 2) {
        interesting.push_back({i, j, k});
        if (k != 5) {
          changing.push_back({i, j, static_cast<uint16_t>(k + 1)});
        }
      }
    }
  }
  for (uint16_t i = 1; i < 6; i += 2) {
    for (uint16_t j = 2; j < 5; j += 2) {
      for (uint16_t k = 1; k < 6; k += 2) {
        changing.push_back({i, j, k});
        changing.push_back({j, i, k});
      }
    }
  }
  interesting.insert(interesting.begin(), changing.begin(), changing.end());

  for (size_t i = 0; i < 10000; ++i) {
    for (size_t j = 0; j < 2; ++j) {
      choose_one_legal_mutation(map, changing.at(rand() % changing.size()));
    }
    check_random_paths(interesting, map);
  }
}


