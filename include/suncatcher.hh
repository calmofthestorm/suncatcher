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

#ifndef SUNCATCHER_81cfd67ed28c4409bd6d189fcc635dc9
#define SUNCATCHER_81cfd67ed28c4409bd6d189fcc635dc9

#include <cstdint>
#include <limits>

namespace suncatcher {


const uint_least8_t PATH_COST_INFINITE = (uint_least8_t)-1;

const int_least32_t COLOR_MULTIPLE = std::numeric_limits<int_least32_t>::min();
const int_least32_t COLOR_UNKNOWN = COLOR_MULTIPLE + 1;
const int_least32_t COLOR_IMPASSABLE = COLOR_UNKNOWN + 1;
const int_least32_t COLOR_LOWEST_VALID = COLOR_IMPASSABLE + 1;


}  // namespace suncatcher

#endif  /* SUNCATCHER_81cfd67ed28c4409bd6d189fcc635dc9 */
