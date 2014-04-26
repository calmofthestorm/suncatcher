#ifndef MAPMUTATOR_4687638a8f5c4d2fb43b15d6274b78c7
#define MAPMUTATOR_4687638a8f5c4d2fb43b15d6274b78c7

#include "MapMutator.h"
#include "Map.h"

namespace suncatcher {
namespace pathfinder {

MapMutator::MapMutator(Map* map_in)
: map(map_in) {
  if (map_in) {
    map_in->notify_mutator_created();
  }
}

MapMutator::~MapMutator() {
  if (map) {
    map->notify_mutator_destroyed();
  }
}

}  // namespace pathfinder
}  // namespace suncatcher


#endif  /* MAPMUTATOR_4687638a8f5c4d2fb43b15d6274b78c7 */
