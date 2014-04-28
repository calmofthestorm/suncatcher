#ifndef DYNAMICDISJOINTSETS_aad82f09d61946839aa33f7c20060e92
#define DYNAMICDISJOINTSETS_aad82f09d61946839aa33f7c20060e92

#include <utility>
#include <vector>

#include <boost/functional/hash.hpp>

#include "UnionFind.h"

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
namespace util {

// The DynamicDisjointSets is used to maintain the connections between
// components in a structure that is efficient to update. Adding an edge
// between components will always be constant time. Removing an edge may in the
// worst case be linear in graph size.
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
template <typename T>
class DynamicDisjointSets {
  public:
    // Adds a node with the given label to the graph with no adjacent nodes.
    // No effect if label exists. Amortized constant time, though vector
    // resizes are involved so it's a slow constant time.
    inline void add_component(T label);

    // Adds an edge between two labels, merging their equivalence classes.
    // Edges are weighted, so adding an edge N times means it must be removed N
    // times to actually be gone. If either label does not exist, it's a no-op.
    // Amortized constant time.
    inline void add_edge(T label1, T label2);

    // Decrements an edge's weight by 1, removing it if the weight reaches 0.
    // Constant time if the edge is not removed, linear if it is. Asserts edge
    // weight is non-zero/edge exists.
    inline void remove_edge(T label1, T label2);
    inline void remove_edges(const std::vector<std::pair<T, T>>& edges);

    // Remove all of the label's adjacent edges (but not it). Linear.
    inline void isolate_component(T label);

    // Get a representative label for the given label. All nodes in an
    // equivalence class will have the same representative label. Note that
    // adding or removing edges may change the representative for any/all
    // labels. Asserts label is valid. Amortized constant time.
    inline T lookup(T label) const;

    // Convenience function that returns whether two labels are equivalent.
    // Asserts labels are valid. Amortized constant time.
    inline bool equivalent(T label1, T label2) const;

  private:
    void rebuild();

    // lookup needs to be const for const to mean anything in the scope of the
    // wider program. So here we are:/
    mutable UnionFind<T> uf;
    std::unordered_map<std::pair<T, T>, unsigned int> weights;
};

}  // namespace util
}  // namespace suncatcher

#include "DynamicDisjointSets-inl.h"

#endif  /* DYNAMICDISJOINTSETS_aad82f09d61946839aa33f7c20060e92 */
