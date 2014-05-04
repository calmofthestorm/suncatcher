// This file is part of Suncatcher
// Alex Roper <alex@aroper.net>
//
// Suncatcher is free software: you can redistribute it and/or modify it under
// the terms of version 3 of the GNU General Public License as published by the
// Free Software Foundation.
//
// Suncatcher is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// Suncatcher.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2014 Alex Roper

#ifndef MICROPATHERGRAPH_0b8eae0922b042fe860d049a997b4c28
#define MICROPATHERGRAPH_0b8eae0922b042fe860d049a997b4c28

#include "suncatcher/micropather/micropather.hh"

#include "suncatcher/Coord.hh"
#include "suncatcher/Map.hh"

#include "suncatcher/util/util.hh"

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
      assert(graph->is_passable(cur));
      for (const auto& n : graph->get_data().get_adjacent(cur)) {
        if (graph->is_passable(n) && graph->is_passable(cur)) {
          float cost = graph->move_cost(cur, n);
          if (cost != -1) {
            adjacent->push_back({encode(n), cost});
          }
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

#endif  /* MICROPATHERGRAPH_0b8eae0922b042fe860d049a997b4c28 */
