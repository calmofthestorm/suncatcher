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

#ifndef PATHSTATEDELEGATE_cbaad490fb084a638db49ea8c3804d26
#define PATHSTATEDELEGATE_cbaad490fb084a638db49ea8c3804d26

#include <memory>

#include "suncatcher/Coord.hh"
#include "suncatcher/graph/PatherStateInterface.hh"
#include "suncatcher/graph/EuclideanPatherState.hh"

namespace suncatcher {
namespace pathfinder {



class PatherStateDelegate {
  public:
    #ifdef POLYMORPHIC_API
      inline PatherStateDelegate(std::unique_ptr<graph::PatherStateInterface> impl);
    #else
      inline PatherStateDelegate(Coord size);
      inline PatherStateDelegate(graph::EuclideanPatherState&& impl);
    #endif

    inline bool get_expanded(Coord cell) const;
    inline void set_expanded(Coord cell, bool value);

    inline float get_distance(Coord cell) const;
    inline void set_distance(Coord cell, float value);

    inline Coord get_previous(Coord cell) const;
    inline void set_previous(Coord cell, Coord value);

    inline void clear();

  private:
    #ifdef POLYMORPHIC_API
      std::unique_ptr<graph::PatherStateInterface> impl;
    #else
      graph::EuclideanPatherState impl;
    #endif
};



}  // namespace pathfinder
}  // namespace suncatcher

#include "suncatcher/PathStateDelegate-inl.hh"

#endif  /* PATHSTATEDELEGATE_cbaad490fb084a638db49ea8c3804d26 */
