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

#ifndef EUCLIDEANPATHERSTATE_5f403e3fac21428bbfed9b35edee721c
#define EUCLIDEANPATHERSTATE_5f403e3fac21428bbfed9b35edee721c


#include "suncatcher/graph/EuclideanCoord.hh"


namespace suncatcher {
namespace graph {

class EuclideanPatherState {
  public:
    inline EuclideanPatherState(EuclideanCoord size);

    inline bool get_expanded(EuclideanCoord cell) const;
    inline void set_expanded(EuclideanCoord cell, bool value);

    inline float get_distance(EuclideanCoord cell) const;
    inline void set_distance(EuclideanCoord cell, float value);

    inline EuclideanCoord get_previous(EuclideanCoord cell) const;
    inline void set_previous(EuclideanCoord cell, EuclideanCoord value);

    inline void clear();


  private:
    util::Grid<uint8_t> version;
    util::Grid<bool> expanded;
    util::Grid<float> distance;
    util::Grid<Coord> previous;
    uint8_t current;
};

}  // namespace graph
}  // namespace suncatcher


#include "suncatcher/graph/EuclideanPatherState-inl.hh"


#endif  /* EUCLIDEANPATHERSTATE_5f403e3fac21428bbfed9b35edee721c */
