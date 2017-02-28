#ifndef STORM_MODELCHECKER_HINTS_EXPLICITMODELCHECKERHINT_H
#define STORM_MODELCHECKER_HINTS_EXPLICITMODELCHECKERHINT_H

#include <vector>
#include <boost/optional.hpp>

#include "storm/modelchecker/hints/ModelCheckerHint.h"
#include "storm/storage/TotalScheduler.h"

namespace storm {
    namespace modelchecker {
        
        template<typename ValueType>
        class ExplicitModelCheckerHint : public ModelCheckerHint {
        public:
            
            ExplicitModelCheckerHint(ExplicitModelCheckerHint<ValueType> const& other) = default;
            ExplicitModelCheckerHint(ExplicitModelCheckerHint<ValueType>&& other) = default;
            ExplicitModelCheckerHint(boost::optional<std::vector<ValueType>> const& resultHint = boost::none, boost::optional<storm::storage::TotalScheduler> const& schedulerHint = boost::none);
            ExplicitModelCheckerHint(boost::optional<std::vector<ValueType>>&& resultHint, boost::optional<storm::storage::TotalScheduler>&& schedulerHint = boost::none);
            
            // Returns true iff this hint does not contain any information
            virtual bool isEmpty() const override;
            
            // Returns true iff this is an explicit model checker hint
            virtual bool isExplicitModelCheckerHint() const override;
            
            bool hasResultHint() const;
            std::vector<ValueType> const& getResultHint() const;
            std::vector<ValueType>& getResultHint();
            void setResultHint(boost::optional<std::vector<ValueType>> const& resultHint);
            void setResultHint(boost::optional<std::vector<ValueType>>&& resultHint);
    
            bool hasSchedulerHint() const;
            storm::storage::TotalScheduler const& getSchedulerHint() const;
            storm::storage::TotalScheduler& getSchedulerHint();
            void setSchedulerHint(boost::optional<storage::TotalScheduler> const& schedulerHint);
            void setSchedulerHint(boost::optional<storage::TotalScheduler>&& schedulerHint);
            
        private:
            boost::optional<std::vector<ValueType>> resultHint;
            boost::optional<storm::storage::TotalScheduler> schedulerHint;
        };
        
    }
}

#endif /* STORM_MODELCHECKER_HINTS_EXPLICITMODELCHECKERHINT_H */
