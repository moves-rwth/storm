#ifndef STORM_MODELCHECKER_HYBRIDQUANTITATIVECHECKRESULT_H_
#define STORM_MODELCHECKER_HYBRIDQUANTITATIVECHECKRESULT_H_

#include "src/storage/dd/DdType.h"
#include "src/storage/dd/Add.h"
#include "src/storage/dd/Bdd.h"
#include "src/storage/dd/cudd/CuddOdd.h"
#include "src/modelchecker/results/QuantitativeCheckResult.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType Type, typename ValueType = double>
        class HybridQuantitativeCheckResult : public QuantitativeCheckResult {
        public:
            HybridQuantitativeCheckResult() = default;
            HybridQuantitativeCheckResult(storm::dd::Bdd<Type> const& reachableStates, storm::dd::Bdd<Type> const& symbolicStates, storm::dd::Add<Type> const& symbolicValues, storm::dd::Bdd<Type> const& explicitStates, storm::dd::Odd<Type> const& odd, std::vector<double> const& explicitValues);
            
            HybridQuantitativeCheckResult(HybridQuantitativeCheckResult const& other) = default;
            HybridQuantitativeCheckResult& operator=(HybridQuantitativeCheckResult const& other) = default;
#ifndef WINDOWS
            HybridQuantitativeCheckResult(HybridQuantitativeCheckResult&& other) = default;
            HybridQuantitativeCheckResult& operator=(HybridQuantitativeCheckResult&& other) = default;
#endif
            
            virtual std::unique_ptr<CheckResult> compareAgainstBound(storm::logic::ComparisonType comparisonType, double bound) const override;
            
            std::unique_ptr<CheckResult> toExplicitQuantitativeCheckResult() const;
            
            virtual bool isHybrid() const override;
            virtual bool isResultForAllStates() const override;
            
            virtual bool isHybridQuantitativeCheckResult() const override;
            
            storm::dd::Bdd<Type> const& getSymbolicStates() const;
            
            storm::dd::Add<Type, ValueType> const& getSymbolicValueVector() const;
            
            storm::dd::Bdd<Type> const& getExplicitStates() const;
            
            storm::dd::Odd<Type> const& getOdd() const;
            
            std::vector<ValueType> const& getExplicitValueVector() const;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
            virtual void filter(QualitativeCheckResult const& filter) override;
            
            virtual ValueType getMin() const;

            virtual ValueType getMax() const;
            
        private:
            // The set of all reachable states.
            storm::dd::Bdd<Type> reachableStates;
            
            // The set of all states whose result is stored symbolically.
            storm::dd::Bdd<Type> symbolicStates;

            // The symbolic value vector.
            storm::dd::Add<Type, ValueType> symbolicValues;
            
            // The set of all states whose result is stored explicitly.
            storm::dd::Bdd<Type> explicitStates;
            
            // The ODD that enables translation of the explicit values to a symbolic format.
            storm::dd::Odd<Type> odd;

            // The explicit value vector.
            std::vector<ValueType> explicitValues;
        };
    }
}

#endif /* STORM_MODELCHECKER_HYBRIDQUANTITATIVECHECKRESULT_H_ */