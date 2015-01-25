#include "src/modelchecker/CheckResult.h"

#include "src/modelchecker/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/ExplicitQuantitativeCheckResult.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidOperationException.h"

namespace storm {
    namespace modelchecker {
        CheckResult& CheckResult::operator&=(CheckResult const& other) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Unable to perform logical 'and' on the two check results.");
        }
        
        CheckResult& CheckResult::operator|=(CheckResult const& other) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Unable to perform logical 'or' on the two check results.");
        }
        
        void CheckResult::complement() {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Unable to perform logical 'not' on the check result.");
        }
        
        std::unique_ptr<CheckResult> CheckResult::compareAgainstBound(storm::logic::ComparisonType comparisonType, double bound) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Unable to perform comparison against bound on the check result.");
        }
        
        bool CheckResult::isExplicit() const {
            return false;
        }
        
        bool CheckResult::isQuantitative() const {
            return false;
        }
        
        bool CheckResult::isQualitative() const {
            return false;
        }
        
        bool CheckResult::isResultForAllStates() const {
            return false;
        }
        
        std::ostream& operator<<(std::ostream& out, CheckResult& checkResult) {
            checkResult.writeToStream(out);
            return out;
        }
        
        bool CheckResult::isExplicitQualitativeCheckResult() const {
            return false;
        }
        
        bool CheckResult::isExplicitQuantitativeCheckResult() const {
            return false;
        }
        
        ExplicitQualitativeCheckResult& CheckResult::asExplicitQualitativeCheckResult() {
            return dynamic_cast<ExplicitQualitativeCheckResult&>(*this);
        }
        
        ExplicitQualitativeCheckResult const& CheckResult::asExplicitQualitativeCheckResult() const {
            return dynamic_cast<ExplicitQualitativeCheckResult const&>(*this);
        }
        
        template<typename ValueType>
        ExplicitQuantitativeCheckResult<ValueType>& CheckResult::asExplicitQuantitativeCheckResult() {
            return dynamic_cast<ExplicitQuantitativeCheckResult<ValueType>&>(*this);
        }
        
        template<typename ValueType>
        ExplicitQuantitativeCheckResult<ValueType> const& CheckResult::asExplicitQuantitativeCheckResult() const {
            return dynamic_cast<ExplicitQuantitativeCheckResult<ValueType> const&>(*this);
        }
        
        // Explicitly instantiate the template functions.
        template ExplicitQuantitativeCheckResult<double>& CheckResult::asExplicitQuantitativeCheckResult();
        template ExplicitQuantitativeCheckResult<double> const& CheckResult::asExplicitQuantitativeCheckResult() const;
    }
}