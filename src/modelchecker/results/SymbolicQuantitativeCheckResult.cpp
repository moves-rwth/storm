#include "src/modelchecker/results/SymbolicQuantitativeCheckResult.h"
#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"

#include "src/storage/dd/cudd/CuddDd.h"
#include "src/storage/dd/cudd/CuddDdManager.h"
#include "src/exceptions/InvalidOperationException.h"
#include "src/utility/macros.h"
#include "src/utility/constants.h"

namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType Type>
        SymbolicQuantitativeCheckResult<Type>::SymbolicQuantitativeCheckResult(storm::dd::Bdd<Type> const& reachableStates, storm::dd::Add<Type> const& values) : reachableStates(reachableStates), states(reachableStates), values(values) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType Type>
        std::unique_ptr<CheckResult> SymbolicQuantitativeCheckResult<Type>::compareAgainstBound(storm::logic::ComparisonType comparisonType, double bound) const {
            storm::dd::Bdd<Type> states;
            if (comparisonType == storm::logic::ComparisonType::Less) {
                states = values.less(bound);
            } else if (comparisonType == storm::logic::ComparisonType::LessEqual) {
                states = values.lessOrEqual(bound);
            } else if (comparisonType == storm::logic::ComparisonType::Greater) {
                states = values.greater(bound);
            } else if (comparisonType == storm::logic::ComparisonType::GreaterEqual) {
                states = values.greaterOrEqual(bound);
            }
            return std::unique_ptr<SymbolicQualitativeCheckResult<Type>>(new SymbolicQualitativeCheckResult<Type>(reachableStates, values.greaterOrEqual(bound)));;
        }
        
        template<storm::dd::DdType Type>
        bool SymbolicQuantitativeCheckResult<Type>::isSymbolic() const {
            return true;
        }

        template<storm::dd::DdType Type>
        bool SymbolicQuantitativeCheckResult<Type>::isResultForAllStates() const {
            return states == reachableStates;
        }
        
        template<storm::dd::DdType Type>
        bool SymbolicQuantitativeCheckResult<Type>::isSymbolicQuantitativeCheckResult() const {
            return true;
        }
        
        template<storm::dd::DdType Type>
        storm::dd::Add<Type> const& SymbolicQuantitativeCheckResult<Type>::getValueVector() const {
            return values;
        }
        
        template<storm::dd::DdType Type>
        std::ostream& SymbolicQuantitativeCheckResult<Type>::writeToStream(std::ostream& out) const {
            out << "[";
            if (this->values.isZero()) {
                out << "0";
            } else {
                bool first = true;
                for (auto valuationValuePair : this->values) {
                    if (!first) {
                        out << ", ";
                    } else {
                        first = false;
                    }
                    out << valuationValuePair.second;
                }
            }
            out << "]";
            return out;
        }
        
        template<storm::dd::DdType Type>
        void SymbolicQuantitativeCheckResult<Type>::filter(QualitativeCheckResult const& filter) {
            STORM_LOG_THROW(filter.isSymbolicQualitativeCheckResult(), storm::exceptions::InvalidOperationException, "Cannot filter symbolic check result with non-symbolic filter.");
            this->states &= filter.asSymbolicQualitativeCheckResult<Type>().getTruthValuesVector();
            this->values *= filter.asSymbolicQualitativeCheckResult<Type>().getTruthValuesVector().toAdd();
        }
        
        template<storm::dd::DdType Type>
        double SymbolicQuantitativeCheckResult<Type>::getMin() const {
            // In order to not get false zeros, we need to set the values of all states whose values is not stored
            // symbolically to infinity.
            return states.toAdd().ite(this->values, states.getDdManager()->getConstant(storm::utility::infinity<double>())).getMin();
        }
        
        template<storm::dd::DdType Type>
        double SymbolicQuantitativeCheckResult<Type>::getMax() const {
            return this->values.getMax();
        }
        
        // Explicitly instantiate the class.
        template class SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>;
    }
}