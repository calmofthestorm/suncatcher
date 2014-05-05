#ifndef SUNCATCHER_81cfd67ed28c4409bd6d189fcc635dc9
#define SUNCATCHER_81cfd67ed28c4409bd6d189fcc635dc9

#include <climits>
#include <cstdint>

namespace suncatcher {


const uint_least8_t PATH_COST_INFINITE = (uint_least8_t)-1;

const int_least32_t COLOR_MULTIPLE = std::numeric_limits<int_least32_t>::min();
const int_least32_t COLOR_UNKNOWN = COLOR_MULTIPLE + 1;
const int_least32_t COLOR_IMPASSABLE = COLOR_UNKNOWN + 1;
const int_least32_t COLOR_LOWEST_VALID = COLOR_IMPASSABLE + 1;


}  // namespace suncatcher

#endif  /* SUNCATCHER_81cfd67ed28c4409bd6d189fcc635dc9 */
