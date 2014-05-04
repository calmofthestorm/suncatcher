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

// ****************************************************************************
// *** Single mutations, as simple as possible, affect the map.
// ****************************************************************************

// Door update
class SimpleDoor : public MapTest<MAP_MICRO> { };

TEST_F(SimpleDoor, ToggleOpen) {
  Coord cell = {6, 4};
  ASSERT_TRUE(map.is_door(cell));
  ASSERT_FALSE(map.get_doors().at(cell).open);
  ASSERT_EQ(map.get_data().at(cell), PATH_COST_INFINITE);
  map.mutate(std::move(map.get_mutator().toggle_door_open(cell)));
  ASSERT_TRUE(map.get_doors().at(cell).open);
  ASSERT_EQ(map.get_data().at(cell), map.get_doors().find(cell)->second.cost_open);
}


TEST_F(SimpleDoor, ToggleClosed) {
  Coord cell = {12, 8};
  ASSERT_TRUE(map.is_door(cell));
  ASSERT_TRUE(map.get_doors().at(cell).open);
  ASSERT_EQ(map.get_data().at(cell), map.get_doors().find(cell)->second.cost_open);
  map.mutate(std::move(map.get_mutator().toggle_door_open(cell)));
  ASSERT_FALSE(map.get_doors().at(cell).open);
  ASSERT_EQ(map.get_data().at(cell), PATH_COST_INFINITE);
}


TEST_F(SimpleDoor, SetOpenStateChange) {
  Coord cell = {6, 4};
  ASSERT_TRUE(map.is_door(cell));
  ASSERT_FALSE(map.get_doors().at(cell).open);
  ASSERT_EQ(map.get_data().at(cell), PATH_COST_INFINITE);
  map.mutate(std::move(map.get_mutator().set_door_open(cell, true)));
  ASSERT_TRUE(map.get_doors().at(cell).open);
}


TEST_F(SimpleDoor, SetClosedStateChange) {
  Coord cell = {12, 8};
  ASSERT_TRUE(map.is_door(cell));
  ASSERT_TRUE(map.get_doors().at(cell).open);
  ASSERT_EQ(map.get_data().at(cell), map.get_doors().find(cell)->second.cost_open);
  map.mutate(std::move(map.get_mutator().set_door_open(cell, false)));
  ASSERT_FALSE(map.get_doors().at(cell).open);
  ASSERT_EQ(map.get_data().at(cell), PATH_COST_INFINITE);
}


TEST_F(SimpleDoor, SetClosedNop) {
  Coord cell = {6, 4};
  ASSERT_TRUE(map.is_door(cell));
  ASSERT_FALSE(map.get_doors().at(cell).open);
  map.mutate(std::move(map.get_mutator().set_door_open(cell, false)));
  ASSERT_FALSE(map.get_doors().at(cell).open);
  ASSERT_EQ(map.get_data().at(cell), PATH_COST_INFINITE);
}

TEST_F(SimpleDoor, SetOpenNop) {
  Coord cell = {12, 8};
  ASSERT_TRUE(map.is_door(cell));
  ASSERT_TRUE(map.get_doors().at(cell).open);
  ASSERT_EQ(map.get_data().at(cell), map.get_doors().find(cell)->second.cost_open);
  map.mutate(std::move(map.get_mutator().set_door_open(cell, true)));
  ASSERT_TRUE(map.get_doors().at(cell).open);
  ASSERT_EQ(map.get_data().at(cell), map.get_doors().find(cell)->second.cost_open);
}


TEST_F(SimpleDoor, SetCost1) {
  Coord cell = {12, 8};
  ASSERT_TRUE(map.is_door(cell));
  map.mutate(std::move(map.get_mutator().set_door_open_cost(cell, 0)));
  ASSERT_TRUE(map.get_doors().at(cell).cost_open == 0);
  ASSERT_EQ(map.get_data().at(cell), map.get_doors().find(cell)->second.cost_open);
}


