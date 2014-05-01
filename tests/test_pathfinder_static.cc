#include "test_pathfinder_common.h"

// Really simple tests for basic functionality.
class StaticSanity : public StaticMapTest<test::MAP_OPEN> { };

TEST_F(StaticSanity, OpenMapOOB1) {
  ASSERT_FALSE(map.path({0, 0}, {100, 100}));
}

TEST_F(StaticSanity, OpenMapOOB2) {
  ASSERT_FALSE(map.path({0, 0}, {200, 200}));
}

TEST_F(StaticSanity, OpenMapIdentityOOB) {
  ASSERT_FALSE(map.path({100, 100}, {100, 100}));
}

TEST_F(StaticSanity, OpenMapIdentityPath) {
  ASSERT_EQ(
      Path({{0, 0}}, 0),
      map.path({0, 0}, {0, 0})
    );
}

TEST_F(StaticSanity, OpenMapHorizPath) {
  ASSERT_EQ(
      Path({{1, 5}, {1, 4}, {1, 3}, {1, 2}, {1, 1}}, 4),
      map.path({1, 5}, {1, 1})
    );
}

TEST_F(StaticSanity, OpenMapVertPath) {
  ASSERT_EQ(
      Path({{1, 1}, {2, 1}, {3, 1}}, 2),
      map.path({1, 1}, {3, 1})
    );
}

TEST_F(StaticSanity, OpenMapDiagPath) {
  ASSERT_EQ(
      Path({{1, 1}, {2, 2}, {3, 3}}, (float)2.8284271247461903),
      map.path({1, 1}, {3, 3})
    );
}

TEST_F(StaticSanity, OpenMapKnightPath1) {
  // Two equally good choices for this one.
  auto expected = Path({{1, 1}, {1, 2}, {3, 2}}, (float)2.414213562373095);
  auto actual = map.path({1, 1}, {3, 2});
  ASSERT_NEAR(expected.get_length(), actual.get_length(), 0.001);
  ASSERT_EQ(expected.get_path().size(), actual.get_path().size());
}

TEST_F(StaticSanity, OpenMapKnightPath2) {
  // Two equally good choices for this one.
  auto expected = Path({{5, 4}, {4, 4}, {3, 3}}, (float)2.414213562373095);
  auto actual = map.path({5, 4}, {3, 3});
  ASSERT_NEAR(expected.get_length(), actual.get_length(), 0.001);
  ASSERT_EQ(expected.get_path().size(), actual.get_path().size());
}

TEST_F(StaticSanity, OpenMapLongPath) {
  Coord start{23, 92};
  Coord finish{97, 0};
  auto actual = map.path(start, finish);
  ASSERT_NEAR(actual.get_length(), 122.652, 0.5);
  ASSERT_EQ(actual.get_path().size(), 93);

  // Delta test this one against MicroPather.
  #ifdef MICROPATHER_DELTA_TEST
    auto expected = empty_mp->path(start, finish);
    ASSERT_NEAR(expected.get_length(), actual.get_length(), 0.5);
    ASSERT_EQ(expected.get_path().size(), actual.get_path().size());
  #endif
}

// Invalid cases involving walls
class StaticInvalidWall : public StaticMapTest<test::MAP_MAIN> { };

TEST_F(StaticInvalidWall, ToSelf) {
  ASSERT_FALSE(map.path({0, 0}, {0, 0}));
}

TEST_F(StaticInvalidWall, ToAdj) {
  ASSERT_FALSE(map.path({0, 2}, {0, 1}));
}

TEST_F(StaticInvalidWall, ToOtherWall) {
  ASSERT_FALSE(map.path({11, 0}, {55, 39}));
}

TEST_F(StaticInvalidWall, ToOOB1) {
  ASSERT_FALSE(map.path({50, 0}, {85, 100}));
}

TEST_F(StaticInvalidWall, ToOOB2) {
  ASSERT_FALSE(map.path({0, 100}, {55, 0}));
}

// More interesting terrain-based tests.
class StaticTerrainPathing : public StaticMapTest<test::MAP_MAIN> { };

// Upper left to just outside great hall.
TEST_F(StaticTerrainPathing, ULToJustOutsideGreatHall) {
  auto actual = map.path({1, 1}, {67, 51});
  ASSERT_NEAR(908.366, actual.get_length(), 0.5);
  ASSERT_EQ(887, actual.get_path().size());
}

// Upper right into great hall (door locked).
TEST_F(StaticTerrainPathing, ULToLockedGreatHall) {
  ASSERT_FALSE(map.path({1, 98}, {69, 51}));
}

// Upper right into left tower
TEST_F(StaticTerrainPathing, ULToStrongholdLeftTower) {
  auto actual = map.path({1, 98}, {69, 43});
  ASSERT_NEAR(991.994, actual.get_length(), 0.5);
  ASSERT_EQ(964, actual.get_path().size());
}

// Isolated component
TEST_F(StaticTerrainPathing, IsolatedInternal) {
  auto actual = map.path({68, 63}, {68, 65});
  ASSERT_EQ(Path({{68, 63}, {68, 64}, {68, 65}}, 2), actual);
}

// Isolated component outside
TEST_F(StaticTerrainPathing, IsolatedInternalNoPath) {
  ASSERT_FALSE(map.path({68, 63}, {1, 1}));
}

