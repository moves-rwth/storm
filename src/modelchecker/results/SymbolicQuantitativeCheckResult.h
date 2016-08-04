#ifndef STORM_MODELCHECKER_SYMBOLICQUANTITATIVECHECKRESULT_H_
#define STORM_MODELCHECKER_SYMBOLICQUANTITATIVECHECKRESULT_H_

#include "src/storage/dd/DdType.h"
#include "src/storage/dd/Add.h"
#include "src/modelchecker/results/QuantitativeCheckResult.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType Type, typename ValueType = double>
        class SymbolicQuantitativeCheckResult : public QuantitativeCheckResult<ValueType> {
        public:
            SymbolicQuantitativeCheckResult() = default;
            SymbolicQuantitativeCheckResult(storm::dd::Bdd<Type> const& reachableStates, storm::dd::Add<Type, ValueType> const& values);
            
            SymbolicQuantitativeCheckResult(SymbolicQuantitativeCheckResult const& other) = default;
            SymbolicQuantitativeCheckResult& operator=(SymbolicQuantitativeCheckResult const& other) = default;
#ifndef WINDOWS
            SymbolicQuantitativeCheckResult(SymbolicQuantitativeCheckResult&& other) = default;
            SymbolicQuantitativeCheckResult& operator=(SymbolicQuantitativeCheckResult&& other) = default;
#endif
            
            virtual std::unique_ptr<CheckResult> compareAgainstBound(storm::logic::ComparisonType comparisonType, ValueType const& bound) const override;


            virtual bool isSymbolic() const override;
            virtual bool isResultForAllStates() const override;
            
            virtual bool isSymbolicQuantitativeCheckResult() const override;
            
            storm::dd::Add<Type, ValueType> const& getValueVector() const;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
            virtual void filter(QualitativeCheckResult const& filter) override;
            
            virtual ValueType getMin() const;
            
            virtual ValueType getMax() const;
            
            virtual void oneMinus() override;
            
        private:
            // The set of all reachable states.
            storm::dd::Bdd<Type> reachableStates;
            
            // The set of states for which this check result contains values.
            storm::dd::Bdd<Type> states;
            
            // The values of the quantitative check result.
            storm::dd::Add<Type, ValueType> values;
        };
    }
}

#endif /* STORM_MODELCHECKER_SYMBOLICQUANTITATIVECHECKRESULT_H_ */