TEST_F(SimpleDoor, SetCost2) {
  Coord cell = {12, 8};
  ASSERT_TRUE(map.is_door(cell));
  map.mutate(std::move(map.get_mutator().set_door_open_cost(cell, 55)));
  ASSERT_TRUE(map.get_doors().at(cell).cost_open == 55);
  ASSERT_EQ(map.get_data().at(cell), map.get_doors().find(cell)->second.cost_open);
}


// Door creation/deletion.
class DoorCreateRemove : public MapTest<MAP_MICRO> { };


TEST_F(DoorCreateRemove, CreateOpenValidFree) {
  Coord cell = {12, 7};
  ASSERT_FALSE(map.is_door(cell));
  map.mutate(std::move(map.get_mutator().create_door(cell, true, 0)));
  ASSERT_TRUE(map.get_doors().at(cell).open);
  ASSERT_TRUE(map.get_doors().at(cell).cost_open == 0);
  ASSERT_EQ(map.get_data().at(cell), map.get_doors().find(cell)->second.cost_open);
}


TEST_F(DoorCreateRemove, CreateOpenValid) {
  Coord cell = {12, 7};
  ASSERT_FALSE(map.is_door(cell));
  map.mutate(std::move(map.get_mutator().create_door(cell, true, 5)));
  ASSERT_TRUE(map.get_doors().at(cell).open);
  ASSERT_TRUE(map.get_doors().at(cell).cost_open == 5);
  ASSERT_EQ(map.get_data().at(cell), map.get_doors().find(cell)->second.cost_open);
}


TEST_F(DoorCreateRemove, CreateClosedValid) {
  Coord cell = {12, 4};
  ASSERT_FALSE(map.is_door(cell));
  map.mutate(std::move(map.get_mutator().create_door(cell, false, 55)));
  ASSERT_FALSE(map.get_doors().at(cell).open);
  ASSERT_EQ(map.get_doors().at(cell).cost_open, 55);
  ASSERT_EQ(map.get_data().at(cell), PATH_COST_INFINITE);
}


TEST_F(DoorCreateRemove, WallToOpenValid) {
  Coord cell = {0, 0};
  ASSERT_FALSE(map.is_door(cell));
  ASSERT_EQ(map.get_data().at(cell), PATH_COST_INFINITE);
  map.mutate(std::move(map.get_mutator().create_door(cell, true, 5)));
  ASSERT_TRUE(map.get_doors().at(cell).open);
  ASSERT_TRUE(map.get_doors().at(cell).cost_open == 5);
  ASSERT_EQ(map.get_data().at(cell), map.get_doors().find(cell)->second.cost_open);
}


TEST_F(DoorCreateRemove, WallToClosedValid) {
  Coord cell = {0, 0};
  ASSERT_FALSE(map.is_door(cell));
  ASSERT_EQ(map.get_data().at(cell), PATH_COST_INFINITE);
  map.mutate(std::move(map.get_mutator().create_door(cell, false, 55)));
  ASSERT_FALSE(map.get_doors().at(cell).open);
  ASSERT_EQ(map.get_doors().at(cell).cost_open, 55);
  ASSERT_EQ(map.get_data().at(cell), PATH_COST_INFINITE);
}


TEST_F(DoorCreateRemove, RemoveOpenToWallValid) {
  Coord cell = {12, 8};
  ASSERT_TRUE(map.is_door(cell));
  ASSERT_EQ(map.get_data().at(cell), map.get_doors().find(cell)->second.cost_open);
  ASSERT_TRUE(map.get_doors().at(cell).open);
  map.mutate(std::move(map.get_mutator().remove_door(cell, PATH_COST_INFINITE)));
  ASSERT_FALSE(map.is_door(cell));
  ASSERT_EQ(map.get_data().at(cell), PATH_COST_INFINITE);
}


TEST_F(DoorCreateRemove, RemoveOpenToFreeSquare) {
  Coord cell = {12, 8};
  ASSERT_TRUE(map.is_door(cell));
  ASSERT_EQ(map.get_data().at(cell), map.get_doors().find(cell)->second.cost_open);
  ASSERT_TRUE(map.get_doors().at(cell).open);
  map.mutate(std::move(map.get_mutator().remove_door(cell, 0)));
  ASSERT_FALSE(map.is_door(cell));
  ASSERT_EQ(map.get_data().at(cell), 0);
}


