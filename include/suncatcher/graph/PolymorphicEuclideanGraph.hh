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

// Provides a euclidean graph via the polymorphic API. Primarily for running
// the tests and way of example.

#ifndef POLYMORPHICEUCLIDEANGRAPH_aff7242533b043fd80d827b892d9a6a7
#define POLYMORPHICEUCLIDEANGRAPH_aff7242533b043fd80d827b892d9a6a7

#include "suncatcher/Coord.hh"
#include "suncatcher/graph/GraphInterface.hh"
#include "suncatcher/graph/GraphDomainInterface.hh"
#include "suncatcher/graph/EuclideanGraph.hh"

namespace suncatcher {
namespace graph {

class EuclideanGraphBuilder;

class PolymorphicEuclideanGraph : public GraphInterface {
  public:
    PolymorphicEuclideanGraph(graph::EuclideanGraphBuilder&& builder);

    virtual ~PolymorphicEuclideanGraph() override;

    virtual float move_cost(
        pathfinder::Coord start,
        pathfinder::Coord finish
      ) override;

    virtual uint_least8_t get_cost(pathfinder::Coord cell) override;
    virtual void set_cost(pathfinder::Coord cell, uint_least8_t cost) override;

    virtual int_least32_t get_color(pathfinder::Coord cell) override;
    virtual void set_color(
        pathfinder::Coord cell,
        int_least32_t color
      ) override;

    virtual pathfinder::Coord size() override;
    virtual bool check_bounds(pathfinder::Coord cell) override;

    virtual void get_adjacent(
        const pathfinder::Coord cell,
        bool include_diagonals,
        std::vector<pathfinder::Coord>& adj
      ) override;

    virtual void fill_color(int_least32_t fill) override;

    virtual bool is_passable(pathfinder::Coord cell) override;

    virtual GraphDomain domain() const override;

    virtual std::unique_ptr<GraphInterface> lazy_clone() const override;


  protected:
    EuclideanGraph delegate;
};

}  // namespace graph
}  // namespace suncatcher

#endif  /* POLYMORPHICEUCLIDEANGRAPH_aff7242533b043fd80d827b892d9a6a7 */
