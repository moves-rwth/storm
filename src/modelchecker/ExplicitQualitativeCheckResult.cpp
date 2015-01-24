#include "src/modelchecker/ExplicitQualitativeCheckResult.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidOperationException.h"

namespace storm {
    namespace modelchecker {
        CheckResult& ExplicitQualitativeCheckResult::operator&=(CheckResult const& other) {
            STORM_LOG_THROW(typeid(other) == typeid(ExplicitQualitativeCheckResult), storm::exceptions::InvalidOperationException, "Cannot perform logical 'and' on check results of incompatible type.");
            ExplicitQualitativeCheckResult const& otherCheckResult = static_cast<ExplicitQualitativeCheckResult const&>(other);
            this->truthValues &= otherCheckResult.truthValues;
            return *this;
        }
        
        CheckResult& ExplicitQualitativeCheckResult::operator|=(CheckResult const& other) {
            STORM_LOG_THROW(typeid(other) == typeid(ExplicitQualitativeCheckResult), storm::exceptions::InvalidOperationException, "Cannot perform logical 'or' on check results of incompatible type.");
            ExplicitQualitativeCheckResult const& otherCheckResult = static_cast<ExplicitQualitativeCheckResult const&>(other);
            this->truthValues |= otherCheckResult.truthValues;
            return *this;
        }
        
        bool ExplicitQualitativeCheckResult::operator[](uint_fast64_t index) const {
            return truthValues.get(index);
        }
        
        storm::storage::BitVector const& ExplicitQualitativeCheckResult::getTruthValues() const {
            return truthValues;
        }
        
        void ExplicitQualitativeCheckResult::complement() {
            truthValues.complement();
        }
        
        bool ExplicitQualitativeCheckResult::isExplicit() const {
            return true;
        }
        
        bool ExplicitQualitativeCheckResult::isResultForAllStates() const {
            return true;
        }
        
        bool ExplicitQualitativeCheckResult::isExplicitQualitativeCheckResult() const {
            return true;
        }
        
        std::ostream& ExplicitQualitativeCheckResult::writeToStream(std::ostream& out) const {
            out << truthValues;
            return out;
        }
    }
}