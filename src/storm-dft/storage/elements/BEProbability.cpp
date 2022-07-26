#include "BEProbability.h"

namespace storm::dft {
namespace storage {
namespace elements {

template<typename ValueType>
ValueType BEProbability<ValueType>::getUnreliability(ValueType time) const {
    return this->activeFailureProbability();
}

// Explicitly instantiate the class.
template class BEProbability<double>;
template class BEProbability<RationalFunction>;

}  // namespace elements
}  // namespace storage
}  // namespace storm::dft
