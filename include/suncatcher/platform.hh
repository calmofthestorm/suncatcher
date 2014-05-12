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
// If you wish, you can use the second form of path and implement a PatherState
// to make more efficient the maintenance of open and closed sets. The default
// is hash tables.
//
// If you can, strongly consider avoiding the polymorphic API if your model
// can be construed as a grid of up to 3 dimensions.
// #define POLYMORPHIC_API

// Some tests (and the repl) are able to delta against micropather for sanity.
// This will make tests run a bit slower, but is recommended. You must also
// enable it at runtime with SLOWTEST=1.
#define MICROPATHER_DELTA_TEST

// If you need to customize the type used to hold Coordinates, see Coord.hh

// Comment out to disable copy on write for graphs. Typically not super
// expensive, and saves memory, but does add a layer of indirection and
// refcounting. The larger the maps, and the more frequent mutatons are, the
// better this is. I see roughly 20% slower pathfinding, 5x slower map loads
// (IE, from disk/your program), but really fast mutations.
//
// This setting only affects the Euclidean graph (polymorphic or not). If using
// the polymorphic API with your own graph, you can choose to make lazy_clone
// CoW (or not) regardless of this setting.
//
// (TODO: update benchmarks with legit bootstrap)
#define EUCLIDEAN_GRID_USE_COW
