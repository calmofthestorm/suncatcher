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

#include "suncatcher/test/common.hh"

#include "suncatcher/Coord.hh"
#include "suncatcher/test/DeltaMap.hh"
#include "suncatcher/MapMutator.hh"
#include "suncatcher/util/util.hh"
#include "suncatcher/test/ResourceManager.hh"
#include "suncatcher/CoordRange.hh"


using namespace suncatcher::test;

// ****************************************************************************
// *** Single mutations, as simple as possible, affect the map.
// ****************************************************************************

// Door update
class OptOpenCloseDoor : public MapTest<MAP_MICRO> { };

TEST_F(OptOpenCloseDoor, ConnectedComponentsMaintained) {
  Coord door1 = {2, 4, 0};
  Coord door2 = {8, 4, 0};
  Coord door3 = {12, 8, 0};
  Coord cell1 = {3, 4, 0};
  Coord cell2 = {7, 4, 0};
  ASSERT_TRUE(map.path_exists(cell1, cell2));
  map.mutate(std::move(map.get_mutator().toggle_door_open(door1)));
  ASSERT_FALSE(map.path_exists(cell1, cell2));
  map.mutate(std::move(map.get_mutator().toggle_door_open(door2)));
  ASSERT_FALSE(map.path_exists(cell1, cell2));
  map.mutate(std::move(map.get_mutator().toggle_door_open(door1)));
  ASSERT_FALSE(map.path_exists(cell1, cell2));
  map.mutate(std::move(map.get_mutator().toggle_door_open({6, 4, 0})));
  ASSERT_TRUE(map.path_exists(cell1, cell2));
  map.mutate(std::move(map.get_mutator().toggle_door_open({6, 4, 0})));
  ASSERT_FALSE(map.path_exists(cell1, cell2));
  map.mutate(std::move(map.get_mutator().toggle_door_open(door2)));
  ASSERT_TRUE(map.path_exists(cell1, cell2));
}

TEST_F(OptOpenCloseDoor, SLOWSmartBruteForceTorture) {
  std::vector<Coord> doors{Coord{2, 4, 0}, Coord{6, 4, 0}, Coord{8, 4, 0},
    Coord{12, 8, 0}, Coord{14, 2, 0}, Coord{11, 8, 0}};
  std::vector<Coord> interesting{{1, 1, 0}, {3, 4, 0}, {7, 4, 0}};

  size_t num_iterations = std::pow(2, doors.size());

  map.mutate(std::move(map.get_mutator().create_door(doors.back(), false, 3)));

  for (size_t i = 0; i < 10; ++i) {
    // Set door configuration.
    for (size_t iterations = 0; iterations < num_iterations; ++iterations) {
      size_t mask = 1;
      for (const auto& door : doors) {
        if (mask & iterations) {
          map.mutate(std::move(map.get_mutator().toggle_door_open(door)));
        }
        mask <<= 1;
      }

      bool top_open = map.get_doors().at({2, 4, 0}).open;
      bool bot_open = (map.get_doors().at({6, 4, 0}).open ||
                       map.get_doors().at({8, 4, 0}).open);

      // Handle paths between open spaces.
      for (const auto& it : interesting) {
        for (const auto& jt : interesting) {
          bool is_path = map.path_exists(it, jt);
          if (it < jt) {
            if (it == Coord{3, 4, 0} && jt == Coord{7, 4, 0}) {
              ASSERT_EQ(is_path, top_open && bot_open);
            } else if (it == Coord{3, 4, 0} || jt == Coord{3, 4, 0}) {
                ASSERT_EQ(is_path, top_open);
            } else if (it == Coord{7, 4, 0} || jt == Coord{7, 4, 0}) {
              ASSERT_EQ(is_path, bot_open);
            } else {
              ASSERT_TRUE(is_path);
            }
          } else if (it == jt) {
            ASSERT_TRUE(is_path);
          }
        }
      }

      // Handle paths between doors.
      for (const auto& it : map.get_doors()) {
        for (const auto& jt : map.get_doors()) {
          if (it.first <= jt.first) {
            bool is_path = map.path_exists(it.first, jt.first);
            ASSERT_EQ(is_path, it.second.open && jt.second.open);
          }
        }
      }

      // Handle paths between a door and an open space.
      for (const auto& door : map.get_doors()) {
        for (const auto& square : interesting) {
          bool is_path = map.path_exists(door.first, square);
          bool should = door.second.open;
          should &= square != Coord{3, 4, 0} || top_open;
          should &= square != Coord{7, 4, 0} || bot_open;
          ASSERT_EQ(is_path, should);
        }
      }

      // Restore door configuration.
      auto mutator = map.get_mutator();
      mask = 1;
      for (const auto& door : doors) {
        if (mask & iterations) {
          mutator.toggle_door_open(door);
        }
        mask <<= 1;
      }
      map.mutate(std::move(mutator));
    }
  }
}

