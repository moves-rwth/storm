#pragma once

#include <vector>

#include "storm/modelchecker/multiobjective/pcaa/PcaaWeightVectorChecker.h"
#include "storm/modelchecker/multiobjective/rewardbounded/MultiDimensionalRewardUnfolding.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            /*!
             * Helper Class that takes preprocessed Pcaa data and a weight vector and ...
             * - computes the maximal expected reward w.r.t. the weighted sum of the rewards of the individual objectives
             * - extracts the scheduler that induces this maximum
             * - computes for each objective the value induced by this scheduler
             */
            template <class SparseMdpModelType>
            class SparseMdpRewardBoundedPcaaWeightVectorChecker : public PcaaWeightVectorChecker<SparseMdpModelType> {
            public:
                typedef typename SparseMdpModelType::ValueType ValueType;
                typedef typename SparseMdpModelType::RewardModelType RewardModelType;
            
                SparseMdpRewardBoundedPcaaWeightVectorChecker(SparseMultiObjectivePreprocessorResult<SparseMdpModelType> const& preprocessorResult);

                virtual ~SparseMdpRewardBoundedPcaaWeightVectorChecker() = default;

                /*!
                 * - computes the optimal expected reward w.r.t. the weighted sum of the rewards of the individual objectives
                 * - extracts the scheduler that induces this optimum
                 * - computes for each objective the value induced by this scheduler
                 */
                virtual void check(std::vector<ValueType> const& weightVector) override;
                
                /*!
                 * Retrieves the results of the individual objectives at the initial state of the given model.
                 * Note that check(..) has to be called before retrieving results. Otherwise, an exception is thrown.
                 * Also note that there is no guarantee that the under/over approximation is in fact correct
                 * as long as the underlying solution methods are unsound (e.g., standard value iteration).
                 */
                virtual std::vector<ValueType> getUnderApproximationOfInitialStateResults() const override;
                virtual std::vector<ValueType> getOverApproximationOfInitialStateResults() const override;
                
            private:
                
                void computeEpochSolution(typename MultiDimensionalRewardUnfolding<ValueType>::Epoch const& epoch, std::vector<ValueType> const& weightVector);
                
                
                MultiDimensionalRewardUnfolding<ValueType> rewardUnfolding;
                
                boost::optional<std::vector<ValueType>> underApproxResult;
                boost::optional<std::vector<ValueType>> overApproxResult;

                
            };
            
        }
    }
}
