#include "storm/modelchecker/results/QuantitativeCheckResult.h"

#include "storm-config.h"
#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/exceptions/InvalidOperationException.h"
#include "storm/utility/macros.h"

namespace storm {
namespace modelchecker {

template<typename ValueType>
std::unique_ptr<CheckResult> QuantitativeCheckResult<ValueType>::compareAgainstBound(storm::logic::ComparisonType, ValueType const&) const {
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
}  // namespace modelchecker
}  // namespace storm
