#include "src/modelchecker/results/QuantitativeCheckResult.h"

#include "storm-config.h"
#include "src/adapters/CarlAdapter.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidOperationException.h"

namespace storm {
    namespace modelchecker {

        template<typename ValueType>
        std::unique_ptr<CheckResult> QuantitativeCheckResult<ValueType>::compareAgainstBound(storm::logic::ComparisonType comparisonType, ValueType const& bound) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Unable to perform comparison against bound on the check result.");
        }

        template<typename ValueType>
        bool QuantitativeCheckResult<ValueType>::isQuantitative() const {
            return true;
        }

        template class QuantitativeCheckResult<double>;

#ifdef STORM_HAVE_CARL
        template class QuantitativeCheckResult<RationalNumber>;
        template class QuantitativeCheckResult<RationalFunction>;
#endif
    }
}
