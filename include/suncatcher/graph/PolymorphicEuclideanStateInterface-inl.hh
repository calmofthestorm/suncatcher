#ifndef POLYMORPHICEUCLIDEANSTATEINTERFACE_INL_e4510bb1d8584fdfbc9528db57786089
#define POLYMORPHICEUCLIDEANSTATEINTERFACE_INL_e4510bb1d8584fdfbc9528db57786089

namespace suncatcher {
namespace graph {


PolymorphicEuclideanStateInterface::PolymorphicEuclideanStateInterface(EuclideanCoord size)
: impl(size) { }


bool PolymorphicEuclideanStateInterface::get_expanded(EuclideanCoord cell) const {
  return impl.get_expanded(cell);
}


void PolymorphicEuclideanStateInterface::set_expanded(EuclideanCoord cell, bool value) {
  impl.set_expanded(cell, value);
}


float PolymorphicEuclideanStateInterface::get_distance(EuclideanCoord cell) const {
  return impl.get_distance(cell);
}


void PolymorphicEuclideanStateInterface::set_distance(EuclideanCoord cell, float value) {
  impl.set_distance(cell, value);
}


EuclideanCoord PolymorphicEuclideanStateInterface::get_previous(EuclideanCoord cell) const {
  return impl.get_previous(cell);
}


void PolymorphicEuclideanStateInterface::set_previous(EuclideanCoord cell, EuclideanCoord value) {
  impl.set_previous(cell, value);
}


void PolymorphicEuclideanStateInterface::clear() {
  impl.clear();
}




}  // namespace graph
}  // namespace suncatcher



#endif  /* POLYMORPHICEUCLIDEANSTATEINTERFACE_INL_e4510bb1d8584fdfbc9528db57786089 */