TEST_F(DoorCreateRemove, RemoveOpenToNormalPassable) {
  Coord cell = {12, 8};
  ASSERT_TRUE(map.is_door(cell));
  ASSERT_EQ(map.get_data().at(cell), map.get_doors().find(cell)->second.cost_open);
  ASSERT_TRUE(map.get_doors().at(cell).open);
  map.mutate(std::move(map.get_mutator().remove_door(cell, 1)));
  ASSERT_FALSE(map.is_door(cell));
  ASSERT_EQ(map.get_data().at(cell), 1);
}


TEST_F(DoorCreateRemove, RemoveClosedToWallValid) {
  Coord cell = {6, 4};
  ASSERT_TRUE(map.is_door(cell));
  ASSERT_FALSE(map.get_doors().at(cell).open);
  map.mutate(std::move(map.get_mutator().remove_door(cell, PATH_COST_INFINITE)));
  ASSERT_FALSE(map.is_door(cell));
  ASSERT_EQ(map.get_data().at(cell), PATH_COST_INFINITE);
}


TEST_F(DoorCreateRemove, RemoveClosedToFreeSquare) {
  Coord cell = {6, 4};
  ASSERT_TRUE(map.is_door(cell));
  ASSERT_FALSE(map.get_doors().at(cell).open);
  map.mutate(std::move(map.get_mutator().remove_door(cell, 0)));
  ASSERT_FALSE(map.is_door(cell));
  ASSERT_EQ(map.get_data().at(cell), 0);
}


TEST_F(DoorCreateRemove, RemoveClosedToNormalPassable) {
  Coord cell = {6, 4};
  ASSERT_TRUE(map.is_door(cell));
  ASSERT_FALSE(map.get_doors().at(cell).open);
  map.mutate(std::move(map.get_mutator().remove_door(cell, 1)));
  ASSERT_FALSE(map.is_door(cell));
  ASSERT_EQ(map.get_data().at(cell), 1);
}


// Setting terrain cost.
class TerrainCost : public MapTest<MAP_MICRO> { };


TEST_F(TerrainCost, WallToWall) {
  Coord cell = {0, 0};
  ASSERT_EQ(map.get_data().at(cell), PATH_COST_INFINITE);
  map.mutate(std::move(map.get_mutator().set_cost(cell, PATH_COST_INFINITE)));
  ASSERT_EQ(map.get_data().at(cell), PATH_COST_INFINITE);
}


TEST_F(TerrainCost, WallToFree) {
  Coord cell = {0, 0};
  ASSERT_EQ(map.get_data().at(cell), PATH_COST_INFINITE);
  map.mutate(std::move(map.get_mutator().set_cost(cell, 0)));
  ASSERT_EQ(map.get_data().at(cell), 0);
}


TEST_F(TerrainCost, WallToNormal) {
  Coord cell = {0, 0};
  ASSERT_EQ(map.get_data().at(cell), PATH_COST_INFINITE);
  map.mutate(std::move(map.get_mutator().set_cost(cell, 1)));
  ASSERT_EQ(map.get_data().at(cell), 1);
}


TEST_F(TerrainCost, NormalCostToWall) {
  Coord cell = {1, 1};
  ASSERT_EQ(map.get_data().at(cell), 1);
  map.mutate(std::move(map.get_mutator().set_cost(cell, PATH_COST_INFINITE)));
  ASSERT_EQ(map.get_data().at(cell), PATH_COST_INFINITE);
}


TEST_F(TerrainCost, NormalCostToFree) {
  Coord cell = {1, 1};
  ASSERT_EQ(map.get_data().at(cell), 1);
  map.mutate(std::move(map.get_mutator().set_cost(cell, 0)));
  ASSERT_EQ(map.get_data().at(cell), 0);
}


TEST_F(TerrainCost, NormalCostTo4X) {
  Coord cell = {1, 1};
  ASSERT_EQ(map.get_data().at(cell), 1);
  map.mutate(std::move(map.get_mutator().set_cost(cell, 4)));
  ASSERT_EQ(map.get_data().at(cell), 4);
}