TEST_F(StaticTerrainPathing, DiagonalWallHug) {
  ASSERT_EQ(
      Path({{4, 51}, {5, 52}, {6, 53}, {7, 54}, {8, 55}, {9, 56}, {10, 57},
            {11, 58}, {12, 59}, {11, 60}, {10, 59}}, (float)14.142135623730951),
      map.path({4, 51}, {10, 59})
    );
}

TEST_F(StaticTerrainPathing, CornerToCornerLRUL) {
  auto actual = map.path({83, 98}, {1, 1});
  ASSERT_NEAR(939.064, actual.get_length(), 0.5);
  ASSERT_EQ(909, actual.get_path().size());
}

TEST_F(StaticTerrainPathing, CornerToCornerURLL) {
  auto actual = map.path({1, 97}, {83, 1});
  ASSERT_NEAR(1081.41, actual.get_length(), 0.5);
  ASSERT_EQ(1051, actual.get_path().size());
}

TEST_F(StaticTerrainPathing, NoBlockedDiagonalMoves) {
  auto actual = map.path({6, 55}, {7, 54});
  ASSERT_EQ(actual.get_path().size(), 10);
  ASSERT_NEAR(actual.get_length(), 12.7279, 0.5);
}

TEST_F(StaticTerrainPathing, DiagonalWallHugNoBlockedDiagonalMove) {
  auto actual = map.path({3, 67}, {10, 74});
  ASSERT_EQ(actual.get_path().size(), 10);
  ASSERT_NEAR(actual.get_length(), 12.7279, 0.5);
}

TEST_F(StaticTerrainPathing, DiagonalTube) {
  auto actual = map.path({5, 79}, {9, 80});
  ASSERT_EQ(actual.get_path().size(), 11);
  ASSERT_NEAR(actual.get_length(), 13.728, 0.5);
}

// Static door tests (open ones are pathable, closed are not.)
class StaticDoorSanity : public StaticMapTest<test::MAP_MAIN> { };

TEST_F(StaticDoorSanity, OpenToSelf) {
  auto actual = map.path({8, 28}, {8, 28});
  ASSERT_EQ(actual.get_path().size(), 1);
  ASSERT_NEAR(actual.get_length(), 0, 0.001);
}

TEST_F(StaticDoorSanity, OpenToSpace) {
  auto actual = map.path({8, 28}, {10, 28});
  ASSERT_EQ(actual.get_path().size(), 3);
  ASSERT_NEAR(actual.get_length(), 2, 0.001);
}

TEST_F(StaticDoorSanity, SpaceToOpen) {
  auto actual = map.path({10, 28}, {8, 28});
  ASSERT_EQ(actual.get_path().size(), 3);
  ASSERT_NEAR(actual.get_length(), 2, 0.001);
}

TEST_F(StaticDoorSanity, OpenToOpen) {
  auto actual = map.path({8, 28}, {33, 49});
  ASSERT_EQ(actual.get_path().size(), 746);
  ASSERT_NEAR(actual.get_length(), 755.35516357421875, 0.001);
}

TEST_F(StaticDoorSanity, OpenToClosed) {
  ASSERT_FALSE(map.path({8, 28}, {70, 51}));
}

TEST_F(StaticDoorSanity, ClosedToOpen) {
  ASSERT_FALSE(map.path({70, 51}, {8, 28}));
}

TEST_F(StaticDoorSanity, ClosedToSelf) {
  ASSERT_FALSE(map.path({71, 18}, {71, 18}));
}

TEST_F(StaticDoorSanity, ClosedToSpace) {
  ASSERT_FALSE(map.path({71, 18}, {68, 18}));
}

TEST_F(StaticDoorSanity, SpaceToClosed) {
  ASSERT_FALSE(map.path({68, 18}, {71, 18}));
}

// Welcome to Doorland
class StaticDoorland : public StaticMapTest<test::MAP_DOORLAND> { };

TEST_F(StaticDoorland, LockedDoorWall) {
  Coord inside{2, 2};
  ASSERT_FALSE(map.path(inside, {1, 4}));
  ASSERT_FALSE(map.path(inside, {2, 4}));
  ASSERT_FALSE(map.path(inside, {3, 4}));
  ASSERT_FALSE(map.path(inside, {1, 3}));
  ASSERT_FALSE(map.path(inside, {2, 3}));
  ASSERT_FALSE(map.path(inside, {3, 3}));
}


TEST_F(StaticDoorland, OpenDoorWall) {
  Coord inside{2, 6};
  ASSERT_TRUE((bool)map.path(inside, {1, 8}));
  ASSERT_TRUE((bool)map.path(inside, {2, 8}));
  ASSERT_TRUE((bool)map.path(inside, {3, 8}));
  ASSERT_TRUE((bool)map.path(inside, {1, 7}));
  ASSERT_TRUE((bool)map.path(inside, {2, 7}));
  ASSERT_TRUE((bool)map.path(inside, {3, 7}));
}


TEST_F(StaticDoorland, PathThroughDoors) {
  ASSERT_TRUE((bool)map.path({1, 9}, {3, 12}));
  ASSERT_TRUE((bool)map.path({1, 12}, {6, 6}));
}
