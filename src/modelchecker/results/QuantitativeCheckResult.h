#ifndef STORM_MODELCHECKER_QUANTITATIVECHECKRESULT_H_
#define STORM_MODELCHECKER_QUANTITATIVECHECKRESULT_H_

#include "src/modelchecker/results/CheckResult.h"

namespace storm {
    namespace modelchecker {
        class QuantitativeCheckResult : public CheckResult {
        public:
            virtual std::unique_ptr<CheckResult> compareAgainstBound(storm::logic::ComparisonType comparisonType, double bound) const;
            
            virtual bool isQuantitative() const override;
        };
    }
}

#endif /* STORM_MODELCHECKER_QUANTITATIVECHECKRESULT_H_ */