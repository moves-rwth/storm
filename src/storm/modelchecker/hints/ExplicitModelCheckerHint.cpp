#include "storm/modelchecker/hints/ExplicitModelCheckerHint.h"
#include "storm/adapters/CarlAdapter.h"
namespace storm {
    namespace modelchecker {
           
        template<typename ValueType>
        ExplicitModelCheckerHint<ValueType>::ExplicitModelCheckerHint(boost::optional<std::vector<ValueType>> const& resultHint, boost::optional<storm::storage::TotalScheduler> const& schedulerHint) : resultHint(resultHint), schedulerHint(schedulerHint), forceApplicationOfHints(false) {
            // Intentionally left empty
        }
        
        template<typename ValueType>
        ExplicitModelCheckerHint<ValueType>::ExplicitModelCheckerHint(boost::optional<std::vector<ValueType>>&& resultHint, boost::optional<storm::storage::TotalScheduler>&& schedulerHint) : resultHint(resultHint), schedulerHint(schedulerHint), forceApplicationOfHints(false) {
            // Intentionally left empty
        }
        
        template<typename ValueType>
        bool ExplicitModelCheckerHint<ValueType>::isEmpty() const {
            return !resultHint.is_initialized() && !schedulerHint.is_initialized();
        }
        
        template<typename ValueType>
        bool ExplicitModelCheckerHint<ValueType>::isExplicitModelCheckerHint() const {
            return true;
        }
    
        template<typename ValueType>
        bool ExplicitModelCheckerHint<ValueType>::hasResultHint() const {
            return resultHint.is_initialized();
        }
        
        template<typename ValueType>
        std::vector<ValueType> const& ExplicitModelCheckerHint<ValueType>::getResultHint() const {
            return *resultHint;
        }
        
        template<typename ValueType>
        std::vector<ValueType>& ExplicitModelCheckerHint<ValueType>::getResultHint() {
            return *resultHint;
        }
    
        template<typename ValueType>
        void ExplicitModelCheckerHint<ValueType>::setResultHint(boost::optional<std::vector<ValueType>> const& resultHint) {
            this->resultHint = resultHint;
        }
      
        template<typename ValueType>
        void ExplicitModelCheckerHint<ValueType>::setResultHint(boost::optional<std::vector<ValueType>>&& resultHint) {
            this->resultHint = resultHint;
        }
        
        template<typename ValueType>
        bool ExplicitModelCheckerHint<ValueType>::hasSchedulerHint() const {
            return schedulerHint.is_initialized();
        }
        
        template<typename ValueType>
        storm::storage::TotalScheduler const& ExplicitModelCheckerHint<ValueType>::getSchedulerHint() const {
            return *schedulerHint;
        }
        
        template<typename ValueType>
        storm::storage::TotalScheduler& ExplicitModelCheckerHint<ValueType>::getSchedulerHint() {
            return *schedulerHint;
        }
    
        template<typename ValueType>
        void ExplicitModelCheckerHint<ValueType>::setSchedulerHint(boost::optional<storm::storage::TotalScheduler> const& schedulerHint) {
            this->schedulerHint = schedulerHint;
        }
      
        template<typename ValueType>
        void ExplicitModelCheckerHint<ValueType>::setSchedulerHint(boost::optional<storm::storage::TotalScheduler>&& schedulerHint) {
            this->schedulerHint = schedulerHint;
        }
    
        template<typename ValueType>
        bool ExplicitModelCheckerHint<ValueType>::getForceApplicationOfHints() const {
            return forceApplicationOfHints;
        }
    
        template<typename ValueType>
        void ExplicitModelCheckerHint<ValueType>::setForceApplicationOfHints(bool value) {
            forceApplicationOfHints = value;
        }
    
        template class ExplicitModelCheckerHint<double>;
        template class ExplicitModelCheckerHint<storm::RationalNumber>;
        template class ExplicitModelCheckerHint<storm::RationalFunction>;
        
    }
}