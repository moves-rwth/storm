#pragma once

#include <vector>

#include "storm/modelchecker/multiobjective/deterministicScheds/MultiObjectiveSchedulerEvaluator.h"

namespace storm {
    
    class Environment;
    
    namespace modelchecker {
        namespace multiobjective {
            
            template <typename ModelType>
            class DetSchedsWeightVectorChecker {
            public:
                
                typedef typename ModelType::ValueType ValueType;

                DetSchedsWeightVectorChecker(std::shared_ptr<MultiObjectiveSchedulerEvaluator<ModelType>> const& schedulerEvaluator);

                /*!
                 * Optimizes the objectives in the given direction.
                 * Returns a sequence of points such that all points are achievable and the last point is the farest point in the given direction.
                 * After calling this, getResultForAllStates and getScheduler yield results with respect to that last point.
                 */
                std::vector<std::vector<ValueType>> check(Environment const& env, std::vector<ValueType> const& weightVector);
                
                std::vector<ValueType> const& getResultForAllStates(uint64_t objIndex) const;
                std::vector<uint64_t> const& getScheduler() const;

            private:
                std::shared_ptr<MultiObjectiveSchedulerEvaluator<ModelType>> schedulerEvaluator;

            };
            
        }
    }
}