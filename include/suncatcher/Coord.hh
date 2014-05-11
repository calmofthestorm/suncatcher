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

#ifndef COORD_c0afad281d7b4f03901d6c674d45841c
#define COORD_c0afad281d7b4f03901d6c674d45841c

#include "graph/EuclideanCoord.hh"
#include "graph/EuclideanCoordRange.hh"

namespace suncatcher {

// The type to use for coords. For the non-polymorhic API or to use
// PolymorphicEuclideanGraph and friends, choose EuclideanCoord. If you're
// using the polymorphic API with your own code, it can be any type that obeys
// sane value semantics. Best choices are integral types, pointers, etc.
//
// If you want to make a class: Note that coords are typically passed by value
// -- suggest using something like a pointer, integral type, etc. Must support
// ==, !=, and < (for std::map/set). Must also have std::hash specialized for
// it.
using Coord = graph::EuclideanCoord;

namespace pathfinder {

// Needed for the Euclidean graph.
using CoordRange = graph::EuclideanCoordRange;

}  // namespace pathfinder

}  // namespace suncatcher

#endif  /* COORD_c0afad281d7b4f03901d6c674d45841c */
