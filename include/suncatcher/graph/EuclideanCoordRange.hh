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

#ifndef COORDRANGE_b4aa66ac1ad04cc2b1f3bee4f59b9af4
#define COORDRANGE_b4aa66ac1ad04cc2b1f3bee4f59b9af4

#include "suncatcher/graph/EuclideanCoord.hh"

namespace suncatcher {
namespace graph {

class EuclideanCoordRange {
  public:
    class const_iterator {
      public:
        typedef const_iterator self_type;
        typedef EuclideanCoord value_type;
        typedef EuclideanCoord& reference;
        typedef EuclideanCoord* pointer;
        typedef ptrdiff_t difference_type;
        typedef std::input_iterator_tag iterator_category;

        const_iterator(EuclideanCoord start, EuclideanCoord size);
        self_type operator++();
        self_type operator++(int);
        const value_type& operator*() const;
        bool operator==(const self_type& rhs) const;
        bool operator!=(const self_type& rhs) const;

      private:
        EuclideanCoord pos;
        EuclideanCoord size;
    };

    inline explicit EuclideanCoordRange(EuclideanCoord c);

    inline const_iterator begin() const;
    inline const_iterator end() const;

    inline size_t size() const;

    inline EuclideanCoord euclidean_size() const;

    inline EuclideanCoord get_coord_by_index(size_t index) const;
    inline size_t get_index_by_coord(EuclideanCoord cell) const;

  private:
    EuclideanCoord range_size;
};

}  // namespace graph
}  // namespace suncatcher

#include "suncatcher/graph/EuclideanCoordRange-inl.hh"

#endif  /* COORDRANGE_b4aa66ac1ad04cc2b1f3bee4f59b9af4 */
