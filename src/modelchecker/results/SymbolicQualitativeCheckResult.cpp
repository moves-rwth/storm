#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidOperationException.h"

namespace storm {
    namespace modelchecker {
        template <storm::dd::DdType Type>
        SymbolicQualitativeCheckResult<Type>::SymbolicQualitativeCheckResult(storm::dd::Bdd<Type> const& reachableStates, storm::dd::Bdd<Type> const& truthValues) : reachableStates(reachableStates), truthValues(truthValues) {
            // Intentionally left empty.
        }
        
        template <storm::dd::DdType Type>
        bool SymbolicQualitativeCheckResult<Type>::isSymbolic() const {
            return true;
        }

        template <storm::dd::DdType Type>
        bool SymbolicQualitativeCheckResult<Type>::isResultForAllStates() const {
            return true;
        }
        
        template <storm::dd::DdType Type>
        bool SymbolicQualitativeCheckResult<Type>::isSymbolicQualitativeCheckResult() const {
            return true;
        }
        
        template <storm::dd::DdType Type>
        QualitativeCheckResult& SymbolicQualitativeCheckResult<Type>::operator&=(QualitativeCheckResult const& other) {
            STORM_LOG_THROW(other.isSymbolicQualitativeCheckResult(), storm::exceptions::InvalidOperationException, "Cannot perform logical 'and' on check results of incompatible type.");
            this->truthValues &= other.asSymbolicQualitativeCheckResult<Type>().getTruthValuesVector();
            return *this;
        }

        template <storm::dd::DdType Type>
        QualitativeCheckResult& SymbolicQualitativeCheckResult<Type>::operator|=(QualitativeCheckResult const& other) {
            STORM_LOG_THROW(other.isSymbolicQualitativeCheckResult(), storm::exceptions::InvalidOperationException, "Cannot perform logical 'and' on check results of incompatible type.");
            this->truthValues |= other.asSymbolicQualitativeCheckResult<Type>().getTruthValuesVector();
            return *this;
        }

        template <storm::dd::DdType Type>
        void SymbolicQualitativeCheckResult<Type>::complement() {
            this->truthValues = !this->truthValues && reachableStates;
        }
        
        template <storm::dd::DdType Type>
        storm::dd::Bdd<Type> const& SymbolicQualitativeCheckResult<Type>::getTruthValuesVector() const {
            return truthValues;
        }
        
        template <storm::dd::DdType Type>
        std::ostream& SymbolicQualitativeCheckResult<Type>::writeToStream(std::ostream& out) const {
            if (this->truthValues.isZero()) {
                out << "[false]" << std::endl;
            } else {
                out << "[true]" << std::endl;
            }
            return out;
        }
        
        template <storm::dd::DdType Type>
        void SymbolicQualitativeCheckResult<Type>::filter(QualitativeCheckResult const& filter) {
            STORM_LOG_THROW(filter.isSymbolicQualitativeCheckResult(), storm::exceptions::InvalidOperationException, "Cannot filter symbolic check result with non-symbolic filter.");
            this->truthValues &= filter.asSymbolicQualitativeCheckResult<Type>().getTruthValuesVector();
        }
        
        template class SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>;
        template class SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>;
    }
}