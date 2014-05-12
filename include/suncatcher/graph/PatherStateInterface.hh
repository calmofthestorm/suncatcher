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
namespace graph {


class PatherStateInterface {
  public:
    virtual bool get_expanded(Coord cell) const = 0;
    virtual void set_expanded(Coord cell, bool value) = 0;

    virtual float get_distance(Coord cell) const = 0;
    virtual void set_distance(Coord cell, float value) = 0;

    virtual Coord get_previous(Coord cell) const = 0;
    virtual void set_previous(Coord cell, Coord value) = 0;

    virtual void clear() = 0;
};



}  // namespace grap
}  // namespace suncatcher