// Creating/Removing walls.
class CrossWorld : public MapTest<MAP_CROSS> {
  protected:
    virtual void make_impassable(Coord cell) = 0;
    virtual void make_passable(Coord cell) = 0;
    void check();
};

void CrossWorld::check() {
  make_passable({3, 5, 2});
  ASSERT_TRUE(map.path_exists({1, 1, 2}, {1, 8, 2}));
  ASSERT_FALSE(map.path_exists({1, 1, 2}, {8, 1, 2}));
  ASSERT_FALSE(map.path_exists({1, 1, 2}, {8, 8, 2}));
  ASSERT_FALSE(map.path_exists({1, 8, 2}, {8, 1, 2}));
  ASSERT_FALSE(map.path_exists({1, 8, 2}, {8, 8, 2}));
  ASSERT_FALSE(map.path_exists({8, 1, 2}, {8, 8, 2}));
  make_passable({7, 5, 2});
  ASSERT_TRUE(map.path_exists({1, 1, 2}, {1, 8, 2}));
  ASSERT_TRUE(map.path_exists({8, 1, 2}, {8, 8, 2}));
  ASSERT_FALSE(map.path_exists({1, 1, 2}, {8, 1, 2}));
  ASSERT_FALSE(map.path_exists({1, 1, 2}, {8, 8, 2}));
  ASSERT_FALSE(map.path_exists({1, 8, 2}, {8, 1, 2}));
  ASSERT_FALSE(map.path_exists({1, 8, 2}, {8, 8, 2}));
  make_impassable({3, 5, 2});
  ASSERT_TRUE(map.path_exists({8, 1, 2}, {8, 8, 2}));
  ASSERT_FALSE(map.path_exists({1, 1, 2}, {1, 8, 2}));
  ASSERT_FALSE(map.path_exists({1, 1, 2}, {8, 1, 2}));
  ASSERT_FALSE(map.path_exists({1, 1, 2}, {8, 8, 2}));
  ASSERT_FALSE(map.path_exists({1, 8, 2}, {8, 1, 2}));
  ASSERT_FALSE(map.path_exists({1, 8, 2}, {8, 8, 2}));
  make_passable({2, 9, 2});
  ASSERT_TRUE(map.path_exists({8, 1, 2}, {8, 8, 2}));
  ASSERT_FALSE(map.path_exists({1, 1, 2}, {1, 8, 2}));
  ASSERT_FALSE(map.path_exists({1, 1, 2}, {8, 1, 2}));
  ASSERT_FALSE(map.path_exists({1, 1, 2}, {8, 8, 2}));
  ASSERT_FALSE(map.path_exists({1, 8, 2}, {8, 1, 2}));
  ASSERT_FALSE(map.path_exists({1, 8, 2}, {8, 8, 2}));
  ASSERT_TRUE(map.path_exists({1, 8, 2}, {10, 10, 2}));
  ASSERT_FALSE(map.path_exists({1, 1, 2}, {10, 10, 2}));
  make_passable({5, 2, 2});
  ASSERT_TRUE(map.path_exists({8, 1, 2}, {8, 8, 2}));
  ASSERT_FALSE(map.path_exists({1, 1, 2}, {1, 8, 2}));
  ASSERT_TRUE(map.path_exists({1, 1, 2}, {8, 1, 2}));
  ASSERT_TRUE(map.path_exists({1, 1, 2}, {8, 8, 2}));
  ASSERT_FALSE(map.path_exists({1, 8, 2}, {8, 1, 2}));
  ASSERT_FALSE(map.path_exists({1, 8, 2}, {8, 8, 2}));
  ASSERT_TRUE(map.path_exists({1, 8, 2}, {10, 10, 2}));
  ASSERT_FALSE(map.path_exists({1, 1, 2}, {10, 10, 2}));
  make_passable({4, 5, 2});
  ASSERT_TRUE(map.path_exists({8, 1, 2}, {8, 8, 2}));
  ASSERT_TRUE(map.path_exists({1, 1, 2}, {1, 8, 2}));
  ASSERT_TRUE(map.path_exists({1, 1, 2}, {8, 1, 2}));
  ASSERT_TRUE(map.path_exists({1, 1, 2}, {8, 8, 2}));
  ASSERT_TRUE(map.path_exists({1, 8, 2}, {8, 1, 2}));
  ASSERT_TRUE(map.path_exists({1, 8, 2}, {8, 8, 2}));
  ASSERT_TRUE(map.path_exists({1, 8, 2}, {10, 10, 2}));
  ASSERT_TRUE(map.path_exists({1, 1, 2}, {10, 10, 2}));
  make_passable({6, 7, 2});
  ASSERT_TRUE(map.path_exists({8, 1, 2}, {8, 8, 2}));
  ASSERT_TRUE(map.path_exists({1, 1, 2}, {1, 8, 2}));
  ASSERT_TRUE(map.path_exists({1, 1, 2}, {8, 1, 2}));
  ASSERT_TRUE(map.path_exists({1, 1, 2}, {8, 8, 2}));
  ASSERT_TRUE(map.path_exists({1, 8, 2}, {8, 1, 2}));
  ASSERT_TRUE(map.path_exists({1, 8, 2}, {8, 8, 2}));
  ASSERT_TRUE(map.path_exists({1, 8, 2}, {10, 10, 2}));
  ASSERT_TRUE(map.path_exists({1, 1, 2}, {10, 10, 2}));
  make_passable({5, 6, 2});
  ASSERT_TRUE(map.path_exists({8, 1, 2}, {8, 8, 2}));
  ASSERT_TRUE(map.path_exists({1, 1, 2}, {1, 8, 2}));
  ASSERT_TRUE(map.path_exists({1, 1, 2}, {8, 1, 2}));
  ASSERT_TRUE(map.path_exists({1, 1, 2}, {8, 8, 2}));
  ASSERT_TRUE(map.path_exists({1, 8, 2}, {8, 1, 2}));
  ASSERT_TRUE(map.path_exists({1, 8, 2}, {8, 8, 2}));
  ASSERT_TRUE(map.path_exists({1, 8, 2}, {10, 10, 2}));
  ASSERT_TRUE(map.path_exists({1, 1, 2}, {10, 10, 2}));
  make_impassable({7, 5, 2});
  make_impassable({2, 9, 2});
  make_impassable({5, 2, 2});
  make_impassable({4, 5, 2});
  make_impassable({6, 7, 2});
  make_impassable({5, 6, 2});
}


