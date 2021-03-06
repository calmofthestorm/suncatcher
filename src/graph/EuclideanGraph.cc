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

#include "suncatcher/graph/EuclideanGraph.hh"

#include "suncatcher.hh"
#include "suncatcher/graph/EuclideanGraphBuilder.hh"

namespace suncatcher {
namespace graph {


EuclideanGraph::EuclideanGraph()
: color({0, 0, 0}, {0, 0, 0}, 0),
  data({0, 0, 0}, {0, 0, 0}, 0) { }


EuclideanGraph::EuclideanGraph(EuclideanGraphBuilder&& builder)
: color(std::move(builder.color)),
  data(std::move(builder.data)) { }


EuclideanGraph::EuclideanGraph(
    EuclideanCoord chunks,
    EuclideanCoord chunk_size,
    uint8_t cost_i,
    int_least32_t color_i
  )
: color(chunks, chunk_size, color_i),
  data(chunks, chunk_size, cost_i) { }


}  // namespace pathfinder
}  // namespace suncatcher
