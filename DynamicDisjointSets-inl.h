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
  ++weights[{label1, label2}];
}


template <typename T>
inline void DynamicDisjointSets<T>::remove_edge(T label1, T label2) {
  auto it = weights.find({label1, label2});
  assert(it != weights.end() && *it > 0);
  if (--(*it) == 0) {
    weights.erase(it);
  }
  this->rebuild();
}


template <typename T>
inline void DynamicDisjointSets<T>::remove_edges(const std::vector<std::pair<T, T>>& edges) {
  for (const auto& edge : edges) {
    auto it = weights.find(edge);
    assert(it != weights.end() && *it > 0);
    if (--(*it) == 0) {
      weights.erase(it);
    }
  }
  this->rebuild();
}


template <typename T>
inline T DynamicDisjointSets<T>::lookup(T label) const {
  return uf.at(label);
}


template <typename T>
inline bool DynamicDisjointSets<T>::equivalent(T label1, T label2) const {
  return (uf.at(label1) == uf.at(label2));
}


template <typename T>
void DynamicDisjointSets<T>::rebuild() {
  uf = UnionFind<T>();
  for (const auto& edge : weights) {
    if (edge.second > 0) {
      uf.union_sets(uf[edge.first], uf[edge.second]);
    }
  }
}



}  // namespace util
}  // namespace suncatcher

#endif  /* DYNAMICDISJOINTSETS_INL_0fd023f8d85844849cae7e21ed212731 */
