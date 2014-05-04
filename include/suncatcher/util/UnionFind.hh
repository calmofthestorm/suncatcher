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

#ifndef UNIONFIND_0bb7b9ebeb3048f58090c09247502b31
#define UNIONFIND_0bb7b9ebeb3048f58090c09247502b31

#include <deque>
#include <unordered_map>
#include <vector>

#include <boost/pending/disjoint_sets.hpp>
#include <boost/iterator/counting_iterator.hpp>

namespace suncatcher {

namespace util {
  template <typename T>
  class UnionFind;
}  // namespace util

namespace test {
  template <typename T>
  const T& union_find_lookup_no_compress(const util::UnionFind<T>& uf, const T& elem);
}  // namespace test

namespace util {

template <typename T>
class UnionFind {
  public:
    UnionFind();
    UnionFind(std::initializer_list<typename std::deque<T>::value_type> l);
    UnionFind(const UnionFind& other);
    UnionFind(UnionFind&& other);
    UnionFind& operator=(const UnionFind& other);
    UnionFind& operator=(UnionFind&& other);

    // Get element, adding it as a singleton set if it does not exist.
    T& operator[] (const T& elem);
    T& operator[] (T&& elem);
    const T& operator[] (const T& elem) const;

    // Access element, throwing std::out_of_range if it does not exist.
    T& at (const T& elem);
    const T& at (const T& elem) const;

    // Union the two sets. Throws std::out_of_range if either does not exist.
    T& union_sets(const T& elem1, const T& elem2);

    // Get the number of SETS.
    size_t count_sets() const;

    void disconnect_all_sets();

    // Get the number of ELEMENTS.
    size_t count_elements() const;

    // Get a reference to the elements in the union find (Handy for iteration).
    inline const std::deque<T>& get_elements() const { return id_to_handle; }

  private:
    friend const T& test::union_find_lookup_no_compress<T>(const UnionFind<T>& uf, const T& elem);
    mutable std::vector<size_t> rank, parent;
    std::deque<T> id_to_handle;
    std::unordered_map<T, size_t> handle_to_id;
    mutable boost::disjoint_sets<size_t*, size_t*> uf;

    bool grow();
};

}  // namespace suncatcher
}  // namespace util

#include "suncatcher/util/UnionFind-inl.hh"

#endif  /* UNIONFIND_0bb7b9ebeb3048f58090c09247502b31 */
