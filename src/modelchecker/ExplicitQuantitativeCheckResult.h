#ifndef STORM_MODELCHECKER_EXPLICITQUANTITATIVECHECKRESULT_H_
#define STORM_MODELCHECKER_EXPLICITQUANTITATIVECHECKRESULT_H_

#include <vector>

#include "src/modelchecker/QuantitativeCheckResult.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        class ExplicitQuantitativeCheckResult : public QuantitativeCheckResult<ValueType> {
        public:
            /*!
             * Constructs a check result with the provided values.
             *
             * @param values The values of the result.
             */
            ExplicitQuantitativeCheckResult(std::vector<ValueType> const& values) : values(values) {
                // Intentionally left empty.
            }
            
            /*!
             * Constructs a check result with the provided values.
             *
             * @param values The values of the result.
             */
            ExplicitQuantitativeCheckResult(std::vector<ValueType>&& values) : values(std::move(values)) {
                // Intentionally left empty.
            }
            
            ExplicitQuantitativeCheckResult(ExplicitQuantitativeCheckResult const& other) = default;
            ExplicitQuantitativeCheckResult& operator=(ExplicitQuantitativeCheckResult const& other) = default;
#ifndef WINDOWS
            ExplicitQuantitativeCheckResult(ExplicitQuantitativeCheckResult&& other) = default;
            ExplicitQuantitativeCheckResult& operator=(ExplicitQuantitativeCheckResult&& other) = default;
#endif
            
            virtual std::unique_ptr<CheckResult> compareAgainstBound(storm::logic::ComparisonType comparisonType, double bound) const override;
            
            ValueType operator[](uint_fast64_t index) const;
            
            virtual bool isExplicit() const override;
            virtual bool isResultForAllStates() const override;
            
            virtual bool isExplicitQuantitativeCheckResult() const override;
            
            std::vector<ValueType> const& getValues() const;
            
        protected:
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
        private:
            // The values of the quantitative check result.
            std::vector<ValueType> values;
        };
    }
}

#endif /* STORM_MODELCHECKER_EXPLICITQUANTITATIVECHECKRESULT_H_ */