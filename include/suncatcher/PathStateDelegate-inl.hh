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

namespace suncatcher {
namespace pathfinder {


#ifdef POLYMORPHIC_API


inline PatherStateDelegate::PatherStateDelegate(
    std::unique_ptr<graph::PatherStateInterface> impl_i
  )
: impl(std::move(impl_i)) { }


#else


inline PatherStateDelegate::PatherStateDelegate(graph::EuclideanPatherState&& impl_i)
: impl(std::move(impl_i)) { }


inline PatherStateDelegate::PatherStateDelegate(Coord size)
: impl(size) { }


#endif


inline bool PatherStateDelegate::get_expanded(Coord cell) const {
  #ifdef POLYMORPHIC_API
  return impl->get_expanded(cell);
  #else
  return impl.get_expanded(cell);
  #endif
}


inline void PatherStateDelegate::set_expanded(Coord cell, bool value) {
  #ifdef POLYMORPHIC_API
  impl->set_expanded(cell, value);
  #else
  impl.set_expanded(cell, value);
  #endif
}



inline Coord PatherStateDelegate::get_previous(Coord cell) const {
  #ifdef POLYMORPHIC_API
  return impl->get_previous(cell);
  #else
  return impl.get_previous(cell);
  #endif
}


inline void PatherStateDelegate::set_previous(Coord cell, Coord value) {
  #ifdef POLYMORPHIC_API
  impl->set_previous(cell, value);
  #else
  impl.set_previous(cell, value);
  #endif
}



inline float PatherStateDelegate::get_distance(Coord cell) const {
  #ifdef POLYMORPHIC_API
  return impl->get_distance(cell);
  #else
  return impl.get_distance(cell);
  #endif
}


inline void PatherStateDelegate::set_distance(Coord cell, float value) {
  #ifdef POLYMORPHIC_API
  impl->set_distance(cell, value);
  #else
  impl.set_distance(cell, value);
  #endif
}



void PatherStateDelegate::clear() {
  #ifdef POLYMORPHIC_API
  impl->clear();
  #else
  impl.clear();
  #endif
}



}  // namespace pathfinder
}  // namespace suncatcher
