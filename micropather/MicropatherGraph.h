#include "micropather.h"

#include "../Coord.h"
#include "../Map.h"

#include "../util.h"

namespace suncatcher {
namespace pathfinder {

using suncatcher::util::manhattan;
using suncatcher::pathfinder::Map;
using suncatcher::pathfinder::Coord;

class MicropatherGraph : public micropather::Graph {
  public:
    explicit MicropatherGraph(const Map* my_graph)
    : graph(my_graph) { }

    virtual float LeastCostEstimate(void* stateStart, void* stateEnd) const {
      return manhattan(decode(stateStart), decode(stateEnd));
    }

    virtual void AdjacentCost(
        void* state, MP_VECTOR< micropather::StateCost > *adjacent
      ) const {
      auto cur = decode(state);
      for (const auto& n : graph->get_data().get_adjacent(cur)) {
        float cost = graph->move_cost(cur, n);
        if (cost != -1) {
          adjacent->push_back({encode(n), cost});
        }
      }
    }

    virtual void PrintStateInfo(void*) const { }

    void* encode(const suncatcher::pathfinder::Coord& c) const {
      uintptr_t encoded = (c.row * graph->size().col + c.col);
      return (void*)encoded;
    }

    suncatcher::pathfinder::Coord decode(void* c) const {
      uintptr_t encoded = (uintptr_t)c;
      uintptr_t width = graph->size().col;
      return {(uint16_t)(encoded / width),
              (uint16_t)(encoded % width)};
    }

  private:
    const Map* graph;
};

}  // namespace pathfinder
}  // namespace suncatcher
