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

#include "suncatcher/platform.hh"

#ifdef MICROPATHER_DELTA_TEST

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
      std::vector<Coord> adj;
      graph.get_adjacent(cur, true, adj);
      for (const auto& n : adj) {
        if (graph.is_passable(n) && graph.is_passable(cur)) {
          float cost = graph.move_cost(cur, n);
          if (cost != -1) {
            adjacent->push_back({encode(n), cost});
          }
        }
      }
    }

    virtual void PrintStateInfo(void*) const { }

    void* encode(const Coord& c) const {
      static_assert(sizeof(c.row) + sizeof(c.col) + sizeof(c.layer) <=
                    sizeof(void*), "");
      static const uint16_t dim = std::numeric_limits<uint16_t>::max();
      uintptr_t encoded = c.col + dim * (c.row + dim * c.layer);
      return (void*)encoded;
    }

    Coord decode(void* c) const {
      static const uint16_t dim = std::numeric_limits<uint16_t>::max();

      uintptr_t encoded = (uintptr_t)c;

      Coord rval;
      rval.layer = encoded / ((size_t)dim * (size_t)dim);
      encoded %= ((size_t)dim * (size_t)dim);
      rval.row = encoded / dim;
      rval.col = encoded % dim;
      return rval;
    }

  private:
    pathfinder::MapView graph;
};

}  // namespace test
}  // namespace suncatcher

#endif

#endif  /* MICROPATHERGRAPH_0b8eae0922b042fe860d049a997b4c28 */