class OptWall : public CrossWorld {
  protected:
    virtual void make_impassable(Coord cell) {
      map.mutate(std::move(map.get_mutator().set_cost(cell, PATH_COST_INFINITE)));
    }

    virtual void make_passable(Coord cell) {
      map.mutate(std::move(map.get_mutator().set_cost(cell, 10)));
    }
};


// Verifies that when we do a flood fill of all neighbors of a wall,
// we look at the static components of the neighbors, rather than their
// colors.
TEST_F(OptWall, ColorComponentRegression) {
  auto m = map.get_mutator();
  m.set_cost({3, 5, 2}, 1);
  m.set_cost({7, 5, 2}, 1);
  map.mutate(m);
  map.mutate(map.get_mutator().set_cost({5, 2, 2}, 1));
  map.mutate(map.get_mutator().set_cost({5, 2, 2}, PATH_COST_INFINITE));
  ASSERT_FALSE(map.path_exists({4, 6, 2}, {6, 6, 2}));
}


TEST_F(OptWall, Sanity) {
  std::vector<Coord> interesting{Coord{1, 1, 2}, Coord{1, 8, 2}, Coord{8, 8, 2},
                                 Coord{8, 1, 2}, Coord{10, 10, 2}};
  for (const auto& it : interesting) {
    for (const auto& jt : interesting) {
      ASSERT_EQ(map.path_exists(it, jt), it == jt);
    }
  }
}


