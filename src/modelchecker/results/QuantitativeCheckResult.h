#ifndef STORM_MODELCHECKER_QUANTITATIVECHECKRESULT_H_
#define STORM_MODELCHECKER_QUANTITATIVECHECKRESULT_H_

#include "src/modelchecker/results/CheckResult.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        class QuantitativeCheckResult : public CheckResult {
        public:
            virtual ~QuantitativeCheckResult() = default;
            
            virtual std::unique_ptr<CheckResult> compareAgainstBound(storm::logic::ComparisonType comparisonType, ValueType const& bound) const;
            
            virtual void oneMinus() = 0;
            
            virtual bool isQuantitative() const override;
        };
    }
}

#endif /* STORM_MODELCHECKER_QUANTITATIVECHECKRESULT_H_ */