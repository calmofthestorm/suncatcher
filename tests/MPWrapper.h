#ifndef MPWRAPPER_616e1e92191d445dac0fe08e40c6429b
#define MPWRAPPER_616e1e92191d445dac0fe08e40c6429b

#include "Path.h"
#include "Coord.h"

namespace micropather {
  class MicroPather;
}  // namespace  Micropather

namespace suncatcher {

namespace pathfinder {
  class MicropatherGraph;
  class Map;
}  // namespace pathfinder

namespace test {

class DeltaMap;

class MPWrapper {
  public:
    MPWrapper(const DeltaMap* map);

    void new_pather();

    pathfinder::Path path(pathfinder::Coord start, pathfinder::Coord finish);

  private:
    const pathfinder::Map* my_map;
    std::unique_ptr<pathfinder::MicropatherGraph> mpg;
    std::unique_ptr<micropather::MicroPather> mp;
};

}  // namespace test
}  // namespace suncatcher

#endif  /* MPWRAPPER_616e1e92191d445dac0fe08e40c6429b */
