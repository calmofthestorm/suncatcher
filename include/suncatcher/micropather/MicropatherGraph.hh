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
#include "suncatcher/MapView.hh"

#include "suncatcher/util/util.hh"

namespace suncatcher {
namespace test {

using util::manhattan;

class MicropatherGraph : public micropather::Graph {
  public:
    explicit MicropatherGraph(pathfinder::MapView map)
    : graph(map) { }

    virtual float LeastCostEstimate(void* stateStart, void* stateEnd) const {
      return manhattan(decode(stateStart), decode(stateEnd));
    }

    virtual void AdjacentCost(
        void* state, MP_VECTOR< micropather::StateCost > *adjacent
      ) const {
      auto cur = decode(state);
      assert(graph.is_passable(cur));
      for (const auto& n : graph.get_adjacent(cur)) {
        if (graph.is_passable(n) && graph.is_passable(cur)) {
          float cost = graph.move_cost(cur, n);
          if (cost != -1) {
            adjacent->push_back({encode(n), cost});
          }
        }
      }
    }

    virtual void PrintStateInfo(void*) const { }

    void* encode(const pathfinder::Coord& c) const {
      static_assert(sizeof(c.row) + sizeof(c.col) + sizeof(c.layer) <=
                    sizeof(void*), "");
      uintptr_t encoded = c.col + graph.size().col * (c.row + graph.size().row * c.layer);
      return (void*)encoded;
    }

    pathfinder::Coord decode(void* c) const {
      uintptr_t encoded = (uintptr_t)c;
      uintptr_t width = graph.size().col;
      uintptr_t height = graph.size().row;
      pathfinder::Coord rval;
      rval.layer = encoded / (width * height);
      encoded %= (width * height);
      rval.row = encoded / width;
      rval.col = encoded % width;
      return rval;
    }

  private:
    pathfinder::MapView graph;
};

}  // namespace test
}  // namespace suncatcher

#endif  /* MICROPATHERGRAPH_0b8eae0922b042fe860d049a997b4c28 */
