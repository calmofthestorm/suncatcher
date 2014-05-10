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

#include "suncatcher/graph/PolymorphicEuclideanGraphDomain.hh"

namespace suncatcher {
namespace graph {

namespace {
  using pathfinder::CoordRange;
  using pathfinder::Coord;
}  // anonymous namespace

PolymorphicEuclideanGraphDomain::PolymorphicEuclideanGraphDomain(CoordRange cr)
: domain(std::move(cr)) { }


PolymorphicEuclideanGraphDomain::~PolymorphicEuclideanGraphDomain() { }


GraphDomainInterface::DomainIterator PolymorphicEuclideanGraphDomain::begin() {
  return GraphDomainInterface::DomainIterator(domain.begin());
}


GraphDomainInterface::DomainIterator PolymorphicEuclideanGraphDomain::end() {
  return GraphDomainInterface::DomainIterator(domain.end());
}


size_t PolymorphicEuclideanGraphDomain::size() {
  return domain.size();
}


Coord PolymorphicEuclideanGraphDomain::get_coord_by_index(size_t index) {
  return domain.get_coord_by_index(index);
}


size_t PolymorphicEuclideanGraphDomain::get_index_by_coord(Coord cell) {
  return domain.get_index_by_coord(cell);
}



}  // namespace graph
}  // namespace suncatcher

