#ifndef MAPBUILDER_36ddaa5cac2d43bc999e75a96dafc375
#define MAPBUILDER_36ddaa5cac2d43bc999e75a96dafc375

#include <cstdint>
#include <iosfwd>
#include <map>

#include "suncatcher/util/Grid.hh"

#include "suncatcher/Door.hh"
#include "suncatcher/Coord.hh"

namespace suncatcher {
namespace pathfinder {

class Map;

class MapBuilder {
  public:
    MapBuilder();
    MapBuilder(Coord size, uint_least8_t default_cost);

    // Load a MapBuilder from a simple text format. Intended mostly for
    // tests and debugging.
    explicit MapBuilder(std::istream& is);

    // Set/get the cost of the specified cell. Can't do this to a door.
    inline const uint_least8_t& cost(Coord cell) const { return data.at(cell); }
    inline uint_least8_t& cost(Coord cell) {
      assert(doors.find(cell) == doors.end());
      return data.at(cell);
    }

    // If dynamic updates are disabled, every mutation will recompute all cache
    // state (linear in graph size).
    void enable_dynamic_updates(bool enabled) { dynamic_updates = enabled; }

    // Add a door to the given cell.
    void add_door(Coord cell, bool open, uint_least8_t cost_open,
                  uint_least8_t cost_closed);

  private:
    friend class Map;
    suncatcher::util::Grid<uint_least8_t> data;
    bool dynamic_updates;
    std::map<const Coord, Door> doors;
};

}  // namespace pathfinder
}  // namespace suncatcher

#endif  /* MAPBUILDER_36ddaa5cac2d43bc999e75a96dafc375 */
