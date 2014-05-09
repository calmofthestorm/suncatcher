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

#ifndef MAPBUILDER_295c500da7c446bd985189b58095766c
#define MAPBUILDER_295c500da7c446bd985189b58095766c

#include "suncatcher/GraphDelegate.hh"
#include "suncatcher/Door.hh"
#include "suncatcher/graph/EuclideanGraphBuilder.hh"

namespace suncatcher {
namespace pathfinder {

class MapBuilder {
  public:
    MapBuilder();

    MapBuilder(graph::EuclideanGraphBuilder&& egb)
    : doors(std::move(egb.doors)) {

      graph = graph::EuclideanGraph(std::move(egb));
    }

    MapBuilder(
        pathfinder::GraphDelegate&& gd,
        std::map<const Coord, Door>&& doors_in
      )
    : graph(std::move(gd)),
      doors(std::move(doors_in)) { }

    MapBuilder& set_graph(pathfinder::GraphDelegate gd) {
      std::swap(gd, graph);
      return *this;
    }

    MapBuilder& set_doors(std::map<const Coord, Door>&& doors_in) {
      std::swap(doors, doors_in);
      return *this;
    }

  private:
    friend class MapImpl;
    pathfinder::GraphDelegate graph;
    std::map<const Coord, Door> doors;
};

}  // namespace pathfinder
}  // namespace suncatcher


#endif  /* MAPBUILDER_295c500da7c446bd985189b58095766c */
