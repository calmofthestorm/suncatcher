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

#include "suncatcher/Coord.hh"
#include "suncatcher/util/util.hh"
#include "suncatcher/graph/GraphInterface.hh"
#include "suncatcher/graph/GraphDomainInterface.hh"
#include "suncatcher/graph/EuclideanGraphBuilder.hh"
#include "suncatcher/graph/PolymorphicEuclideanGraph.hh"
#include "suncatcher/graph/PolymorphicEuclideanGraphDomain.hh"

namespace suncatcher {
namespace graph {

PolymorphicEuclideanGraph::PolymorphicEuclideanGraph(
    graph::EuclideanGraphBuilder&& builder
  )
: delegate(std::move(builder)) { }


PolymorphicEuclideanGraph::~PolymorphicEuclideanGraph() { }


float PolymorphicEuclideanGraph::move_cost(
Coord start,
Coord finish
) {
  return delegate.move_cost(start, finish);
}


uint_least8_t PolymorphicEuclideanGraph::get_cost(Coord cell) {
  return delegate.get_cost(cell);
}


void PolymorphicEuclideanGraph::set_cost(
  Coord cell,
  uint_least8_t cost
) {
  delegate.set_cost(cell, cost);
}


int_least32_t PolymorphicEuclideanGraph::get_color(Coord cell) {
  return delegate.get_color(cell);
}


void PolymorphicEuclideanGraph::set_color(
Coord cell,
int_least32_t color
) {
  delegate.set_color(cell, color);
}


Coord PolymorphicEuclideanGraph::size() {
  return delegate.size();
}


bool PolymorphicEuclideanGraph::check_bounds(Coord cell) {
  return delegate.check_bounds(cell);
}


void PolymorphicEuclideanGraph::get_adjacent(
  const Coord cell,
  bool include_diagonals,
std::vector<Coord>& adj
) {
  return delegate.get_adjacent(cell, include_diagonals, adj);
}


void PolymorphicEuclideanGraph::fill_color(int_least32_t fill) {
  delegate.fill_color(fill);
}


bool PolymorphicEuclideanGraph::is_passable(Coord cell) {
  return delegate.is_passable(cell);
}


GraphDomain PolymorphicEuclideanGraph::domain() const {
  return GraphDomain(make_unique<PolymorphicEuclideanGraphDomain>(delegate.domain()));
}


std::unique_ptr<GraphInterface> PolymorphicEuclideanGraph::lazy_clone() const {
  return make_unique<PolymorphicEuclideanGraph>(*this);
}



}  // namespace graph
}  // namespace suncatcher
