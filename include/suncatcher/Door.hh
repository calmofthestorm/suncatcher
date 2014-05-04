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

#ifndef DOOR_9d946b6074164accb5974a3808196f91
#define DOOR_9d946b6074164accb5974a3808196f91

#include <cstdint>

namespace suncatcher {
namespace pathfinder {


// Not necessarily a door; anything that changes between between passable and
// impassible during normal play. This could be used to represent anything,
// including possibly an in-construction wall being converted to an open door
// any time between queueing and completed construction, then "closed" and
// removed in constant time when the wall is completed.
class Door {
  public:
    bool open;
    uint_least8_t cost_open; // cost to walk through when open
    uint_least8_t cost_closed; // cost to walk through when closed

    bool operator==(const Door& other) const {
      return (other.open == open &&
              other.cost_open == cost_open &&
              other.cost_closed == cost_closed);
    }
};


}  // namespace pathfinder
}  // namespace suncatcher

#endif  /* DOOR_9d946b6074164accb5974a3808196f91 */
