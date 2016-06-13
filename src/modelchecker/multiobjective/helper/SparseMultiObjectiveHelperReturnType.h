#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEHELPERRETURNTYPE_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEHELPERRETURNTYPE_H_

#include <vector>
#include <memory>
#include <iomanip>
#include <boost/optional.hpp>

#include "src/storage/geometry/Polytope.h"
#include "src/storage/TotalScheduler.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template <typename RationalNumberType>
            struct SparseMultiObjectiveHelperReturnType {
                
                struct Iteration {
                    Iteration (std::vector<RationalNumberType> const& weightVector, std::vector<RationalNumberType> const& point, storm::storage::TotalScheduler const& scheduler) : weightVector(weightVector), point(point), scheduler(scheduler) {
                        //Intentionally left empty
                    }
                    
                    Iteration (std::vector<RationalNumberType>&& weightVector, std::vector<RationalNumberType>&& point, storm::storage::TotalScheduler&& scheduler) : weightVector(weightVector), point(point), scheduler(scheduler) {
                        //Intentionally left empty
                    }
                    
                    std::vector<RationalNumberType> weightVector;
                    std::vector<RationalNumberType> point;
                    storm::storage::TotalScheduler scheduler;
                }
                
                //Stores the results for the individual iterations
                std::vector<Iteration> iterations;
                
                //Stores an over/ under approximation of the set of achievable values
                std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>> overApproximation;
                std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>> underApproximation;

                // Stores the result of an achievability query (if applicable)
                // For a numerical query, stores whether there is one feasible solution
                boost::optional<bool> thresholdsAreAchievable;
                
                //Stores the result of a numerical query (if applicable)
                boost::optional<RationalNumberType> numericalResult;
                boost::optional<bool> optimumIsAchievable; //True if there is an actual scheduler that induces the computed supremum (i.e., supremum == maximum)
                
                //Stores the achieved precision for numerical and pareto queries
                boost::optional<RationalNumberType> precisionOfResult;
                
            };
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEHELPERRETURNTYPE_H_ */
