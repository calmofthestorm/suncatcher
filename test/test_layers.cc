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


using namespace suncatcher::test;

class Basic3D : public MapTest<MAP_CUBE3> { };

TEST_F(Basic3D, Trivial) {
  ASSERT_NEAR(map.path({0, 0, 0}, {0, 0, 0}).get_length(), 0, 0.1);
  ASSERT_NEAR(map.path({0, 0, 0}, {2, 2, 2}).get_length(), 4.82842712474619, 0.1);
  ASSERT_FALSE(map.path({0, 0, 0}, {2, 2, 3}));
}

TEST_F(Basic3D, Hill) {
  Coord start(0, 2, 0);
  Coord finish(0, 0, 0);
  auto m = map.get_mutator();
  for (uint16_t k = 0; k < 2; ++k) {
    for (uint16_t j = 0; j < 3; ++j) {
      m.set_cost({j, 1, k}, PATH_COST_INFINITE);
    }
  }
  map.mutate(m);

  ASSERT_NEAR(map.path(start, finish).get_length(), 6, 0.1);
  map.mutate(map.get_mutator().set_cost({0, 1, 2}, PATH_COST_INFINITE));
  ASSERT_NEAR(map.path(start, finish).get_length(), 6.828427315, 0.1);
  map.mutate(map.get_mutator().set_cost({1, 1, 2}, PATH_COST_INFINITE));
  ASSERT_NEAR(map.path(start, finish).get_length(), 8.828427315, 0.1);
  map.mutate(map.get_mutator().set_cost({2, 1, 2}, PATH_COST_INFINITE));
  ASSERT_FALSE(map.path(start, finish));
}

class Mutation3D : public MapTest<MAP_CUBE3> { };

TEST_F(Mutation3D, Hill) {
  Coord start(1, 2, 0);
  Coord finish(1, 0, 0);
  auto m = map.get_mutator();
  for (uint16_t k = 0; k < 3; ++k) {
    for (uint16_t j = 0; j < 3; ++j) {
      m.set_cost({j, 1, k}, PATH_COST_INFINITE);
    }
  }
  map.mutate(m);
  ASSERT_FALSE(map.path(start, finish));
  map.mutate(map.get_mutator().set_cost({1, 1, 1}, 203));
  ASSERT_NEAR(map.path(start, finish).get_length(), 206, 0.1);
  map.mutate(map.get_mutator().create_door({1, 1, 2}, false, 10));
  ASSERT_NEAR(map.path(start, finish).get_length(), 206, 0.1);
  map.mutate(map.get_mutator().toggle_door_open({1, 1, 2}));
  ASSERT_NEAR(map.path(start, finish).get_length(), 15, 0.1);
  map.mutate(map.get_mutator().toggle_door_open({1, 1, 2}));
  ASSERT_NEAR(map.path(start, finish).get_length(), 206, 0.1);
  map.mutate(map.get_mutator().set_cost({1, 1, 1}, PATH_COST_INFINITE));
  map.mutate(map.get_mutator().toggle_door_open({1, 1, 2}));
  ASSERT_NEAR(map.path(start, finish).get_length(), 15, 0.1);
  map.mutate(map.get_mutator().set_cost({1, 1, 1}, 0));
  ASSERT_NEAR(map.path(start, finish).get_length(), 3, 0.1);
  map.mutate(map.get_mutator().remove_door({1, 1, 2}, PATH_COST_INFINITE));
  map.mutate(map.get_mutator().set_cost({1, 1, 1}, 0));
  map.mutate(map.get_mutator().set_cost({1, 1, 1}, PATH_COST_INFINITE));
  ASSERT_FALSE(map.path(start, finish));
}

class Octants3D : public MapTest<MAP_OCTANTS> {
  protected:
    virtual void make_impassable(Coord cell) = 0;
    virtual void make_passable(Coord cell) = 0;
    void check();
};

class SimpleOctants3D : public Octants3D {
  protected:
    virtual void make_impassable(Coord cell) {
      map.mutate(map.get_mutator().set_cost(cell, PATH_COST_INFINITE));
    }

    virtual void make_passable(Coord cell) {
      map.mutate(map.get_mutator().set_cost(cell, 10));
    }
};


void Octants3D::check() {
  const std::vector<Coord> oct{
    Coord{1, 1, 1}, Coord{1, 3, 1}, Coord{3, 1, 1}, Coord{3, 3, 1},
    Coord{1, 1, 3}, Coord{1, 3, 3}, Coord{3, 1, 3}, Coord{3, 3, 3}
  };

  for (const auto& c1 : oct) {
    for (const auto& c2 : oct) {
      ASSERT_EQ((bool)map.path(c1, c2), c1 == c2);
    }
  }

  make_passable({1, 2, 1});
  for (const auto& c1 : oct) {
    for (const auto& c2 : oct) {
      ASSERT_EQ((bool)map.path(c1, c2), c1 == c2 ||
                (c1 == oct[0] && c2 == oct[1])||
                (c2 == oct[0] && c1 == oct[1]));
    }
  }

  make_passable({3, 2, 1});
  make_passable({1, 2, 3});
  make_passable({3, 2, 3});
  for (const auto& c1 : oct) {
    for (const auto& c2 : oct) {
      ASSERT_EQ((bool)map.path(c1, c2), c1 == c2 || (c1.row == c2.row && c1.layer == c2.layer));
    }
  }

  make_passable({2, 2, 1});
  make_passable({2, 2, 3});
  for (const auto& c1 : oct) {
    for (const auto& c2 : oct) {
      ASSERT_EQ((bool)map.path(c1, c2), c1 == c2 || (c1.layer == c2.layer));
    }
  }

  make_passable({1, 1, 2});
  for (const auto& c1 : oct) {
    for (const auto& c2 : oct) {
      ASSERT_TRUE((bool)map.path(c1, c2));
    }
  }

  make_impassable({1, 2, 1});
  make_impassable({3, 2, 1});
  make_impassable({1, 2, 3});
  make_impassable({3, 2, 3});
  ASSERT_TRUE((bool)map.path(oct[0], oct[4]));

  make_impassable({2, 2, 1});
  make_impassable({2, 2, 3});
  make_impassable({1, 1, 2});
}


TEST_F(SimpleOctants3D, Check) {
  check();
}


class DynamicOctants3D : public Octants3D {
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


TEST_F(DynamicOctants3D, Check) {
  for (size_t i = 0; i < 1; ++i) {
    check();
  }
}


class ComplexOctants3D : public Octants3D {
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


TEST_F(ComplexOctants3D, Check) {
  for (size_t i = 0; i < 5; ++i) {
    check();
  }
}
