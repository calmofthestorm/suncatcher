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

#ifndef DYNAMICDISJOINTSETS_INL_0fd023f8d85844849cae7e21ed212731
#define DYNAMICDISJOINTSETS_INL_0fd023f8d85844849cae7e21ed212731

namespace suncatcher {
namespace util {


template <typename T>
inline void DynamicDisjointSets<T>::add_component(T label) {
  uf[label];
}


template <typename T>
inline void DynamicDisjointSets<T>::add_edge(T label1, T label2) {
  edges.insert({label1, label2});
  uf.union_sets(label1, label2);
}


//TODO: make not need 2 linear scan; cando in 1.
template <typename T>
inline void DynamicDisjointSets<T>::isolate_component(T label) {
  uf.disconnect_all_sets();
  auto it = edges.begin();
  while (it != edges.end()) {
    uf[it->first];
    uf[it->second];
    if (it->first == label || it->second == label) {
      edges.erase(it++);
    } else {
      uf.union_sets(it->first, it->second);
      ++it;
    }
  }
}


template <typename T>
inline T DynamicDisjointSets<T>::lookup(T label) const {
  return uf.at(label);
}


template <typename T>
inline bool DynamicDisjointSets<T>::equivalent(T label1, T label2) const {
  return (uf.at(label1) == uf.at(label2));
}


}  // namespace util
}  // namespace suncatcher

#endif  /* DYNAMICDISJOINTSETS_INL_0fd023f8d85844849cae7e21ed212731 */