// Misc simple
class Misc : public MapTest<MAP_MICRO> { };


TEST_F(Misc, DoorToDoorTesseractService) {
  Coord door1{14, 7};
  Coord door2{0, 0};
  Coord old{14, 2};
  map.mutate(std::move(map.get_mutator().create_door(door1, true, 1)));
  map.mutate(std::move(map.get_mutator().create_door(door2, true, 1)));
  ASSERT_FALSE(map.path_exists(old, door2));
}


// Non-uniform cost pathfinding
class NonUniformCost : public MapTest<MAP_MICRO> { };


// Create an expensive "ridge" so it paths around
TEST_F(NonUniformCost, HillGoAround) {
  Coord src{5, 7};
  Coord dst{7, 7};
  auto actual = map.path(src, dst);
  ASSERT_EQ(2, actual.get_length());
  ASSERT_EQ(3, actual.get_path().size());
  #ifdef MICROPATHER_DELTA_TEST
  if (enable_micropather) {
    auto expected = get_micropather().path(src, dst);
    ASSERT_EQ(expected, actual);
  }
  #endif
  for (uint16_t i = 6; i <= 8; ++i) {
    map.mutate(std::move(map.get_mutator().set_cost({6, i}, 200)));
  }
  actual = map.path(src, dst);
  ASSERT_NEAR(13.6569, actual.get_length(), 0.1);
  ASSERT_EQ(13, actual.get_path().size());
  #ifdef MICROPATHER_DELTA_TEST
  if (enable_micropather) {
    auto expected = get_micropather().path(src, dst);
    ASSERT_EQ(expected, actual);
  }
  #endif
}


// Create an expensive "ridge" but path into it explicitely.
TEST_F(NonUniformCost, DeliciousHillYouMustClimbIt) {
  Coord src{5, 7};
  Coord dst{6, 7};
  auto actual = map.path(src, dst);
  ASSERT_EQ(1, actual.get_length());
  ASSERT_EQ(2, actual.get_path().size());
  #ifdef MICROPATHER_DELTA_TEST
  if (enable_micropather) {
    auto expected = get_micropather().path(src, dst);
    ASSERT_EQ(expected, actual);
  }
  #endif
  for (uint16_t i = 6; i <= 8; ++i) {
    map.mutate(std::move(map.get_mutator().set_cost({6, i}, 200)));
  }
  actual = map.path(src, dst);
  ASSERT_NEAR(200, actual.get_length(), 0.1);
  ASSERT_EQ(2, actual.get_path().size());
  #ifdef MICROPATHER_DELTA_TEST
  if (enable_micropather) {
    auto expected = get_micropather().path(src, dst);
    ASSERT_EQ(expected, actual);
  }
  #endif
}


// Create an expensive "ridge", that still wins vs going around.
TEST_F(NonUniformCost, HillStillShortestPath) {
  Coord src{5, 7};
  Coord dst{7, 7};
  auto actual = map.path(src, dst);
  ASSERT_EQ(2, actual.get_length());
  ASSERT_EQ(3, actual.get_path().size());
  #ifdef MICROPATHER_DELTA_TEST
  if (enable_micropather) {
    auto expected = get_micropather().path(src, dst);
    ASSERT_EQ(expected, actual);
  }
  #endif
  for (uint16_t i = 6; i <= 8; ++i) {
    map.mutate(std::move(map.get_mutator().set_cost({6, i}, 3)));
  }
  actual = map.path(src, dst);
  ASSERT_NEAR(4, actual.get_length(), 0.1);
  ASSERT_EQ(3, actual.get_path().size());
  #ifdef MICROPATHER_DELTA_TEST
  if (enable_micropather) {
    auto expected = get_micropather().path(src, dst);
    ASSERT_EQ(expected, actual);
  }
  #endif
}


