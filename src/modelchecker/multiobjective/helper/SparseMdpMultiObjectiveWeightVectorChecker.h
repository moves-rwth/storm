#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMDPMULTIOBJECTIVEWEIGHTVECTORCHECKER_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMDPMULTIOBJECTIVEWEIGHTVECTORCHECKER_H_

#include <vector>

#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveWeightVectorChecker.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            /*!
             * Helper Class that takes preprocessed multi objective data and a weight vector and ...
             * - computes the maximal expected reward w.r.t. the weighted sum of the rewards of the individual objectives
             * - extracts the scheduler that induces this maximum
             * - computes for each objective the value induced by this scheduler
             */
            template <class SparseMdpModelType>
            class SparseMdpMultiObjectiveWeightVectorChecker : public SparseMultiObjectiveWeightVectorChecker<SparseMdpModelType> {
            public:
                typedef typename SparseMdpModelType::ValueType ValueType;
                typedef SparseMultiObjectivePreprocessorData<SparseMdpModelType> PreprocessorData;
            
                SparseMdpMultiObjectiveWeightVectorChecker(PreprocessorData const& data);
                
            private:
                
                /*!
                 * For each time epoch (starting with the maximal stepBound occurring in the objectives), this method
                 * - determines the objectives that are relevant in the current time epoch
                 * - determines the maximizing scheduler for the weighted reward vector of these objectives
                 * - computes the values of these objectives w.r.t. this scheduler
                 *
                 * @param weightVector the weight vector of the current check
                 * @param weightedRewardVector the weighted rewards (initially only considering the unbounded objectives, will be extended to all objectives)
                 */
                virtual void boundedPhase(std::vector<ValueType> const& weightVector, std::vector<ValueType>& weightedRewardVector) override;
  
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMDPMULTIOBJECTIVEWEIGHTEDVECTORCHECKER_H_ */
