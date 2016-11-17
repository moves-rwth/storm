#include "storm/modelchecker/results/SymbolicQuantitativeCheckResult.h"
#include "storm/modelchecker/results/SymbolicQualitativeCheckResult.h"

#include "storm/storage/dd/DdManager.h"
#include "storm/storage/dd/cudd/CuddAddIterator.h"

#include "storm/exceptions/InvalidOperationException.h"

#include "storm/exceptions/NotImplementedException.h"
#include "storm/utility/macros.h"
#include "storm/utility/constants.h"

namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType Type, typename ValueType>
        SymbolicQuantitativeCheckResult<Type, ValueType>::SymbolicQuantitativeCheckResult(storm::dd::Bdd<Type> const& reachableStates, storm::dd::Add<Type, ValueType> const& values) : reachableStates(reachableStates), states(reachableStates), values(values) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::unique_ptr<CheckResult> SymbolicQuantitativeCheckResult<Type, ValueType>::compareAgainstBound(storm::logic::ComparisonType comparisonType, ValueType const& bound) const {
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
            return std::unique_ptr<SymbolicQualitativeCheckResult<Type>>(new SymbolicQualitativeCheckResult<Type>(reachableStates, states));
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        bool SymbolicQuantitativeCheckResult<Type, ValueType>::isSymbolic() const {
            return true;
        }

        template<storm::dd::DdType Type, typename ValueType>
        bool SymbolicQuantitativeCheckResult<Type, ValueType>::isResultForAllStates() const {
            return states == reachableStates;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        bool SymbolicQuantitativeCheckResult<Type, ValueType>::isSymbolicQuantitativeCheckResult() const {
            return true;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        storm::dd::Add<Type, ValueType> const& SymbolicQuantitativeCheckResult<Type, ValueType>::getValueVector() const {
            return values;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::ostream& SymbolicQuantitativeCheckResult<Type, ValueType>::writeToStream(std::ostream& out) const {
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
        
        template<storm::dd::DdType Type, typename ValueType>
        void SymbolicQuantitativeCheckResult<Type, ValueType>::filter(QualitativeCheckResult const& filter) {
            STORM_LOG_THROW(filter.isSymbolicQualitativeCheckResult(), storm::exceptions::InvalidOperationException, "Cannot filter symbolic check result with non-symbolic filter.");
            this->states &= filter.asSymbolicQualitativeCheckResult<Type>().getTruthValuesVector();
            this->values *= filter.asSymbolicQualitativeCheckResult<Type>().getTruthValuesVector().template toAdd<ValueType>();
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        ValueType SymbolicQuantitativeCheckResult<Type, ValueType>::getMin() const {
            // In order to not get false zeros, we need to set the values of all states whose values is not stored
            // symbolically to infinity.
            return states.ite(this->values, states.getDdManager().getConstant(storm::utility::infinity<double>())).getMin();
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        ValueType SymbolicQuantitativeCheckResult<Type, ValueType>::getMax() const {
            return this->values.getMax();
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        ValueType SymbolicQuantitativeCheckResult<Type, ValueType>::average() const {
            return this->sum() / this->states.getNonZeroCount();
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        ValueType SymbolicQuantitativeCheckResult<Type, ValueType>::sum() const {
            return this->values.sumAbstract(this->values.getContainedMetaVariables()).getValue();
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        void SymbolicQuantitativeCheckResult<Type, ValueType>::oneMinus() {
            storm::dd::Add<Type> one = values.getDdManager().template getAddOne<ValueType>();
            values = one - values;
        }
        
        // Explicitly instantiate the class.
        template class SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD>;
        template class SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan>;
    }
}
