#pragma once

#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/storage/Scheduler.h"
#include "storm/modelchecker/multiobjective/Objective.h"
#include "storm/modelchecker/multiobjective/pcaa/PcaaWeightVectorChecker.h"
#include "storm/utility/vector.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            /*!
             * Helper Class that takes preprocessed Pcaa data and a weight vector and ...
             * - computes the optimal expected reward w.r.t. the weighted sum of the rewards of the individual objectives
             * - extracts the scheduler that induces this optimum
             * - computes for each objective the value induced by this scheduler
             */
            template <class SparseModelType>
            class SparsePcaaWeightVectorChecker : public PcaaWeightVectorChecker<SparseModelType> {
            public:
                typedef typename SparseModelType::ValueType ValueType;
                
                /*
                 * Creates a weight vextor checker.
                 *
                 * @param model The (preprocessed) model
                 * @param objectives The (preprocessed) objectives
                 * @param possibleECActions Overapproximation of the actions that are part of an EC
                 * @param possibleBottomStates The states for which it is posible to not collect further reward with prob. 1
                 *
                 */
                
                SparsePcaaWeightVectorChecker(SparseModelType const& model,
                                                        std::vector<Objective<ValueType>> const& objectives,
                                                        storm::storage::BitVector const& possibleECActions,
                                                        storm::storage::BitVector const& possibleBottomStates);
                
                virtual ~SparsePcaaWeightVectorChecker() = default;
                
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
                
                /*!
                 * Retrieves a scheduler that induces the current values
                 * Note that check(..) has to be called before retrieving the scheduler. Otherwise, an exception is thrown.
                 * Also note that (currently) the scheduler only supports unbounded objectives.
                 */
                virtual storm::storage::Scheduler<ValueType> computeScheduler() const override;
                
                
            protected:
                
                /*!
                 * Determines the scheduler that optimizes the weighted reward vector of the unbounded objectives
                 *
                 * @param weightedRewardVector the weighted rewards (only considering the unbounded objectives)
                 */
                void unboundedWeightedPhase(std::vector<ValueType> const& weightedRewardVector, boost::optional<ValueType> const& lowerResultBound, boost::optional<ValueType> const& upperResultBound);
                
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
                 * @param weightedRewardVector the weighted rewards considering the unbounded objectives. Will be invalidated after calling this.
                 */
                virtual void boundedPhase(std::vector<ValueType> const& weightVector, std::vector<ValueType>& weightedRewardVector) = 0;
                
                /*!
                 * Transforms the results of a min-max-solver that considers a reduced model (without end components) to a result for the original (unreduced) model
                 */
                void transformReducedSolutionToOriginalModel(storm::storage::SparseMatrix<ValueType> const& reducedMatrix,
                                                             std::vector<ValueType> const& reducedSolution,
                                                             std::vector<uint_fast64_t> const& reducedOptimalChoices,
                                                             std::vector<uint_fast64_t> const& reducedToOriginalChoiceMapping,
                                                             std::vector<uint_fast64_t> const& originalToReducedStateMapping,
                                                             std::vector<ValueType>& originalSolution,
                                                             std::vector<uint_fast64_t>& originalOptimalChoices) const;
                
                
                // Overapproximation of the set of choices that are part of an end component.
                storm::storage::BitVector possibleECActions;
                // The actions that have reward assigned for at least one objective without upper timeBound
                storm::storage::BitVector actionsWithoutRewardInUnboundedPhase;
                // The states for which it is allowed to visit them infinitely often
                // Put differently, if one of the states is part of a neutral EC, it is possible to
                // stay in this EC forever (withoud inducing infinite reward for some objective).
                storm::storage::BitVector possibleBottomStates;
                // stores the indices of the objectives for which there is no upper time bound
                storm::storage::BitVector objectivesWithNoUpperTimeBound;
                // stores the (discretized) state action rewards for each objective.
                std::vector<std::vector<ValueType>> discreteActionRewards;
                // Memory for the solution of the most recent call of check(..)
                // becomes true after the first call of check(..)
                bool checkHasBeenCalled;
                // The result for the weighted reward vector (for all states of the model)
                std::vector<ValueType> weightedResult;
                // The results for the individual objectives (w.r.t. all states of the model)
                std::vector<std::vector<ValueType>> objectiveResults;
                // Stores for each objective the distance between the computed result (w.r.t. the initial state) and an over/under approximation for the actual result.
                // The distances are stored as a (possibly negative) offset that has to be added (+) to to the objectiveResults.
                std::vector<ValueType> offsetsToUnderApproximation;
                std::vector<ValueType> offsetsToOverApproximation;
                // The scheduler choices that optimize the weighted rewards of undounded objectives.
                std::vector<uint_fast64_t> optimalChoices;
                
            };
            
        }
    }
}

