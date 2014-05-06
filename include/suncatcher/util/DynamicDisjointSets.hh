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

#ifndef DYNAMICDISJOINTSETS_aad82f09d61946839aa33f7c20060e92
#define DYNAMICDISJOINTSETS_aad82f09d61946839aa33f7c20060e92

#include <set>
#include <utility>
#include <vector>

#include <boost/functional/hash.hpp>

#include "suncatcher/util/UnionFind.hh"

namespace std {

template<typename T1, typename T2>
struct hash<std::pair<T1, T2>> {
  size_t operator () (const std::pair<T1, T2>& key) const {
    size_t seed = 0;
    boost::hash_combine(seed, key.first);
    boost::hash_combine(seed, key.second);
    return seed;
  }
};

}

namespace suncatcher {

namespace test {
  class DeltaMap;
}  // namespace test

namespace util {


// The DynamicDisjointSets is used to maintain the connections between
// components in a structure that is efficient to update.
//
// Internally, a union find is used to efficiently merge components, and
// potentially disconnect components trigger a regeneration of the union find.
//
// TODO: profile and replace UnionFind with raw disjoint sets if indicated. We
// could halve the ram usage and remove some unnecessary lookups.
// (we do two levels of handles, one here and one in the union find!)
// TODO: evaluate using flood fill instead of UF for partial rebuild semantics.
// Probably overkill. TLDR this class could be faster but I doubt it matters
// -- its worst ops are linear in number of components, which just won't get
// that big, and the linear ops aren't THAT frequent.
// TODO: evaluate boost multiindex and/or bimap for our state.
template <typename T>
class DynamicDisjointSets {
  public:
    // Adds a node with the given label to the graph with no adjacent nodes.
    // No effect if label exists.
    inline void add_component(T label);

    // Adds an edge between two labels, merging their equivalence classes.
    // Both labels must exist.
    inline void add_edge(T label1, T label2);

    // Remove all of the label's adjacent edges (but not it).
    inline void isolate_component(T label);

    // Get a representative label for the given label. All nodes in an
    // equivalence class will have the same representative label. Note that
    // adding or removing edges may change the representative for any/all
    // labels. Asserts label is valid.
    inline T lookup(T label) const;

    // Convenience function that returns whether two labels are equivalent.
    // Asserts labels are valid.
    inline bool equivalent(T label1, T label2) const;

  private:
    friend class test::DeltaMap;

    UnionFind<T> uf;
    std::set<std::pair<T, T>> edges;
};


}  // namespace util
}  // namespace suncatcher

#include "suncatcher/util/DynamicDisjointSets-inl.hh"

#endif  /* DYNAMICDISJOINTSETS_aad82f09d61946839aa33f7c20060e92 */
