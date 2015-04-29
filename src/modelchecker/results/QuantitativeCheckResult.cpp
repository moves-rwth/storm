#include "src/modelchecker/results/QuantitativeCheckResult.h"

#include "storm-config.h"
#include "src/adapters/CarlAdapter.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidOperationException.h"

namespace storm {
    namespace modelchecker {
        std::unique_ptr<CheckResult> QuantitativeCheckResult::compareAgainstBound(storm::logic::ComparisonType comparisonType, double bound) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Unable to perform comparison against bound on the check result.");
        }
        
        bool QuantitativeCheckResult::isQuantitative() const {
            return true;
        }
    }
}