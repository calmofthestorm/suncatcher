#ifndef RESOURCEMANAGER_13da63b0ba044998aa5f164e6907827b
#define RESOURCEMANAGER_13da63b0ba044998aa5f164e6907827b

#include <iostream>
#include <memory>

#include "platform.h"

#ifdef MICROPATHER_DELTA_TEST
  #include "MPWrapper.h"
#endif

#include "MapBuilder.h"

namespace suncatcher {

namespace pathfinder {
  class Map;
}  // namespace PATH_COST_INFINITE

namespace test {

class DeltaMap;

class ResourceManager {
  public:
    static const pathfinder::MapBuilder& get_builder(std::string fn);

    static const DeltaMap& get_map(std::string fn);

    #ifdef MICROPATHER_DELTA_TEST
      static MPWrapper get_micropather(std::string fn);
    #endif

  private:
    static ResourceManager& get_me();

    ResourceManager();
    std::map<const std::string, pathfinder::MapBuilder> builder_cache;
    std::map<const std::string, std::unique_ptr<DeltaMap>> map_cache;
};

}  // namespace test
}  // namespace suncatcher

#endif  /* RESOURCEMANAGER_13da63b0ba044998aa5f164e6907827b */
