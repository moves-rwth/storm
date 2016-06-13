#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEWEIGHTVECTORCHECKER_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEWEIGHTVECTORCHECKER_H_

#include <vector>

#include "src/modelchecker/multiobjective/helper/SparseMultiObjectivePreprocessorReturnType.h"
#include "src/storage/TotalScheduler.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            /*!
             * Helper Class that takes preprocessed multi objective data and a weight vector and ...
             * - computes the maximal expected reward w.r.t. the weighted sum of the rewards of the individual objectives
             * - extracts the scheduler that induces this maximum
             * - computes for each objective the value induced by this scheduler
             */
            template <class SparseModelType>
            class SparseMultiObjectiveWeightVectorChecker {
            public:
                typedef typename SparseModelType::ValueType ValueType;
                typedef typename SparseModelType::RewardModelType RewardModelType;
                typedef SparseMultiObjectivePreprocessorReturnType<SparseModelType> PreprocessorData;
            
                SparseMultiObjectiveWeightVectorChecker(PreprocessorData const& data);
                
                /*!
                 * - computes the maximal expected reward w.r.t. the weighted sum of the rewards of the individual objectives
                 * - extracts the scheduler that induces this maximum
                 * - computes for each objective the value induced by this scheduler
                 */
                void check(std::vector<ValueType> const& weightVector);
                
                /*!
                 * Getter methods for the results of the most recent call of check(..)
                 * Note that check(..) has to be called before retrieving results. Otherwise, an exception is thrown.
                 */
                // The results of the individual objectives at the initial state of the given model
                template<typename TargetValueType = ValueType>
                std::vector<TargetValueType> getInitialStateResultOfObjectives() const;
                // A scheduler that induces the optimal values
                storm::storage::TotalScheduler const& getScheduler() const;
                
                
            private:
                
                /*!
                 * Determines the scheduler that maximizes the weighted reward vector of the unbounded objectives
                 *
                 * @param weightVector the weight vector of the current check
                 */
                void unboundedWeightedPhase(std::vector<ValueType> const& weightVector);
                
                /*!
                 * Computes the values of the objectives that do not have a stepBound w.r.t. the scheduler computed in the unboundedWeightedPhase
                 *
                 * @param weightVector the weight vector of the current check
                 */
                void unboundedIndividualPhase(std::vector<ValueType> const& weightVector);
                
                /*!
                 * For each time epoch (starting with the maximal stepBound occurring in the objectives), this method
                 * - determines the objectives that are relevant in the current time epoch
                 * - determines the maximizing scheduler for the weighted reward vector of these objectives
                 * - computes the values of these objectives w.r.t. this scheduler
                 *
                 * @param weightVector the weight vector of the current check
                 */
                void boundedPhase(std::vector<ValueType> const& weightVector);
                
                // stores the considered information of the multi-objective model checking problem
                PreprocessorData const& data;
                
                // becomes true after the first call of check(..)
                bool checkHasBeenCalled;
                
                // The result for the weighted reward vector (for all states of the model)
                std::vector<ValueType> weightedResult;
                // The results for the individual objectives (for all states of the model)
                std::vector<std::vector<ValueType>> objectiveResults;
                // The scheduler that maximizes the weighted rewards
                storm::storage::TotalScheduler scheduler;
                
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEWEIGHTEDVECTORCHECKER_H_ */
