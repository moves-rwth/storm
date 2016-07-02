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
                SparseMultiObjectiveRefinementStep(std::vector<RationalNumberType> const& weightVector, std::vector<RationalNumberType> const& lowerBoundPoint, std::vector<RationalNumberType> const& upperBoundPoint, storm::storage::TotalScheduler const& scheduler) : weightVector(weightVector), lowerBoundPoint(lowerBoundPoint), upperBoundPoint(upperBoundPoint), scheduler(scheduler) {
                    //Intentionally left empty
                }
                
                SparseMultiObjectiveRefinementStep(std::vector<RationalNumberType>&& weightVector, std::vector<RationalNumberType>&& lowerBoundPoint, std::vector<RationalNumberType>&& upperBoundPoint, storm::storage::TotalScheduler&& scheduler) : weightVector(weightVector), lowerBoundPoint(lowerBoundPoint), upperBoundPoint(upperBoundPoint), scheduler(scheduler) {
                    //Intentionally left empty
                }
                
                SparseMultiObjectiveRefinementStep(std::vector<RationalNumberType> const& weightVector, std::vector<RationalNumberType> const& lowerBoundPoint, std::vector<RationalNumberType> const& upperBoundPoint) : weightVector(weightVector), lowerBoundPoint(lowerBoundPoint), upperBoundPoint(upperBoundPoint) {
                    //Intentionally left empty
                }
                
                SparseMultiObjectiveRefinementStep(std::vector<RationalNumberType>&& weightVector, std::vector<RationalNumberType>&& lowerBoundPoint, std::vector<RationalNumberType>&& upperBoundPoint) : weightVector(weightVector), lowerBoundPoint(lowerBoundPoint), upperBoundPoint(upperBoundPoint) {
                    //Intentionally left empty
                }
                
                std::vector<RationalNumberType> const& getWeightVector() const {
                    return weightVector;
                }
                
                std::vector<RationalNumberType> const& getLowerBoundPoint() const {
                    return lowerBoundPoint;
                }
                
                std::vector<RationalNumberType> const& getUpperBoundPoint() const {
                    return upperBoundPoint;
                }
                
                bool hasScheduler() const {
                    return static_cast<bool>(scheduler);
                }
                
                storm::storage::TotalScheduler const& getScheduler() const {
                    return scheduler.get();
                }
                
            private:
                std::vector<RationalNumberType> const weightVector;
                std::vector<RationalNumberType> const lowerBoundPoint;
                std::vector<RationalNumberType> const upperBoundPoint;
                boost::optional<storm::storage::TotalScheduler> const scheduler;
            };
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEREFINEMENTSTEP_H_ */
