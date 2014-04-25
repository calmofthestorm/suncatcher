#ifndef UTIL_116226f556b04bd5945185d15b5ea706
#define UTIL_116226f556b04bd5945185d15b5ea706

#include <cstdint>
#include <cmath>
#include <vector>

namespace suncatcher {

namespace pathfinder {
  class Coord;
}  // namespace pathfinder

namespace util {

inline uint_least32_t find_representative(
    const std::vector<uint_least32_t>& parents,
    uint_least32_t elem
  ) {
  while (elem != parents[elem]) {
    elem = parents[elem];
  }
  return elem;
}

inline float manhattan(const pathfinder::Coord& a, const pathfinder::Coord& b) {
  return pow(pow(std::abs((float)a.row - (float)b.row), 2) + pow(std::abs((float)a.col - (float)b.col), 2), 0.5);
}

}  // namespace util
}  // namespace suncatcher

#endif  /* UTIL_116226f556b04bd5945185d15b5ea706 */
