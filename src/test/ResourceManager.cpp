#include "suncatcher/test/ResourceManager.h"

#include <fstream>

#include "suncatcher/test/DeltaMap.h"

namespace suncatcher {
namespace test {

const pathfinder::MapBuilder& ResourceManager::get_builder(std::string fn) {
  if (get_me().builder_cache.find(fn) == get_me().builder_cache.end()) {
    std::ifstream is(std::string("maps/" + fn + ".txt"));
    if (!is) {
      std::cerr << "Unable to find maps file. Run tests from project root." << std::endl;
      exit(-1);
    }
    assert(is);
    get_me().builder_cache[fn] = pathfinder::MapBuilder(is);
  }
  return get_me().builder_cache[fn];
}

const DeltaMap& ResourceManager::get_map(std::string fn) {
  if (get_me().map_cache.find(fn) == get_me().map_cache.end()) {
    get_me().map_cache[fn].reset(new DeltaMap(pathfinder::MapBuilder(get_builder(fn))));
  }
  return *get_me().map_cache[fn];
}

#ifdef MICROPATHER_DELTA_TEST
  MPWrapper ResourceManager::get_micropather(std::string fn) {
    return MPWrapper(&get_map(fn));
  }
#endif

ResourceManager& ResourceManager::get_me() {
  static ResourceManager self;
  return self;
}

ResourceManager::ResourceManager() { }

}  // namespace test
}  // namespace suncatcher
