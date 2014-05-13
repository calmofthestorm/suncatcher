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

#include "suncatcher/Coord.hh"

namespace suncatcher {
namespace graph {

class GraphDomainInterface {
  public:
    typedef IteratorTypeErasure::any_iterator<
        Coord const,
        std::input_iterator_tag
      > DomainIterator;

    virtual ~GraphDomainInterface() { }

    virtual DomainIterator begin() = 0;
    virtual DomainIterator end() = 0;

    virtual size_t size() = 0;

    virtual Coord get_coord_by_index(size_t index) = 0;
    virtual size_t get_index_by_coord(Coord cell) = 0;

    virtual std::unique_ptr<GraphDomainInterface> clone() = 0;
};


class GraphDomain {
  public:
    GraphDomain(std::unique_ptr<GraphDomainInterface> gdi_in)
    : gdi(std::move(gdi_in)) { }

    GraphDomain(const GraphDomain& other)
    : gdi(other.gdi->clone()) { }

    const GraphDomain& operator=(const GraphDomain& other) {
      gdi = other.gdi->clone();
      return *this;
    }

    GraphDomainInterface::DomainIterator begin() const { return gdi->begin(); }
    GraphDomainInterface::DomainIterator end() const { return gdi->end(); }

    size_t size() const { return gdi->size(); };

    Coord get_coord_by_index(size_t index) const { return gdi->get_coord_by_index(index); }
    size_t get_index_by_coord(Coord cell) const { return gdi->get_index_by_coord(cell); }

  private:
    // Mutable to avoid enforcing constness on implementors.
    mutable std::unique_ptr<GraphDomainInterface> gdi;
};



}  // namespace graph
}  // namespace suncatcher


#endif  /* GRAPHDOMAININTERFACE_524af6b0ad504f30a87f10c7991caf20 */