TEST_F(OptWall, SameComponent) {
  ASSERT_TRUE(map.path_exists({1, 1, 2}, {3, 3, 2}));
  ASSERT_TRUE(map.path_exists({1, 8, 2}, {3, 6, 2}));
  ASSERT_TRUE(map.path_exists({8, 8, 2}, {6, 6, 2}));
  ASSERT_TRUE(map.path_exists({8, 1, 2}, {6, 3, 2}));
  ASSERT_TRUE(map.path_exists({10, 10, 2}, {10, 0, 2}));
  ASSERT_TRUE(map.path_exists({10, 10, 2}, {0, 10, 2}));
}


TEST_F(OptWall, Transitive) {
  check();
}


class DoorTransitivityStatic : public CrossWorld {
  protected:
    virtual void make_impassable(Coord cell) {
      map.mutate(std::move(map.get_mutator().create_door(cell, false, 10)));
    }

    virtual void make_passable(Coord cell) {
      if (map.is_door(cell)) {
        map.mutate(std::move(map.get_mutator().remove_door(cell, 10)));
      } else {
        map.mutate(std::move(map.get_mutator().set_cost(cell, 10)));
      }
    }
};

TEST_F(DoorTransitivityStatic, TransitiveSimple) {
  for (size_t i = 0; i < 2; ++i) {
    check();
    map.clear_cache();
  }
}


class DoorTransitivityDynamic : public CrossWorld {
  protected:
    virtual void make_impassable(Coord cell) {
      if (map.is_door(cell)) {
        map.mutate(std::move(map.get_mutator().set_door_open(cell, false)));
      } else {
        map.mutate(std::move(map.get_mutator().create_door(cell, false, 10)));
      }
    }

    virtual void make_passable(Coord cell) {
      if (map.is_door(cell)) {
        map.mutate(std::move(map.get_mutator().set_door_open(cell, true)));
      } else {
        map.mutate(std::move(map.get_mutator().create_door(cell, true, 10)));
      }
    }
};


TEST_F(DoorTransitivityDynamic, Transitive) {
  for (size_t i = 0; i < 5; ++i) {
    check();
  }
}


class DoorTransitivityComplex : public CrossWorld {
  protected:
    virtual void make_impassable(Coord cell) {
      static size_t i = 0;
      if (map.is_door(cell)) {
        if (i++ % 2) {
          map.mutate(std::move(map.get_mutator().remove_door(cell, PATH_COST_INFINITE)));
          map.mutate(std::move(map.get_mutator().create_door(cell, false, 10)));
        } else {
          map.mutate(std::move(map.get_mutator().remove_door(cell, PATH_COST_INFINITE)));
        }
      } else {
        if (i++ % 2) {
          map.mutate(std::move(map.get_mutator().create_door(cell, false, 10)));
        } else {
          map.mutate(std::move(map.get_mutator().set_cost(cell, PATH_COST_INFINITE)));
        }
      }
    }

    virtual void make_passable(Coord cell) {
      static size_t i = 0;
      if (map.is_door(cell)) {
          map.mutate(std::move(map.get_mutator().remove_door(cell, 10)));
          map.mutate(std::move(map.get_mutator().create_door(cell, true, 10)));
      } else {
        map.mutate(std::move(map.get_mutator().create_door(cell, true, 10)));
      }
    }
};


TEST_F(DoorTransitivityComplex, Transitive) {
  for (size_t i = 0; i < 5; ++i) {
    check();
  }
}

class SolidWorld : public MapTest<MAP_SMALL_OPEN> {
  protected:
    virtual void SetUp() final override {
      auto m = map.get_mutator();
      for (const Coord& c : suncatcher::pathfinder::CoordRange(map.size())) {
        m.set_cost(c, PATH_COST_INFINITE);
      }
      map.mutate(std::move(m));
    }