// Create an expensive door and a cheap door. The expensive one is closer.
TEST_F(NonUniformCost, NonUniformExpensiveVsCheapDoor) {
  map.mutate(std::move(map.get_mutator().toggle_door_open({6, 4})));
  map.mutate(std::move(map.get_mutator().set_door_open_cost({6, 4}, 25)));
  Coord src{5, 4};
  Coord dst{7, 4};
  auto actual = map.path(src, dst);
  ASSERT_NEAR(8.243, actual.get_length(), 0.1);
  ASSERT_EQ(8, actual.get_path().size());
  #ifdef MICROPATHER_DELTA_TEST
  if (enable_micropather) {
    auto expected = get_micropather().path(src, dst);
    ASSERT_NEAR(expected.get_length(), actual.get_length(), 0.1);
    ASSERT_EQ(expected.get_path().size(), actual.get_path().size());
  }
  #endif
}


// Creating/removing walls blocks/unblocks movement.
class SimpleAddRemoveWallUpdatesComponents : public MapTest<MAP_MICRO> { };

TEST_F(SimpleAddRemoveWallUpdatesComponents, CreatingWallBlocksMovementToFromIt) {
  Coord wall{11, 6};
  Coord open{11, 7};
  ASSERT_TRUE((bool)map.path_exists(wall, open));
  ASSERT_TRUE((bool)map.path(wall, open));
  ASSERT_TRUE((bool)map.path(open, wall));
  ASSERT_TRUE((bool)map.path_exists(open, wall));
  map.mutate(std::move(map.get_mutator().set_cost(wall, PATH_COST_INFINITE)));
  ASSERT_FALSE((bool)map.path(wall, open));
  ASSERT_FALSE((bool)map.path_exists(wall, open));
  ASSERT_FALSE((bool)map.path(open, wall));
  ASSERT_FALSE((bool)map.path_exists(open, wall));
}


TEST_F(SimpleAddRemoveWallUpdatesComponents, CreatingWallBlocksMovementThroughIt) {
  Coord wall{9, 7};
  Coord src{8, 7};
  Coord dst{10, 7};
  ASSERT_NEAR(map.path(src, dst).get_length(), 2, 0.1);
  map.mutate(std::move(map.get_mutator().set_cost(wall, PATH_COST_INFINITE)));
  ASSERT_NEAR(map.path(src, dst).get_length(), 2.828, 0.1);
  map.mutate(std::move(map.get_mutator().set_cost(wall, 0)));
  ASSERT_NEAR(map.path(src, dst).get_length(), 1, 0.1);
}


TEST_F(SimpleAddRemoveWallUpdatesComponents, CreatingWallsDisconnectsComponents) {
  Coord wall{11, 5};
  Coord src{11, 4};
  Coord dst{11, 6};
  ASSERT_NEAR(map.path(src, dst).get_length(), 2, 0.1);
  map.mutate(std::move(map.get_mutator().set_cost(wall, PATH_COST_INFINITE)));
}


TEST_F(SimpleAddRemoveWallUpdatesComponents, DiagonalWallDisconnectReconnect) {
  Coord wall{11, 6};
  Coord src{11, 5};
  Coord dst{11, 7};
  Coord origin{1, 1};
  ASSERT_NEAR(map.path(src, dst).get_length(), 2, 0.1);
  ASSERT_NEAR(map.path(origin, src).get_length(), 13.657, 0.1);
  map.mutate(std::move(map.get_mutator().set_cost(wall, PATH_COST_INFINITE)));
  ASSERT_FALSE((bool)map.path(src, dst));
  ASSERT_FALSE((bool)map.path(origin, src));
  map.mutate(std::move(map.get_mutator().set_cost(wall, 2)));
  ASSERT_NEAR(map.path(src, dst).get_length(), 2.828, 0.1);
  ASSERT_NEAR(map.path(origin, dst).get_length(), 13.657, 0.1);
}


// Dynamic pathfinding in changing door states.
class DoorPathfinding : public MapTest<MAP_MICRO> { };

TEST_F(DoorPathfinding, OpenDoorAndGoStandInDoorframe) {
  Coord cell{6, 4};
  map.mutate(std::move(map.get_mutator().toggle_door_open(cell)));
  ASSERT_TRUE(map.get_doors().at(cell).open);
  ASSERT_TRUE(map.path_exists({7, 4}, cell));
  ASSERT_TRUE(map.path_exists({5, 4}, cell));
}


