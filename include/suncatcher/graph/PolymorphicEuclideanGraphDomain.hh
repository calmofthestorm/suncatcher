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

#ifndef POLYMORPHICEUCLIDEANGRAPHDOMAIN_ce2429e5db8e404486c9ff629ada6435
#define POLYMORPHICEUCLIDEANGRAPHDOMAIN_ce2429e5db8e404486c9ff629ada6435

#include "suncatcher/Coord.hh"
#include "suncatcher/CoordRange.hh"
#include "suncatcher/graph/GraphDomainInterface.hh"

namespace suncatcher {
namespace graph {



class PolymorphicEuclideanGraphDomain : public GraphDomainInterface {
  public:
    PolymorphicEuclideanGraphDomain(pathfinder::CoordRange cr);
    virtual ~PolymorphicEuclideanGraphDomain() override;

    virtual DomainIterator begin() override;
    virtual DomainIterator end() override;

    virtual size_t size() override;

    virtual pathfinder::Coord get_coord_by_index(size_t index) override;
    virtual size_t get_index_by_coord(pathfinder::Coord cell) override;


  private:
    pathfinder::CoordRange domain;
};



}  // namespace graph
}  // namespace suncatcher

#endif  /* POLYMORPHICEUCLIDEANGRAPHDOMAIN_ce2429e5db8e404486c9ff629ada6435 */
