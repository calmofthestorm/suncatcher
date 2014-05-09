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

#ifndef GRAPHDOMAININTERFACE_524af6b0ad504f30a87f10c7991caf20
#define GRAPHDOMAININTERFACE_524af6b0ad504f30a87f10c7991caf20

#include <any_iterator/any_iterator.hpp>

namespace suncatcher {
namespace graph {

class GraphDomainInterface {
  public:
    typedef IteratorTypeErasure::any_iterator<
        pathfinder::Coord,
        std::forward_iterator_tag
      > DomainIterator;

    virtual ~GraphDomainInterface() { }

    virtual DomainIterator begin() const = 0;
    virtual DomainIterator end() const = 0;

    virtual size_t size() = 0;
}

}  // namespace graph
}  // namespace suncatcher


#endif  /* GRAPHDOMAININTERFACE_524af6b0ad504f30a87f10c7991caf20 */