    void check();
};

TEST_F(SolidWorld, Simple) {
  for (const auto& dest : {Coord{5, 7, 0}, Coord{6, 6, 0}}) {
    ASSERT_FALSE(map.path_exists({5, 5, 0}, {5, 5, 0}));
    map.mutate(std::move(map.get_mutator().set_cost({5, 5, 0}, 10)));
    ASSERT_TRUE(map.path_exists({5, 5, 0}, {5, 5, 0}));
    ASSERT_FALSE(map.path_exists({5, 5, 0}, dest));
    ASSERT_FALSE(map.path_exists(dest, dest));
    map.mutate(std::move(map.get_mutator().set_cost(dest, 30)));
    ASSERT_TRUE(map.path_exists({5, 5, 0}, {5, 5, 0}));
    ASSERT_FALSE(map.path_exists({5, 5, 0}, dest));
    ASSERT_TRUE(map.path_exists(dest, dest));
    map.mutate(std::move(map.get_mutator().set_cost({5, 6, 0}, 0)));
    ASSERT_TRUE(map.path_exists({5, 5, 0}, {5, 5, 0}));
    ASSERT_TRUE(map.path_exists({5, 5, 0}, dest));
    ASSERT_TRUE(map.path_exists(dest, dest));
    map.mutate(std::move(map.get_mutator().create_door({5, 6, 0}, false, 10)));
    ASSERT_TRUE(map.path_exists({5, 5, 0}, {5, 5, 0}));
    ASSERT_FALSE(map.path_exists({5, 5, 0}, dest));
    ASSERT_TRUE(map.path_exists(dest, dest));
    map.mutate(std::move(map.get_mutator().set_door_open({5, 6, 0}, true)));
    ASSERT_TRUE(map.path_exists({5, 5, 0}, {5, 5, 0}));
    ASSERT_TRUE(map.path_exists({5, 5, 0}, dest));
    ASSERT_TRUE(map.path_exists(dest, dest));

    map.mutate(std::move(map.get_mutator().remove_door({5, 6, 0}, PATH_COST_INFINITE)));
    map.mutate(std::move(map.get_mutator().set_cost({5, 5, 0}, PATH_COST_INFINITE)));
    map.mutate(std::move(map.get_mutator().set_cost(dest, PATH_COST_INFINITE)));
  }
}


TEST_F(SolidWorld, ConnectDisconnect) {
  std::vector<Coord> inner_cross{Coord{4, 5, 0}, Coord{6, 5, 0}, Coord{5, 5, 0},
                                 Coord{5, 4, 0}, Coord{5, 6, 0}};
  auto m = map.get_mutator();
  for (const auto& it : inner_cross) {
    m.set_cost(it, 10);
  }
  map.mutate(std::move(m));

  for (const auto& it : inner_cross) {
    for (const auto& jt : inner_cross) {
      ASSERT_TRUE(map.path_exists(it, jt));
    }
  }

  map.mutate(std::move(map.get_mutator().set_cost({5, 5, 0}, PATH_COST_INFINITE)));

  for (const auto& it : inner_cross) {
    for (const auto& jt : inner_cross) {
      ASSERT_EQ(map.path_exists(it, jt), (it == jt && it != Coord{5, 5, 0}));
    }
  }

  map.mutate(std::move(map.get_mutator().create_door({5, 5, 0}, true, 10)));

  for (const auto& it : inner_cross) {
    for (const auto& jt : inner_cross) {
      ASSERT_TRUE(map.path_exists(it, jt));
    }
  }

  map.mutate(std::move(map.get_mutator().set_door_open({5, 5, 0}, false)));

  for (const auto& it : inner_cross) {
    for (const auto& jt : inner_cross) {
      ASSERT_EQ(map.path_exists(it, jt), (it == jt && it != Coord{5, 5, 0}));
    }
  }

  map.mutate(std::move(map.get_mutator().remove_door({5, 5, 0}, 10)));

  for (const auto& it : inner_cross) {
    for (const auto& jt : inner_cross) {
      ASSERT_TRUE(map.path_exists(it, jt));
    }
  }
}
