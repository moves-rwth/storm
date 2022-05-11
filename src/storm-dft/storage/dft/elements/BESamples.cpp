#include "BESamples.h"

#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
namespace storage {

template<typename ValueType>
ValueType BESamples<ValueType>::getUnreliability(ValueType time) const {
    auto iter = mActiveSamples.find(time);
    STORM_LOG_THROW(iter != mActiveSamples.end(), storm::exceptions::InvalidArgumentException, "No sample for time point " << time << " given.");
    return iter->second;
}

// Explicitly instantiate the class.
template class BESamples<double>;
template class BESamples<RationalFunction>;

}  // namespace storage
}  // namespace storm