TEST_F(DoorPathfinding, Airlock) {
  Coord outside{5, 4};
  Coord airlock{7, 4};
  Coord inside{9, 4};
  Coord outer_door{6, 4};
  Coord inner_door{8, 4};

  // Add a wall to make it an airlock.
  auto mutator = map.get_mutator();
  for (uint16_t i = 0; i <= 9; ++i) {
    if (i != 4) {
      mutator.set_cost({6, i}, PATH_COST_INFINITE);
    }
  }
  map.mutate(std::move(mutator));

  // Attempt to path directly outside, which won't work.
  ASSERT_FALSE(map.path(inside, outside));

  // Verify we can/can't path into the doors.
  ASSERT_FALSE(map.path(inside, outer_door));
  ASSERT_TRUE((bool)map.path(inside, inner_door));
  ASSERT_TRUE((bool)map.path(inside, airlock));

  // Path into the airlock.
  auto into_airlock = map.path(inside, airlock);
  ASSERT_NEAR(2, into_airlock.get_length(), 0.1);
  ASSERT_EQ(3, into_airlock.get_path().size());

  // Cycle the doors atomically.
  mutator = map.get_mutator();
  mutator
    .toggle_door_open(outer_door)
    .toggle_door_open(inner_door);
  map.mutate(std::move(mutator));

  // Attempt to path inside and fail.
  ASSERT_FALSE(map.path(airlock, inside));

  // Verify we can/can't path into the doors.
  ASSERT_FALSE(map.path(outside, inner_door));
  ASSERT_TRUE((bool)map.path(outside, outer_door));
  ASSERT_FALSE(map.path(inside, inner_door));
  ASSERT_FALSE(map.path(inside, outer_door));
  ASSERT_FALSE(map.path(inside, airlock));
  ASSERT_TRUE((bool)map.path(airlock, outer_door));

  // Exit the airlock.
  auto out_of_airlock = map.path(airlock, outside);
  ASSERT_NEAR(2, into_airlock.get_length(), 0.1);
  ASSERT_EQ(3, into_airlock.get_path().size());
}

TEST_F(DoorPathfinding, OpenDoorAndGoThroughAsShortcut) {
  Coord src{7, 4};
  Coord dst{5, 4};
  auto actual = map.path(src, dst);
  ASSERT_NEAR(8.243, actual.get_length(), 0.1);
  ASSERT_EQ(8, actual.get_path().size());
  map.mutate(std::move(map.get_mutator().toggle_door_open({6, 4})));
  actual = map.path(src, dst);
  ASSERT_NEAR(2, actual.get_length(), 0.1);
  ASSERT_EQ(3, actual.get_path().size());
}


TEST_F(DoorPathfinding, DoorToDoorTogglingFun) {
  Coord outer_door{6, 4};
  Coord inner_door{8, 4};

  for (size_t i = 0; i < 4; ++i) {
    ASSERT_FALSE(map.path(outer_door, inner_door));
    ASSERT_FALSE(map.path(inner_door, outer_door));
    map.mutate(std::move(map.get_mutator().toggle_door_open(inner_door)));
    ASSERT_FALSE(map.path(outer_door, inner_door));
    ASSERT_FALSE(map.path(inner_door, outer_door));
    map.mutate(std::move(map.get_mutator().toggle_door_open(outer_door)));
    ASSERT_FALSE(map.path(outer_door, inner_door));
    ASSERT_FALSE(map.path(inner_door, outer_door));
    map.mutate(std::move(map.get_mutator().toggle_door_open(inner_door)));
    ASSERT_TRUE((bool)map.path(outer_door, inner_door));
    ASSERT_TRUE((bool)map.path(inner_door, outer_door));
    map.mutate(std::move(map.get_mutator().toggle_door_open(outer_door)));
    ASSERT_FALSE(map.path(outer_door, inner_door));
    ASSERT_FALSE(map.path(inner_door, outer_door));
  }
}


