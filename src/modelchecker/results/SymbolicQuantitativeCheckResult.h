#ifndef STORM_MODELCHECKER_SYMBOLICQUANTITATIVECHECKRESULT_H_
#define STORM_MODELCHECKER_SYMBOLICQUANTITATIVECHECKRESULT_H_

#include "src/storage/dd/DdType.h"
#include "src/modelchecker/results/QuantitativeCheckResult.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType Type>
        class SymbolicQuantitativeCheckResult : public QuantitativeCheckResult {
        public:
            SymbolicQuantitativeCheckResult();
            SymbolicQuantitativeCheckResult(storm::dd::Dd<Type> const& values);
            
            SymbolicQuantitativeCheckResult(SymbolicQuantitativeCheckResult const& other) = default;
            SymbolicQuantitativeCheckResult& operator=(SymbolicQuantitativeCheckResult const& other) = default;
#ifndef WINDOWS
            SymbolicQuantitativeCheckResult(SymbolicQuantitativeCheckResult&& other) = default;
            SymbolicQuantitativeCheckResult& operator=(SymbolicQuantitativeCheckResult&& other) = default;
#endif
            
            virtual std::unique_ptr<CheckResult> compareAgainstBound(storm::logic::ComparisonType comparisonType, double bound) const override;
            
            virtual bool isSymbolic() const override;
            virtual bool isResultForAllStates() const override;
            
            virtual bool isSymbolicQuantitativeCheckResult() const override;
            
            storm::dd::Dd<Type> const& getValueVector() const;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
            virtual void filter(QualitativeCheckResult const& filter) override;
            
        private:
            // The values of the quantitative check result.
            storm::dd::Dd<Type> values;
        };
    }
}

#endif /* STORM_MODELCHECKER_SYMBOLICQUANTITATIVECHECKRESULT_H_ */