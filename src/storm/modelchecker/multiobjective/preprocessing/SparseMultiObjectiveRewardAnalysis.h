#pragma once

#include <memory>
#include <string>

#include "storm/logic/Formulas.h"
#include "storm/modelchecker/multiobjective/preprocessing/SparseMultiObjectivePreprocessorResult.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"

namespace storm {

namespace modelchecker {
namespace multiobjective {
namespace preprocessing {

enum class RewardFinitenessType {
    AllFinite,           // The expected reward is finite for all objectives and all schedulers
    ExistsParetoFinite,  // There is a Pareto optimal scheduler yielding finite rewards for all objectives
    Infinite             // All Pareto optimal schedulers yield infinite reward for at least one objective
};

/*
 * This class performs some analysis task regarding the occurring expected reward objectives
 */
template<class SparseModelType>
class SparseMultiObjectiveRewardAnalysis {
   public:
    typedef typename SparseModelType::ValueType ValueType;
    typedef typename SparseModelType::RewardModelType RewardModelType;

    struct ReturnType {
        RewardFinitenessType rewardFinitenessType;

        // The states of the preprocessed model for which...
        storm::storage::BitVector totalReward0EStates;  // ... there is a scheduler such that all expected total reward objectives have value zero
        storm::storage::BitVector reward0AStates;       // ... all schedulers induce value 0 for all reward-based objectives
        boost::optional<storm::storage::BitVector>
            totalRewardLessInfinityEStates;  // ... there is a scheduler yielding finite reward for all expected total reward objectives
    };

    /*!
     * Analyzes the reward objectives of the multi objective query
     * @param preprocessorResult The result from preprocessing. Must only contain expected total reward or cumulative reward objectives.
     */
    static ReturnType analyze(
        storm::modelchecker::multiobjective::preprocessing::SparseMultiObjectivePreprocessorResult<SparseModelType> const& preprocessorResult);

    /*!
     * Tries to finds an upper bound for the expected reward of the objective (assuming it considers an expected total reward objective)
     */
    static void computeUpperResultBound(SparseModelType const& model, storm::modelchecker::multiobjective::Objective<ValueType>& objective,
                                        storm::storage::SparseMatrix<ValueType> const& backwardTransitions);

   private:
    /*!
     * Computes the set of states that have zero expected reward w.r.t. all expected total/cumulative reward objectives
     */
    static void setReward0States(
        ReturnType& result,
        storm::modelchecker::multiobjective::preprocessing::SparseMultiObjectivePreprocessorResult<SparseModelType> const& preprocessorResult,
        storm::storage::SparseMatrix<ValueType> const& backwardTransitions);

    /*!
     * Checks whether the occurring expected rewards are finite and sets the RewardFinitenessType accordingly
     * Returns the set of states for which a scheduler exists that induces finite reward for all objectives
     */
    static void checkRewardFiniteness(
        ReturnType& result,
        storm::modelchecker::multiobjective::preprocessing::SparseMultiObjectivePreprocessorResult<SparseModelType> const& preprocessorResult,
        storm::storage::SparseMatrix<ValueType> const& backwardTransitions);
};

}  // namespace preprocessing
}  // namespace multiobjective
}  // namespace modelchecker
}  // namespace storm
