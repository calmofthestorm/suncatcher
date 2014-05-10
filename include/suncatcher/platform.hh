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

#ifdef _MSC_VER
  #define NOEXCEPT _NOEXCEPT
  #define ALIGNED_8
#else
  // g++ & clang
  #define ALIGNED_8 __attribute__((__aligned__(8)))
  #define NOEXCEPT noexcept
#endif

// Uncomment this line to build with the polymorphic graph API. You may then
// implement GraphInterface and GraphDomainInterface to specify graph
// connectivity. Current benchmarks show this as about 12x slower in
// MapView::path, and roughly 3x slower in the expensive update operations
// that involve virtual calls in tight loops (eg, building a wall), loading
// a map, etc).
//
// If you choose to use the polymorphic API, I strongly suggest using your own
// pathing algorithm that operates directly on your abstraction, mine is 
// very general at substantial cost of speed, and improving its polymorphic
// performance is not a priority for me.
//
// If you can, strongly consider avoiding the polymorphic API if your model
// can be construed as a grid of up to 3 dimensions.
// #define POLYMORPHIC_API

// Some tests (and the repl) are able to delta against micropather for sanity.
// This will make tests run a bit slower, but is recommended. You must also
// enable it at runtime with SLOWTEST=1.
#define MICROPATHER_DELTA_TEST
