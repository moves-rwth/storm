#include "src/modelchecker/results/SymbolicQuantitativeCheckResult.h"
#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"

#include "src/storage/dd/CuddDd.h"
#include "src/exceptions/InvalidOperationException.h"

namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType Type>
        SymbolicQuantitativeCheckResult<Type>::SymbolicQuantitativeCheckResult(storm::dd::Dd<Type> const& allStates, storm::dd::Dd<Type> const& values) : allStates(allStates), values(values) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType Type>
        std::unique_ptr<CheckResult> SymbolicQuantitativeCheckResult<Type>::compareAgainstBound(storm::logic::ComparisonType comparisonType, double bound) const {
            std::unique_ptr<SymbolicQualitativeCheckResult<Type>> result;
            if (comparisonType == storm::logic::ComparisonType::Less || comparisonType == storm::logic::ComparisonType::GreaterEqual) {
                result = std::unique_ptr<SymbolicQualitativeCheckResult<Type>>(new SymbolicQualitativeCheckResult<Type>(allStates, values.greaterOrEqual(bound)));
            } else {
                result = std::unique_ptr<SymbolicQualitativeCheckResult<Type>>(new SymbolicQualitativeCheckResult<Type>(allStates, values.greater(bound)));
            }
            if (comparisonType == storm::logic::ComparisonType::Less || comparisonType == storm::logic::ComparisonType::LessEqual) {
                result->complement();
            }
            return result;
        }
        
        template<storm::dd::DdType Type>
        bool SymbolicQuantitativeCheckResult<Type>::isSymbolic() const {
            return true;
        }

        template<storm::dd::DdType Type>
        bool SymbolicQuantitativeCheckResult<Type>::isResultForAllStates() const {
            return true;
        }
        
        template<storm::dd::DdType Type>
        bool SymbolicQuantitativeCheckResult<Type>::isSymbolicQuantitativeCheckResult() const {
            return true;
        }
        
        template<storm::dd::DdType Type>
        storm::dd::Dd<Type> const& SymbolicQuantitativeCheckResult<Type>::getValueVector() const {
            return values;
        }
        
        template<storm::dd::DdType Type>
        std::ostream& SymbolicQuantitativeCheckResult<Type>::writeToStream(std::ostream& out) const {
            out << "[";
            bool first = true;
            for (auto valuationValuePair : this->values) {
                if (!first) {
                    out << ", ";
                } else {
                    first = false;
                }
                out << valuationValuePair.second;
            }
            out << "]";
            return out;
        }
        
        template<storm::dd::DdType Type>
        void SymbolicQuantitativeCheckResult<Type>::filter(QualitativeCheckResult const& filter) {
            STORM_LOG_THROW(filter.isSymbolicQualitativeCheckResult(), storm::exceptions::InvalidOperationException, "Cannot filter symbolic check result with non-symbolic filter.");
            this->truthValues *= filter.asSymbolicQualitativeCheckResult<Type>().getTruthValuesVector().toMtbdd();
        }
    }
}