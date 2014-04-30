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
// may need revisiting in the future.

#include <iostream>
#include <map>
#include <vector>

#include "Grid.h"
#include "Path.h"
#include "Door.h"
#include "MapMutator.h"
#include "DynamicDisjointSets.h"


namespace suncatcher {

namespace test {
  class DeltaMap;
}  // namespace test

namespace pathfinder {

const uint_least8_t PATH_COST_INFINITE = (uint_least8_t)-1;

const uint_least32_t COLOR_MULTIPLE = (uint_least32_t)-1;
const uint_least32_t COLOR_UNKNOWN = (uint_least32_t)-2;
const uint_least32_t COLOR_IMPASSABLE = (uint_least32_t)-3;

class MapBuilder;

class Map {
  public:
    // See MapBuilder for construction information.
    Map(MapBuilder&& builder);
    ~Map() noexcept;

    // Disabling copy due to large + why would you want to + nontrivial to
    // handle the linked mutator logic.
    Map& operator= (const Map& other) = delete;
    Map(const Map& other) = delete;

    // Since mutators store a pointer to maps, moves are illegal.
    Map& operator= (Map&& other) = delete;
    Map(Map&& other) = delete;

    // Compute the shortest path between two points if one exists. Returns false
    // path on failure (out of bounds, no path, impassable src/dest, etc).
    Path path(Coord src, Coord dst) const ;

    // Returns true if it is possible to path from src to dst. Unlike path,
    // this will always run in constant time. Invalid inputs result in false.
    bool path_exists(
        pathfinder::Coord src,
        pathfinder::Coord dst
        ) const ;

    // Returns the map's size, as a coordinate.
    inline Coord size() const { return data.size(); }

    // Returns a constant reference to the mapping from coordinates to doors.
    const std::map<const Coord, Door>& get_doors() const { return doors; }

    // Returns a constant reference to the underlying backing, for bounds
    // checks, direct access, etc.
    const suncatcher::util::Grid<uint_least8_t>& get_data() const { return data; }

    // Returns the current cost to move from start to finish. The two must
    // be adjacent (including diagonal). Start must be passable. Returns -1
    // if the move is illegal. Asserts coordinates adjacent.
    inline float move_cost(Coord start, Coord finish) const;

    // True iff the cell is currently passable in its current state (eg, doors
    // return true/false based on state).
    inline bool is_passable(Coord cell) const;

    // True iff the cell/static component is a door. We can take either a
    // coordinate or a (valid) color, since all doors have their own color.
    // Both are constant time.
    inline bool is_door(Coord cell) const;
    inline bool is_door(uint_least32_t cell_color) const;

    // True iff the cell is always passable to all movement modes and factions.
    // Thus, returns false for doors regardless of state.
    inline bool is_transparent(Coord cell) const;

    // Returns a mutator object, which can be used to describe desired changes
    // in game world state. Note that destruction of a map with outstanding
    // mutators results in assertion failure and, if assertions disabled,
    // undefined behavior. These need to have their lifetimes managed by
    // the PathManager.
    MapMutator get_mutator();

    // Changes the world immediately, doing all necessary computations. The
    // mutator is cleared to clean state.
    void mutate(MapMutator&& mutation);

    // Useful debugging features -- dump a simple representation of aspects
    // of the map to a stream.
    void print_colors(std::ostream& os) const;
    void print_static_components(std::ostream& os) const;
    void print_dynamic_components(std::ostream& os) const;
    void print_map(std::ostream& os, const Path& path={}) const;

    // Forces recomputation of all cached information.
    void clear_cache();

  private:
    // CONTRACT: MapMutator MUST NOT use friendship for ANY PURPOSE except
    // calling notify_mutator_destroyed and notify_mutator_created.
    // TODO: better design?
    friend class MapMutator;
    friend class test::DeltaMap;

    // Notifies the Map that the mutator was destroyed/copied, so the
    // outstanding mutator reference count can be updated.
    inline void notify_mutator_destroyed() noexcept {
      assert(outstanding_mutators > 0);
      --outstanding_mutators;
    }
    inline void notify_mutator_created() noexcept { ++outstanding_mutators;};

    // Mutator synchronization state
    size_t version;
    size_t outstanding_mutators;

    // Cost to traverse terrain.
    suncatcher::util::Grid<uint_least8_t> data;

    // All registered doors.
    std::map<const Coord, Door> doors;

    // The map is divided into "colors" -- areas connected by purely transparent
    // terrain along Manhattan directions. Colors are handles into static
    // components (thus, we can union two colors in constant time at the price
    // of a small indirect lookup table). Typically there will be anywhere from
    // 1-5 colors per door on the map, plus one for each isolated walled off
    // region.
    suncatcher::util::Grid<uint_least32_t> color;

    // Static component -- represent the same areas as colors, but there may
    // be a many-to-one mapping of colors to static components -- for example,
    // if we remove a door, the three areas (two rooms and door itself) retain
    // their colors but would all map to the same static component.
    util::UnionFind<uint_least32_t> static_component;

    // Dynamic component tracking -- organizes static components (rooms) into
    // dynamic components (reachability). This lets us minimize expensive
    // operations such as DFS to determine connected components by performing
    // the frequent ones on a simplified graph.
    util::DynamicDisjointSets<uint_least32_t> dynamic_component;

    // Whether dynamic updates are enabled. If false, the Map will reinitialize
    // after every mutation. The MapBuilder sets this.
    bool dynamic_updates;

    // All colors less than door_base are non-doors. Any color >=
    // door_base, if valid, is a door.
    uint_least32_t door_base_color;
};

class MapBuilder {
  public:
    MapBuilder();
    MapBuilder(Coord size, uint_least8_t default_cost);

    // Load a MapBuilder from a simple text format. Intended mostly for
    // tests and debugging.
    MapBuilder(std::istream& is);

    // Set/get the cost of the specified cell. Can't do this to a door.
    inline const uint_least8_t& cost(Coord cell) const { return data.at(cell); }
    inline uint_least8_t& cost(Coord cell) {
      assert(doors.find(cell) == doors.end());
      return data.at(cell);
    }

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

#include "Map-inl.h"

#endif  /* PATHFINDER_2c83331d8b5849b28b5f40b38a444a7a */
