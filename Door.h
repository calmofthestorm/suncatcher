#ifndef DOOR_9d946b6074164accb5974a3808196f91
#define DOOR_9d946b6074164accb5974a3808196f91

namespace suncatcher {
namespace pathfinder {



// Not necessarily a door; anything that changes between between passable and
// impassible during normal play. This could be used to represent anything,
// including possibly an in-construction wall being converted to an open door
// any time between queueing and completed construction, then "closed" and
// removed in constant time when the wall is completed.
struct Door {
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
