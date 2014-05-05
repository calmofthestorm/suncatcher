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

using namespace suncatcher::test;

// Really simple tests for basic functionality.
class StaticSanity : public MapTest<MAP_OPEN> { };

TEST_F(StaticSanity, OpenMapOOB1) {
  ASSERT_FALSE(map.path({0, 0, 0}, {100, 100, 0}));
}

TEST_F(StaticSanity, OpenMapOOB2) {
  ASSERT_FALSE(map.path({0, 0, 0}, {200, 200, 0}));
}

TEST_F(StaticSanity, OpenMapIdentityOOB) {
  ASSERT_FALSE(map.path({100, 100, 0}, {100, 100, 0}));
}

TEST_F(StaticSanity, OpenMapIdentityPath) {
  ASSERT_EQ(
      Path({{0, 0, 0}}, 0),
      map.path({0, 0, 0}, {0, 0, 0})
    );
}

TEST_F(StaticSanity, OpenMapHorizPath) {
  ASSERT_EQ(
      Path({{1, 5, 0}, {1, 4, 0}, {1, 3, 0}, {1, 2, 0}, {1, 1, 0}}, 4),
      map.path({1, 5, 0}, {1, 1, 0})
    );
}

TEST_F(StaticSanity, OpenMapVertPath) {
  ASSERT_EQ(
      Path({{1, 1, 0}, {2, 1, 0}, {3, 1, 0}}, 2),
      map.path({1, 1, 0}, {3, 1, 0})
    );
}

TEST_F(StaticSanity, OpenMapDiagPath) {
  ASSERT_EQ(
      Path({{1, 1, 0}, {2, 2, 0}, {3, 3, 0}}, (float)2.8284271247461903),
      map.path({1, 1, 0}, {3, 3, 0})
    );
}

TEST_F(StaticSanity, OpenMapKnightPath1) {
  // Two equally good choices for this one.
  auto expected = Path({{1, 1, 0}, {1, 2, 0}, {3, 2, 0}}, (float)2.414213562373095);
  auto actual = map.path({1, 1, 0}, {3, 2, 0});
  ASSERT_NEAR(expected.get_length(), actual.get_length(), 0.001);
  ASSERT_EQ(expected.get_path().size(), actual.get_path().size());
}

TEST_F(StaticSanity, OpenMapKnightPath2) {
  // Two equally good choices for this one.
  auto expected = Path({{5, 4, 0}, {4, 4, 0}, {3, 3, 0}}, (float)2.414213562373095);
  auto actual = map.path({5, 4, 0}, {3, 3, 0});
  ASSERT_NEAR(expected.get_length(), actual.get_length(), 0.001);
  ASSERT_EQ(expected.get_path().size(), actual.get_path().size());
}

TEST_F(StaticSanity, OpenMapLongPath) {
  Coord start{23, 92, 0};
  Coord finish{97, 0, 0};
  auto actual = map.path(start, finish);
  ASSERT_NEAR(actual.get_length(), 122.652, 0.5);
  ASSERT_EQ(actual.get_path().size(), 93);

  // Delta test this one against MicroPather.
  #ifdef MICROPATHER_DELTA_TEST
  if (enable_micropather) {
    auto expected = get_micropather().path(start, finish);
    ASSERT_NEAR(expected.get_length(), actual.get_length(), 0.5);
    ASSERT_EQ(expected.get_path().size(), actual.get_path().size());
  }
  #endif
}

// Invalid cases involving walls
class StaticInvalidWall : public MapTest<MAP_MAIN> { };

TEST_F(StaticInvalidWall, ToSelf) {
  ASSERT_FALSE(map.path({0, 0, 0}, {0, 0, 0}));
}

TEST_F(StaticInvalidWall, ToAdj) {
  ASSERT_FALSE(map.path({0, 2, 0}, {0, 1, 0}));
}

TEST_F(StaticInvalidWall, ToOtherWall) {
  ASSERT_FALSE(map.path({11, 0, 0}, {55, 39, 0}));
}

TEST_F(StaticInvalidWall, ToOOB1) {
  ASSERT_FALSE(map.path({50, 0, 0}, {85, 100, 0}));
}

TEST_F(StaticInvalidWall, ToOOB2) {
  ASSERT_FALSE(map.path({0, 100, 0}, {55, 0, 0}));
}

// More interesting terrain-based tests.
class StaticTerrainPathing : public MapTest<MAP_MAIN> { };

// Upper left to just outside great hall.
TEST_F(StaticTerrainPathing, ULToJustOutsideGreatHall) {
  auto actual = map.path({1, 1, 0}, {67, 51, 0});
  ASSERT_NEAR(908.366, actual.get_length(), 0.5);
  ASSERT_EQ(887, actual.get_path().size());
}

// Upper right into great hall (door locked).
TEST_F(StaticTerrainPathing, ULToLockedGreatHall) {
  ASSERT_FALSE(map.path({1, 98, 0}, {69, 51, 0}));
}

// Upper right into left tower
TEST_F(StaticTerrainPathing, ULToStrongholdLeftTower) {
  auto actual = map.path({1, 98, 0}, {69, 43, 0});
  ASSERT_NEAR(991.994, actual.get_length(), 0.5);
  ASSERT_EQ(964, actual.get_path().size());
}

// Isolated component
TEST_F(StaticTerrainPathing, IsolatedInternal) {
  auto actual = map.path({68, 63, 0}, {68, 65, 0});
  ASSERT_EQ(Path({{68, 63, 0}, {68, 64, 0}, {68, 65, 0}}, 2), actual);
}

// Isolated component outside
TEST_F(StaticTerrainPathing, IsolatedInternalNoPath) {
  ASSERT_FALSE(map.path({68, 63, 0}, {1, 1, 0}));
}

