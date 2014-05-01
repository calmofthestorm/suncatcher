#include "test_pathfinder_common.h"

// Really simple tests for basic functionality.

TEST_F(MapTest, OpenMapOOB1) {
  ASSERT_FALSE(MapTest::empty_map->path({0, 0}, {100, 100}));
}

TEST_F(MapTest, OpenMapOOB2) {
  ASSERT_FALSE(MapTest::empty_map->path({0, 0}, {200, 200}));
}

TEST_F(MapTest, OpenMapIdentityOOB) {
  ASSERT_FALSE(MapTest::empty_map->path({100, 100}, {100, 100}));
}

TEST_F(MapTest, OpenMapIdentityPath) {
  ASSERT_EQ(
      Path({{0, 0}}, 0),
      MapTest::empty_map->path({0, 0}, {0, 0})
    );
}

TEST_F(MapTest, OpenMapHorizPath) {
  ASSERT_EQ(
      Path({{1, 5}, {1, 4}, {1, 3}, {1, 2}, {1, 1}}, 4),
      MapTest::empty_map->path({1, 5}, {1, 1})
    );
}

TEST_F(MapTest, OpenMapVertPath) {
  ASSERT_EQ(
      Path({{1, 1}, {2, 1}, {3, 1}}, 2),
      MapTest::empty_map->path({1, 1}, {3, 1})
    );
}

TEST_F(MapTest, OpenMapDiagPath) {
  ASSERT_EQ(
      Path({{1, 1}, {2, 2}, {3, 3}}, (float)2.8284271247461903),
      MapTest::empty_map->path({1, 1}, {3, 3})
    );
}

TEST_F(MapTest, OpenMapKnightPath1) {
  // Two equally good choices for this one.
  auto expected = Path({{1, 1}, {1, 2}, {3, 2}}, (float)2.414213562373095);
  auto actual = MapTest::empty_map->path({1, 1}, {3, 2});
  ASSERT_NEAR(expected.get_length(), actual.get_length(), 0.001);
  ASSERT_EQ(expected.get_path().size(), actual.get_path().size());
}

TEST_F(MapTest, OpenMapKnightPath2) {
  // Two equally good choices for this one.
  auto expected = Path({{5, 4}, {4, 4}, {3, 3}}, (float)2.414213562373095);
  auto actual = MapTest::empty_map->path({5, 4}, {3, 3});
  ASSERT_NEAR(expected.get_length(), actual.get_length(), 0.001);
  ASSERT_EQ(expected.get_path().size(), actual.get_path().size());
}

