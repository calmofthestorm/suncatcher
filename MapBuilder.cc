#include "MapBuilder.h"

#include <fstream>
#include <string>

namespace suncatcher {
namespace pathfinder {

MapBuilder::MapBuilder()
: data({0, 0}, 0),
  dynamic_updates(true),
  doors() { }

MapBuilder::MapBuilder(Coord size, uint_least8_t default_cost)
: data(size, default_cost),
  dynamic_updates(true),
  doors() { }

void MapBuilder::add_door(Coord cell, bool open, uint_least8_t cost_open,
                          uint_least8_t cost_closed) {
  assert(doors.find(cell) == doors.end());
  doors[cell] = {open, cost_open, cost_closed};
  data.at(cell) = open ? cost_open : cost_closed;
}

}  // namespace pathfinder
}  // namespace suncatcher