TEST_F(StaticTerrainPathing, DiagonalWallHug) {
  ASSERT_EQ(
      Path({{4, 51, 0}, {5, 52, 0}, {6, 53, 0}, {7, 54, 0}, {8, 55, 0}, {9, 56, 0}, {10, 57, 0},
            {11, 58, 0}, {12, 59, 0}, {11, 60, 0}, {10, 59, 0}}, (float)14.142135623730951),
      map.path({4, 51, 0}, {10, 59, 0})
    );
}

TEST_F(StaticTerrainPathing, CornerToCornerLRUL) {
  auto actual = map.path({83, 98, 0}, {1, 1, 0});
  ASSERT_NEAR(939.064, actual.get_length(), 0.5);
  ASSERT_EQ(909, actual.get_path().size());
}

TEST_F(StaticTerrainPathing, CornerToCornerURLL) {
  auto actual = map.path({1, 97, 0}, {83, 1, 0});
  ASSERT_NEAR(1081.41, actual.get_length(), 0.5);
  ASSERT_EQ(1051, actual.get_path().size());
}

TEST_F(StaticTerrainPathing, NoBlockedDiagonalMoves) {
  auto actual = map.path({6, 55, 0}, {7, 54, 0});
  ASSERT_EQ(actual.get_path().size(), 10);
  ASSERT_NEAR(actual.get_length(), 12.7279, 0.5);
}

TEST_F(StaticTerrainPathing, DiagonalWallHugNoBlockedDiagonalMove) {
  auto actual = map.path({3, 67, 0}, {10, 74, 0});
  ASSERT_EQ(actual.get_path().size(), 10);
  ASSERT_NEAR(actual.get_length(), 12.7279, 0.5);
}

TEST_F(StaticTerrainPathing, DiagonalTube) {
  auto actual = map.path({5, 79, 0}, {9, 80, 0});
  ASSERT_EQ(actual.get_path().size(), 11);
  ASSERT_NEAR(actual.get_length(), 13.728, 0.5);
}

// Static door tests (open ones are pathable, closed are not.)
class StaticDoorSanity : public MapTest<MAP_MAIN> { };

TEST_F(StaticDoorSanity, OpenToSelf) {
  auto actual = map.path({8, 28, 0}, {8, 28, 0});
  ASSERT_EQ(actual.get_path().size(), 1);
  ASSERT_NEAR(actual.get_length(), 0, 0.001);
}

TEST_F(StaticDoorSanity, OpenToSpace) {
  auto actual = map.path({8, 28, 0}, {10, 28, 0});
  ASSERT_EQ(actual.get_path().size(), 3);
  ASSERT_NEAR(actual.get_length(), 2, 0.001);
}

TEST_F(StaticDoorSanity, SpaceToOpen) {
  auto actual = map.path({10, 28, 0}, {8, 28, 0});
  ASSERT_EQ(actual.get_path().size(), 3);
  ASSERT_NEAR(actual.get_length(), 2, 0.001);
}

TEST_F(StaticDoorSanity, OpenToOpen) {
  auto actual = map.path({8, 28, 0}, {33, 49, 0});
  ASSERT_EQ(actual.get_path().size(), 746);
  ASSERT_NEAR(actual.get_length(), 755.35516357421875, 0.001);
}

TEST_F(StaticDoorSanity, OpenToClosed) {
  ASSERT_FALSE(map.path({8, 28, 0}, {70, 51, 0}));
}

TEST_F(StaticDoorSanity, ClosedToOpen) {
  ASSERT_FALSE(map.path({70, 51, 0}, {8, 28, 0}));
}

TEST_F(StaticDoorSanity, ClosedToSelf) {
  ASSERT_FALSE(map.path({71, 18, 0}, {71, 18, 0}));
}

TEST_F(StaticDoorSanity, ClosedToSpace) {
  ASSERT_FALSE(map.path({71, 18, 0}, {68, 18, 0}));
}

TEST_F(StaticDoorSanity, SpaceToClosed) {
  ASSERT_FALSE(map.path({68, 18, 0}, {71, 18, 0}));
}

// Welcome to Doorland
class StaticDoorland : public MapTest<MAP_DOORLAND> { };

TEST_F(StaticDoorland, LockedDoorWall) {
  Coord inside{2, 2, 0};
  ASSERT_FALSE(map.path(inside, {1, 4, 0}));
  ASSERT_FALSE(map.path(inside, {2, 4, 0}));
  ASSERT_FALSE(map.path(inside, {3, 4, 0}));
  ASSERT_FALSE(map.path(inside, {1, 3, 0}));
  ASSERT_FALSE(map.path(inside, {2, 3, 0}));
  ASSERT_FALSE(map.path(inside, {3, 3, 0}));
}


TEST_F(StaticDoorland, OpenDoorWall) {
  Coord inside{2, 6, 0};
  ASSERT_TRUE((bool)map.path(inside, {1, 8, 0}));
  ASSERT_TRUE((bool)map.path(inside, {2, 8, 0}));
  ASSERT_TRUE((bool)map.path(inside, {3, 8, 0}));
  ASSERT_TRUE((bool)map.path(inside, {1, 7, 0}));
  ASSERT_TRUE((bool)map.path(inside, {2, 7, 0}));
  ASSERT_TRUE((bool)map.path(inside, {3, 7, 0}));
}


TEST_F(StaticDoorland, PathThroughDoors) {
  ASSERT_TRUE((bool)map.path({1, 9, 0}, {3, 12, 0}));
  ASSERT_TRUE((bool)map.path({1, 12, 0}, {6, 6, 0}));
}
