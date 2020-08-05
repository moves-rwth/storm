#include "BEExponential.h"

#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace storage {

        template <>
        double BEExponential<double>::getUnreliability(double time) const {
            // 1 - e^(-lambda * t)
            return 1 - exp(-this->activeFailureRate() * time);
        }

        template <typename ValueType>
        ValueType BEExponential<ValueType>::getUnreliability(ValueType time) const {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Computing cumulative failure probability not supported for this data type.");
        }

        // Explicitly instantiate the class.
        template class BEExponential<double>;
        template class BEExponential<RationalFunction>;

    }
}
