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
        
        std::ostream& ExplicitQualitativeCheckResult::writeToStream(std::ostream& out, storm::storage::BitVector const& filter) const {
            std::ios::fmtflags oldflags(std::cout.flags());
            
            out << "[";
            storm::storage::BitVector::const_iterator it = filter.begin();
            storm::storage::BitVector::const_iterator itPlusOne = filter.begin();
            ++itPlusOne;
            storm::storage::BitVector::const_iterator ite = filter.end();
            
            out << std::boolalpha;
            for (; it != ite; ++itPlusOne, ++it) {
                out << truthValues[*it];
                if (itPlusOne != ite) {
                    out << ", ";
                }
            }
            out << "]";
            
            std::cout.flags(oldflags);
            return out;
        }
    }
}