TEST_F(DoorPathfinding, DoorDisconnectsDiagonalComponents) {
  Coord door{11, 6};
  Coord outer{11, 7};
  Coord inner{11, 5};
  ASSERT_TRUE((bool)map.path(outer, inner));
  map.mutate(std::move(map.get_mutator().create_door(door, true, 1)));
  ASSERT_TRUE((bool)map.path(inner, outer));
  map.mutate(std::move(map.get_mutator().toggle_door_open(door)));
  ASSERT_FALSE((bool)map.path(inner, outer));
}


TEST_F(DoorPathfinding, MultipleDoorsDisconnectingComponents) {
  std::array<Coord, 3> doors{Coord{10, 6}, Coord{11, 6}, Coord{12, 6}};
  Coord outer{9, 7};
  Coord inner{11, 5};

  // Create doors
  auto mutator = map.get_mutator();
  for (const Coord& door : doors) {
    mutator.create_door(door, true, 1);
  }
  map.mutate(std::move(mutator));

  // 111
  ASSERT_TRUE((bool)map.path(outer, inner));

  map.mutate(std::move(map.get_mutator().toggle_door_open(doors[0])));
  // 110
  ASSERT_TRUE((bool)map.path(outer, inner));

  map.mutate(std::move(map.get_mutator().toggle_door_open(doors[1])));
  map.mutate(std::move(map.get_mutator().toggle_door_open(doors[0])));
  // 101
  ASSERT_FALSE((bool)map.path(outer, inner));

  map.mutate(std::move(map.get_mutator().toggle_door_open(doors[0])));
  // 100
  ASSERT_FALSE((bool)map.path(outer, inner));

  mutator = map.get_mutator();
  for (const Coord& door : doors) {
    mutator.toggle_door_open(door);
  }
  map.mutate(std::move(mutator));
  // 011
  ASSERT_TRUE((bool)map.path(outer, inner));

  map.mutate(std::move(map.get_mutator().toggle_door_open(doors[0])));
  // 010
  ASSERT_TRUE((bool)map.path(outer, inner));

  map.mutate(std::move(map.get_mutator().toggle_door_open(doors[1])));
  map.mutate(std::move(map.get_mutator().toggle_door_open(doors[0])));
  // 001
  ASSERT_FALSE((bool)map.path(outer, inner));

  map.mutate(std::move(map.get_mutator().toggle_door_open(doors[0])));
  // 000
  ASSERT_FALSE((bool)map.path(outer, inner));
}

// Pathfinding around dynamic doors on a big map.
class DoorPathfindingBigMap : public MapTest<MAP_MAIN> { };

TEST_F(DoorPathfindingBigMap, AdjacentDoorsInCorridor) {
  Coord upper{68, 71};
  Coord top_door{69, 71};
  Coord bot_door{70, 71};
  Coord lower{71, 71};
  assert(map.is_door(top_door));
  assert(map.is_door(bot_door));

  ASSERT_EQ(map.path(top_door, bot_door).get_length(), 1);
  ASSERT_EQ(map.path(upper, lower).get_length(), 3);

  map.mutate(std::move(map.get_mutator().toggle_door_open(top_door)));
  ASSERT_FALSE(map.path(top_door, bot_door));
  ASSERT_NEAR(map.path(upper, lower).get_length(), 88.657, 0.5);
  map.mutate(std::move(map.get_mutator().toggle_door_open(top_door)));
  ASSERT_EQ(map.path(upper, lower).get_length(), 3);
  map.mutate(std::move(map.get_mutator().toggle_door_open(bot_door)));
  ASSERT_NEAR(map.path(upper, lower).get_length(), 88.657, 0.5);
}


