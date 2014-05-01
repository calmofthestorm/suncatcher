#include "MPWrapper.h"

#include "DeltaMap.h"
#include "Path.h"
#include "micropather/MicropatherGraph.h"
#include "Coord.h"

namespace suncatcher {
namespace test {

using suncatcher::test::DeltaMap;
using suncatcher::pathfinder::MicropatherGraph;
using suncatcher::pathfinder::Map;
using suncatcher::pathfinder::Path;
using suncatcher::pathfinder::Coord;

MPWrapper::MPWrapper(const DeltaMap* map) {
  my_map = &map->get_simple();
  mpg.reset(new MicropatherGraph(&map->get_simple()));
}

void MPWrapper::new_pather() {
  mp.reset(new micropather::MicroPather(mpg.get(), 4000000, 8, false));
}

Path MPWrapper::path(Coord start, Coord finish) {
  if (!my_map->get_data().check_bounds(start) ||
      !my_map->get_data().check_bounds(finish) ||
      !my_map->is_passable(start) || !my_map->is_passable(finish)) {
    return Path({}, -1);
  }
  if (start == finish) {
    return Path{{start}, 0};
  }
  // Paradoxically, keeping the pather around slows it down AND caches even
  // when manually cleared and constructed with disabled cache...
  new_pather();
  MP_VECTOR<void*> path_vc;
  float length = 279;
  mp->Solve(mpg->encode(start), mpg->encode(finish), &path_vc, &length);
  std::vector<Coord> real_path(path_vc.size());
  for (size_t i = 0; i < path_vc.size(); ++i) {
    real_path[i] = mpg->decode(path_vc[i]);
  }
  if (real_path.size() > 0) {
    return Path(std::move(real_path), length);
  } else {
    return Path{{}, -1};
  }
}

}  // namespace test
}  // namespace suncatcher
