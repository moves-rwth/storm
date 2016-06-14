#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEREFINEMENTSTEP_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEREFINEMENTSTEP_H_

#include <vector>
#include <boost/optional.hpp>

#include "src/storage/TotalScheduler.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template <typename RationalNumberType>
            class SparseMultiObjectiveRefinementStep {
                
            public:
                SparseMultiObjectiveRefinementStep(std::vector<RationalNumberType> const& weightVector, std::vector<RationalNumberType> const& point, storm::storage::TotalScheduler const& scheduler) : weightVector(weightVector), point(point), scheduler(scheduler) {
                    //Intentionally left empty
                }
                
                SparseMultiObjectiveRefinementStep(std::vector<RationalNumberType>&& weightVector, std::vector<RationalNumberType>&& point, storm::storage::TotalScheduler&& scheduler) : weightVector(weightVector), point(point), scheduler(scheduler) {
                    //Intentionally left empty
                }
                
                SparseMultiObjectiveRefinementStep(std::vector<RationalNumberType> const& weightVector, std::vector<RationalNumberType> const& point) : weightVector(weightVector), point(point) {
                    //Intentionally left empty
                }
                
                SparseMultiObjectiveRefinementStep(std::vector<RationalNumberType>&& weightVector, std::vector<RationalNumberType>&& point) : weightVector(weightVector), point(point) {
                    //Intentionally left empty
                }
                
                std::vector<RationalNumberType> const& getWeightVector() const {
                    return weightVector;
                }
                
                std::vector<RationalNumberType> const& getPoint() const {
                    return point;
                }
                
                bool hasScheduler() const {
                    return static_cast<bool>(scheduler);
                }
                
                storm::storage::TotalScheduler const& getScheduler() const {
                    return scheduler.get();
                }
                
            private:
                std::vector<RationalNumberType> const weightVector;
                std::vector<RationalNumberType> const point;
                boost::optional<storm::storage::TotalScheduler> const scheduler;
            };
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEREFINEMENTSTEP_H_ */