TEST_F(DoorPathfindingBigMap, OneDoorFiveColors) {
  Coord door{68, 64};
  std::array<Coord, 4> cells{Coord{67, 64}, Coord{69, 64}, Coord{68, 63}, Coord{68, 65}};

  for (auto it = cells.begin(); it != cells.end(); ++it) {
    ASSERT_TRUE((bool)map.path(door, *it));
    ASSERT_TRUE((bool)map.path(*it, door));
    for (auto jt = it + 1; jt != cells.end(); ++jt) {
      ASSERT_TRUE((bool)map.path(*it, *jt));
      ASSERT_TRUE((bool)map.path(*jt, *it));
    }
  }

  map.mutate(std::move(map.get_mutator().toggle_door_open(door)));

  for (auto it = cells.begin(); it != cells.end(); ++it) {
    for (auto jt = it + 1; jt != cells.end(); ++jt) {
      ASSERT_FALSE(map.path(*it, *jt));
      ASSERT_FALSE(map.path(*jt, *it));
    }
  }

  map.mutate(std::move(map.get_mutator().toggle_door_open(door)));

  for (auto it = cells.begin(); it != cells.end(); ++it) {
    ASSERT_TRUE((bool)map.path(door, *it));
    ASSERT_TRUE((bool)map.path(*it, door));
    for (auto jt = it + 1; jt != cells.end(); ++jt) {
      ASSERT_TRUE((bool)map.path(*it, *jt));
      ASSERT_TRUE((bool)map.path(*jt, *it));
    }
  }
}

// Doorland revisited -- dynamic
class Doorland : public MapTest<MAP_DOORLAND> { };

TEST_F(Doorland, DoorTunnelExpensiveSides) {
  auto easy = map.path({0, 10}, {5, 3});
  ASSERT_NEAR(easy.get_length(), 9.071, 0.1);
  ASSERT_EQ(8, easy.get_path().size());
  for (uint16_t i = 0; i < 5; ++i) {
    map.mutate(std::move(map.get_mutator().set_door_open_cost({i, 9}, 100)));
    map.mutate(std::move(map.get_mutator().set_door_open_cost({i, 11}, 100)));
  }
  auto hard = map.path({0, 10}, {5, 3});
  ASSERT_NEAR(hard.get_length(), 12.243, 0.1);
  ASSERT_EQ(12, hard.get_path().size());
}


TEST_F(Doorland, DoorTunnelClosedSides) {
  auto easy = map.path({0, 10}, {5, 3});
  ASSERT_NEAR(easy.get_length(), 9.071, 0.1);
  ASSERT_EQ(8, easy.get_path().size());
  map.mutate(std::move(map.get_mutator().set_door_open_cost({4, 10}, 100)));
  for (uint16_t i = 0; i < 5; ++i) {
    map.mutate(std::move(map.get_mutator().toggle_door_open({i, 9})));
    map.mutate(std::move(map.get_mutator().toggle_door_open({i, 11})));
  }
  auto hard = map.path({0, 10}, {5, 3});
  ASSERT_NEAR(hard.get_length(), 111.243, 0.1);
  ASSERT_EQ(12, hard.get_path().size());
}


TEST_F(Doorland, DoorlandItsATrap) {
  Coord start{3, 11};
  Coord finish{7, 12};
  auto escape = map.path(start, finish);
  ASSERT_NEAR(escape.get_length(), 4.414, 0.1);
  ASSERT_EQ(5, escape.get_path().size());

  // Spring all but last "wall"
  map.mutate(std::move(map.get_mutator().toggle_door_open({2, 10})));
  map.mutate(std::move(map.get_mutator().toggle_door_open({2, 11})));
  map.mutate(std::move(map.get_mutator().toggle_door_open({2, 12})));
  map.mutate(std::move(map.get_mutator().toggle_door_open({3, 12})));
  map.mutate(std::move(map.get_mutator().toggle_door_open({4, 10})));
  map.mutate(std::move(map.get_mutator().toggle_door_open({4, 11})));
  map.mutate(std::move(map.get_mutator().toggle_door_open({4, 12})));

  // Can still escape but route is longer!
  escape = map.path(start, finish);
  ASSERT_NEAR(escape.get_length(), 6.657, 0.1);
  ASSERT_EQ(6, escape.get_path().size());

  // Last one
  map.mutate(std::move(map.get_mutator().toggle_door_open({3, 10})));

  // Too late!
  ASSERT_FALSE(map.path(start, finish));
}


