#ifndef PATHFINDER_2c83331d8b5849b28b5f40b38a444a7a
#define PATHFINDER_2c83331d8b5849b28b5f40b38a444a7a

// Pathfinder abstraction of the map. The Map class maintains a simplified
// abstraction of the map, intended to support efficient pathfinding,
// reachability, and updates in a dynamic world.
//
// In an early version of this code, I ran a benchmark suite to determine
// whether using smaller integral types for better cache performance versus
// using native word types resulted in better performance. I found ~5%
// improvement on a moderate sample size (p < 0.01) to using smaller types,
// which makes sense given all the computations are simple and most of this
// algorithm is fetching the map from memory.
//
// The other concern is that predictive pathing in multiple threads could run
// into memory pressure.
//
// The code has undergone substantial refactoring since then, however, so this
// may were three visiting in the future.

#include <iostream>
#include <map>
#include <vector>

#include "Grid.h"
#include "Path.h"

namespace suncatcher {
namespace pathfinder {

const uint8_t PATH_COST_INFINITE = (uint8_t)-1;

const uint32_t COMPONENT_MULTIPLE = (uint32_t)-1;
const uint32_t COMPONENT_UNKNOWN = (uint32_t)-2;
const uint32_t COMPONENT_IMPASSABLE = (uint32_t)-3;

class MapBuilder;
class MapMutator;

// Not necessarily a door; represents any point that is expected to change
// between passable and impassible during normal play. This could be used to
// represent anything, including possibly an in-construction wall being converted
// to an open door any time between queueing and completed construction, then
// "closed" and removed in constant time when the wall is completed.

struct Door {
  bool open;
  uint8_t cost_open; // cost to walk through when open
  uint8_t cost_closed; // cost to walk through when closed
  std::vector<uint_least32_t> adjacent_components;
};

class Map {
  public:
    // See MapBuilder for construction information.
    Map(MapBuilder&& builder);
    ~Map();

    Map& operator= (const Map& other) = delete;
    Map(const Map& other) = delete;

    Map& operator= (Map&& other) = default;
    Map(Map&& other) = default;

    // Compute the shortest path between two points if one exists. Returns false
    // path on failure (out of bounds, no path, impassable src/dest, etc).
    Path path(
        const pathfinder::Coord& src,
        const pathfinder::Coord& dst
      ) const ;

    // Returns true if it is possible to path from src to dst. Unlike path,
    // this will always run in constant time. Invalid inputs result in false.
    bool path_exists(
        pathfinder::Coord src,
        pathfinder::Coord dst
        ) const ;

    // Returns the map's size, as a coordinate.
    inline Coord size() const { return data.size(); }

    // Returns a constant reference to the mapping from coordinates to doors.
    const std::map<Coord, Door>& get_doors() const { return doors; }

    // Returns a constant reference to the underlying backing, for bounds
    // checks, direct access, etc.
    const suncatcher::util::Grid<uint_least8_t>& get_data() const { return data; }

    // Returns the current cost to move from start to finish. The two must
    // be adjacent (including diagonal). Start must be passable. Returns -1
    // if the move is illegal. Asserts coordinates adjacent.
    inline float move_cost(Coord start, Coord finish) const;

    // True iff the cell is always passable to all movement modes and factions.
    // Thus, returns false for doors regardless of state.
    inline bool is_transparent(Coord cell) const;

    // True iff the cell is ALWAYS impassable to all movement modes and factions.
    // (think walls...closed doors are neither transparant nor opaque)
    inline bool is_opaque(Coord cell) const;

    // True iff the cell is currently passable in its current state (eg, doors
    // return true/false based on state).
    inline bool is_passable(Coord cell) const;

    // Returns a mutator object, which can be used to describe desired changes
    // in game world state. Note that destruction of a map with outstanding
    // mutators results in assertion failure and, if assertions disabled,
    // undefined behavior. These need to have their lifetimes managed by
    // the PathManager.
    MapMutator get_mutator();

    // Changes the world immediately, doing all necessary computations. The
    // mutator is cleared to clean state.
    void mutate(MapMutator&& mutation);

    // Useful debugging features -- dump a simple representation of aspecs
    // of the map to a stream.
    void print_components(std::ostream& os) const;
    void print_equivalence_classes(std::ostream& os) const;
    void print_map(std::ostream& os, const Path& path={}) const;

    // Forces recomputation of all cached information.
    void clear_cache();

  private:
    // CONTRACT: MapMutator MUST NOT use friendship for ANY PURPOSE except
    // calling notify_mutator_destroyed and notify_mutator_created.
    // TODO: better design?
    friend class MapMutator;

    // Notifies the Map that the mutator was destroyed/copied, so the
    // outstanding mutator reference count can be updated.
    inline void notify_mutator_destroyed() { --outstanding_mutators; }
    inline void notify_mutator_created() { ++outstanding_mutators;};

    void update_equivalence(const pathfinder::Coord& pos, bool new_state);

    void rebuild_cache();
    void rebuild_equivalence_classes();

    size_t outstanding_mutators;
    suncatcher::util::Grid<uint_least8_t> data;
    suncatcher::util::Grid<uint_least32_t> component;
    std::vector<uint_least32_t> equivalent_components;
    std::map<pathfinder::Coord, Door> doors;
};

class MapBuilder {
  public:
    MapBuilder();
    MapBuilder(const Coord& size, uint8_t cost);

    // Load a MapBuilder from a simple text format. Intended mostly for
    // tests and debugging.
    MapBuilder(std::istream& is);

    // Set/get the cost of the specified cell. Can't do this to a door.
    inline const uint8_t& cost(const Coord& cell) const { return data.at(cell); }
    inline uint8_t& cost(const Coord& cell) {
      assert(doors.find(cell) == doors.end());
      return data.at(cell);
    }

    // Add a door to the given cell.
    void add_door(const Coord& cell, bool open, uint8_t cost_open,
                  uint8_t cost_closed);

  private:
    friend class Map;
    suncatcher::util::Grid<uint8_t> data;
    std::map<pathfinder::Coord, Door> doors;
};

class MapMutator {
  public:
    MapMutator()
    : map(nullptr) { }

    ~MapMutator() { if (map) map->notify_mutator_destroyed(); }

    MapMutator(const MapMutator& other) = delete;
    MapMutator& operator=(const MapMutator& other) = delete;

    MapMutator& operator=(MapMutator&& other) = default;
    MapMutator(MapMutator&& other) = default;

  private:
    friend class Map;

    MapMutator(Map* map_in)
    : map(map_in) { map_in->notify_mutator_created(); }

    Map* map;
};

}  // namespace pathfinder
}  // namespace suncatcher

#include "Map-inl.h"

#endif  /* PATHFINDER_2c83331d8b5849b28b5f40b38a444a7a */
