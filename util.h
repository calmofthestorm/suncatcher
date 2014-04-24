#ifndef UTIL_116226f556b04bd5945185d15b5ea706
#define UTIL_116226f556b04bd5945185d15b5ea706

#include <vector>

namespace suncatcher {

namespace pathfinder {
  class Coord;
}  // namespace pathfinder

namespace util {

int find_representative(const std::vector<int>& parents, int elem);

float manhattan(
    const suncatcher::pathfinder::Coord& a,
    const suncatcher::pathfinder::Coord& b
  );

}  // namespace util
}  // namespace suncatcher

#endif  /* UTIL_116226f556b04bd5945185d15b5ea706 */
