#include "util.h"

#include <cmath>

#include "Coord.h"

namespace suncatcher {
namespace util {

namespace {
  using suncatcher::pathfinder::Coord;
}  // anonymous namespace

int find_representative(const std::vector<int>& parents, int elem) {
  while (elem != parents[elem]) {
    elem = parents[elem];
  }
  return elem;
}

float manhattan(const Coord& a, const Coord& b) {
  // return std::abs((float)a.row - (float)b.row) + std::abs((float)a.col - (float)b.col);
  return pow(pow(std::abs((float)a.row - (float)b.row), 2) + pow(std::abs((float)a.col - (float)b.col), 2), 0.5);
}

}  // namespace util
}  // namespace suncatcher
