#ifndef STORM_MODELCHECKER_SPARSE_DTMC_PRCTL_MODELCHECKER_HELPER_H_
#define STORM_MODELCHECKER_SPARSE_DTMC_PRCTL_MODELCHECKER_HELPER_H_

#include <vector>

#include <boost/optional.hpp>

#include "storm/modelchecker/hints/ModelCheckerHint.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/logic/OperatorFormula.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"

#include "storm/solver/LinearEquationSolver.h"
#include "storm/solver/SolveGoal.h"

namespace storm {
class Environment;

namespace modelchecker {
class CheckResult;

namespace helper {

template<typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
class SparseDtmcPrctlHelper {
   public:
    static std::map<storm::storage::sparse::state_type, ValueType> computeRewardBoundedValues(
        Environment const& env, storm::models::sparse::Dtmc<ValueType> const& model, std::shared_ptr<storm::logic::OperatorFormula const> rewardBoundedFormula);

    static std::vector<ValueType> computeNextProbabilities(Environment const& env, storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                           storm::storage::BitVector const& nextStates);

    static std::vector<ValueType> computeUntilProbabilities(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal,
                                                            storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                            storm::storage::SparseMatrix<ValueType> const& backwardTransitions,
                                                            storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates,
                                                            bool qualitative, ModelCheckerHint const& hint = ModelCheckerHint());

    static std::vector<ValueType> computeAllUntilProbabilities(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal,
                                                               storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                               storm::storage::BitVector const& initialStates, storm::storage::BitVector const& phiStates,
                                                               storm::storage::BitVector const& psiStates);

    static std::vector<ValueType> computeGloballyProbabilities(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal,
                                                               storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                               storm::storage::SparseMatrix<ValueType> const& backwardTransitions,
                                                               storm::storage::BitVector const& psiStates, bool qualitative);

    static std::vector<ValueType> computeCumulativeRewards(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal,
                                                           storm::storage::SparseMatrix<ValueType> const& transitionMatrix, RewardModelType const& rewardModel,
                                                           uint_fast64_t stepBound);

    static std::vector<ValueType> computeInstantaneousRewards(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal,
                                                              storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                              RewardModelType const& rewardModel, uint_fast64_t stepCount);

    static std::vector<ValueType> computeTotalRewards(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal,
                                                      storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                      storm::storage::SparseMatrix<ValueType> const& backwardTransitions, RewardModelType const& rewardModel,
                                                      bool qualitative, ModelCheckerHint const& hint = ModelCheckerHint());

    static std::vector<ValueType> computeReachabilityRewards(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal,
                                                             storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                             storm::storage::SparseMatrix<ValueType> const& backwardTransitions,
                                                             RewardModelType const& rewardModel, storm::storage::BitVector const& targetStates,
                                                             bool qualitative, ModelCheckerHint const& hint = ModelCheckerHint());

    static std::vector<ValueType> computeReachabilityRewards(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal,
                                                             storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                             storm::storage::SparseMatrix<ValueType> const& backwardTransitions,
                                                             std::vector<ValueType> const& totalStateRewardVector,
                                                             storm::storage::BitVector const& targetStates, bool qualitative,
                                                             ModelCheckerHint const& hint = ModelCheckerHint());

    static std::vector<ValueType> computeReachabilityTimes(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal,
                                                           storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                           storm::storage::SparseMatrix<ValueType> const& backwardTransitions,
                                                           storm::storage::BitVector const& targetStates, bool qualitative,
                                                           ModelCheckerHint const& hint = ModelCheckerHint());

    static std::vector<ValueType> computeConditionalProbabilities(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal,
                                                                  storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                                  storm::storage::SparseMatrix<ValueType> const& backwardTransitions,
                                                                  storm::storage::BitVector const& targetStates,
                                                                  storm::storage::BitVector const& conditionStates, bool qualitative);

    static std::vector<ValueType> computeConditionalRewards(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal,
                                                            storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                            storm::storage::SparseMatrix<ValueType> const& backwardTransitions,
                                                            RewardModelType const& rewardModel, storm::storage::BitVector const& targetStates,
                                                            storm::storage::BitVector const& conditionStates, bool qualitative);

   private:
    static std::vector<ValueType> computeReachabilityRewards(
        Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
        storm::storage::SparseMatrix<ValueType> const& backwardTransitions,
        std::function<std::vector<ValueType>(uint_fast64_t, storm::storage::SparseMatrix<ValueType> const&, storm::storage::BitVector const&)> const&
            totalStateRewardVectorGetter,
        storm::storage::BitVector const& targetStates, bool qualitative, std::function<storm::storage::BitVector()> const& zeroRewardStatesGetter,
        ModelCheckerHint const& hint = ModelCheckerHint());

    struct BaierTransformedModel {
        BaierTransformedModel() : noTargetStates(false) {
            // Intentionally left empty.
        }

        storm::storage::BitVector getNewRelevantStates(storm::storage::BitVector const& oldRelevantStates) const;
        storm::storage::BitVector getNewRelevantStates() const;

        storm::storage::BitVector beforeStates;
        boost::optional<storm::storage::SparseMatrix<ValueType>> transitionMatrix;
        boost::optional<storm::storage::BitVector> targetStates;
        boost::optional<std::vector<ValueType>> stateRewards;
        bool noTargetStates;
    };

    static BaierTransformedModel computeBaierTransformation(Environment const& env, storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                            storm::storage::SparseMatrix<ValueType> const& backwardTransitions,
                                                            storm::storage::BitVector const& targetStates, storm::storage::BitVector const& conditionStates,
                                                            boost::optional<std::vector<ValueType>> const& stateRewards);
};
}  // namespace helper
}  // namespace modelchecker
}  // namespace storm

#endif /* STORM_MODELCHECKER_SPARSE_DTMC_PRCTL_MODELCHECKER_HELPER_H_ */