TEST_F(MapTest, OpenMapLongPath) {
  Coord start{23, 92};
  Coord finish{97, 0};
  auto actual = MapTest::empty_map->path(start, finish);
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

TEST_F(MapTest, WallInvalidToSelf) {
  ASSERT_FALSE(MapTest::main_map->path({0, 0}, {0, 0}));
}

TEST_F(MapTest, WallInvalidToAdj) {
  ASSERT_FALSE(MapTest::main_map->path({0, 2}, {0, 1}));
}

TEST_F(MapTest, WallInvalidToOtherWall) {
  ASSERT_FALSE(MapTest::main_map->path({11, 0}, {55, 39}));
}

TEST_F(MapTest, WallInvalidToOOB1) {
  ASSERT_FALSE(MapTest::main_map->path({50, 0}, {85, 100}));
}

TEST_F(MapTest, WallInvalidToOOB2) {
  ASSERT_FALSE(MapTest::main_map->path({0, 100}, {55, 0}));
}

// More interesting terrain-based tests.

// Upper left to just outside great hall.
TEST_F(MapTest, ULToJustOutsideGreatHall) {
  auto actual = MapTest::main_map->path({1, 1}, {67, 51});
  ASSERT_NEAR(908.366, actual.get_length(), 0.5);
  ASSERT_EQ(887, actual.get_path().size());
}

// Upper right into great hall (door locked).
TEST_F(MapTest, ULToLockedGreatHall) {
  ASSERT_FALSE(MapTest::main_map->path({1, 98}, {69, 51}));
}

// Upper right into left tower
TEST_F(MapTest, ULToStrongholdLeftTower) {
  auto actual = MapTest::main_map->path({1, 98}, {69, 43});
  ASSERT_NEAR(991.994, actual.get_length(), 0.5);
  ASSERT_EQ(964, actual.get_path().size());
}

// Isolated component
TEST_F(MapTest, IsolatedInternal) {
  auto actual = MapTest::main_map->path({68, 63}, {68, 65});
  ASSERT_EQ(Path({{68, 63}, {68, 64}, {68, 65}}, 2), actual);
}

// Isolated component outside
TEST_F(MapTest, IsolatedInternalNoPath) {
  ASSERT_FALSE(MapTest::main_map->path({68, 63}, {1, 1}));
}

TEST_F(MapTest, DiagonalWallHug) {
  ASSERT_EQ(
      Path({{4, 51}, {5, 52}, {6, 53}, {7, 54}, {8, 55}, {9, 56}, {10, 57},
            {11, 58}, {12, 59}, {11, 60}, {10, 59}}, (float)14.142135623730951),
      MapTest::main_map->path({4, 51}, {10, 59})
    );
}

TEST_F(MapTest, CornerToCornerLRUL) {
  auto actual = MapTest::main_map->path({83, 98}, {1, 1});
  ASSERT_NEAR(939.064, actual.get_length(), 0.5);
  ASSERT_EQ(909, actual.get_path().size());
}

TEST_F(MapTest, CornerToCornerURLL) {
  auto actual = MapTest::main_map->path({1, 97}, {83, 1});
  ASSERT_NEAR(1081.41, actual.get_length(), 0.5);
  ASSERT_EQ(1051, actual.get_path().size());
}

TEST_F(MapTest, NoBlockedDiagonalMoves) {
  auto actual = MapTest::main_map->path({6, 55}, {7, 54});
  ASSERT_EQ(actual.get_path().size(), 10);
  ASSERT_NEAR(actual.get_length(), 12.7279, 0.5);
}

TEST_F(MapTest, DiagonalWallHugNoBlockedDiagonalMove) {
  auto actual = MapTest::main_map->path({3, 67}, {10, 74});
  ASSERT_EQ(actual.get_path().size(), 10);
  ASSERT_NEAR(actual.get_length(), 12.7279, 0.5);
}

TEST_F(MapTest, DiagonalTube) {
  auto actual = MapTest::main_map->path({5, 79}, {9, 80});
  ASSERT_EQ(actual.get_path().size(), 11);
  ASSERT_NEAR(actual.get_length(), 13.728, 0.5);
}

// Static door tests (open ones are pathable, closed are not.)

TEST_F(MapTest, OpenDoorToSelf) {
  auto actual = MapTest::main_map->path({8, 28}, {8, 28});
  ASSERT_EQ(actual.get_path().size(), 1);
  ASSERT_NEAR(actual.get_length(), 0, 0.001);
}

TEST_F(MapTest, OpenDoorToSpace) {
  auto actual = MapTest::main_map->path({8, 28}, {10, 28});
  ASSERT_EQ(actual.get_path().size(), 3);
  ASSERT_NEAR(actual.get_length(), 2, 0.001);
}

TEST_F(MapTest, SpaceToOpenDoor) {
  auto actual = MapTest::main_map->path({10, 28}, {8, 28});
  ASSERT_EQ(actual.get_path().size(), 3);
  ASSERT_NEAR(actual.get_length(), 2, 0.001);
}

TEST_F(MapTest, OpenDoorToOpenDoor) {
  auto actual = MapTest::main_map->path({8, 28}, {33, 49});
  ASSERT_EQ(actual.get_path().size(), 746);
  ASSERT_NEAR(actual.get_length(), 755.35516357421875, 0.001);
}

TEST_F(MapTest, OpenDoorToClosedDoor) {
  ASSERT_FALSE(MapTest::main_map->path({8, 28}, {70, 51}));
}

TEST_F(MapTest, ClosedDoorToOpenDoor) {
  ASSERT_FALSE(MapTest::main_map->path({70, 51}, {8, 28}));
}

TEST_F(MapTest, ClosedDoorToSelf) {
  ASSERT_FALSE(MapTest::main_map->path({71, 18}, {71, 18}));
}

TEST_F(MapTest, ClosedDoorToSpace) {
  ASSERT_FALSE(MapTest::main_map->path({71, 18}, {68, 18}));
}

TEST_F(MapTest, SpaceToClosedDoor) {
  ASSERT_FALSE(MapTest::main_map->path({68, 18}, {71, 18}));
}

// Welcome to Doorland


TEST_F(MapTest, DoorlandLockedDoorWall) {
  Coord inside{2, 2};
  ASSERT_FALSE(doorland_map->path(inside, {1, 4}));
  ASSERT_FALSE(doorland_map->path(inside, {2, 4}));
  ASSERT_FALSE(doorland_map->path(inside, {3, 4}));
  ASSERT_FALSE(doorland_map->path(inside, {1, 3}));
  ASSERT_FALSE(doorland_map->path(inside, {2, 3}));
  ASSERT_FALSE(doorland_map->path(inside, {3, 3}));
}


TEST_F(MapTest, DoorlandOpenDoorWall) {
  Coord inside{2, 6};
  ASSERT_TRUE((bool)doorland_map->path(inside, {1, 8}));
  ASSERT_TRUE((bool)doorland_map->path(inside, {2, 8}));
  ASSERT_TRUE((bool)doorland_map->path(inside, {3, 8}));
  ASSERT_TRUE((bool)doorland_map->path(inside, {1, 7}));
  ASSERT_TRUE((bool)doorland_map->path(inside, {2, 7}));
  ASSERT_TRUE((bool)doorland_map->path(inside, {3, 7}));
}


TEST_F(MapTest, DoorlandPathThroughDoors) {
  ASSERT_TRUE((bool)doorland_map->path({1, 9}, {3, 12}));
  ASSERT_TRUE((bool)doorland_map->path({1, 12}, {6, 6}));
}
