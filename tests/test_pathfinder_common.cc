#include "test_pathfinder_common.h"

std::unique_ptr<const MapBuilder> MapTest::micro_map_builder = nullptr;
std::unique_ptr<const MapBuilder> MapTest::empty_map_builder = nullptr;
std::unique_ptr<const MapBuilder> MapTest::main_map_builder = nullptr;
std::unique_ptr<const MapBuilder> MapTest::doorland_map_builder = nullptr;
std::unique_ptr<const DeltaMap> MapTest::micro_map = nullptr;
std::unique_ptr<const DeltaMap> MapTest::empty_map = nullptr;
std::unique_ptr<const DeltaMap> MapTest::main_map = nullptr;
std::unique_ptr<const DeltaMap> MapTest::doorland_map = nullptr;
#ifdef MICROPATHER_DELTA_TEST
  std::unique_ptr<MPWrapper> MapTest::micro_mp = nullptr;
  std::unique_ptr<MPWrapper> MapTest::empty_mp = nullptr;
  std::unique_ptr<MPWrapper> MapTest::main_mp = nullptr;
  std::unique_ptr<MPWrapper> MapTest::doorland_mp = nullptr;
#endif
