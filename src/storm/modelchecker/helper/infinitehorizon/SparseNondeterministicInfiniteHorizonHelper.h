#pragma once
#include "storm/modelchecker/helper/SingleValueModelCheckerHelper.h"

#include "storm/storage/SparseMatrix.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"
#include "storm/models/sparse/StandardRewardModel.h"

namespace storm {
    class Environment;
    
    namespace modelchecker {
        namespace helper {
        
            /*!
             * Helper class for model checking queries that depend on the long run behavior of the (nondeterministic) system.
             */
            template <typename ValueType>
            class SparseNondeterministicInfiniteHorizonHelper : public SingleValueModelCheckerHelper<ValueType> {

            public:
                /*!
                 * Initializes the helper for a discrete time (i.e. MDP)
                 */
                SparseNondeterministicInfiniteHorizonHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions);
                
                /*!
                 * Initializes the helper for a continuous time (i.e. MA)
                 */
                SparseNondeterministicInfiniteHorizonHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& markovianStates, std::vector<ValueType> const& exitRates);
                
                /*!
                 * Computes the long run average probabilities, i.e., the fraction of the time we are in a psiState
                 * @return a value for each state
                 */
                std::vector<ValueType> computeLongRunAverageProbabilities(Environment const& env, storm::storage::BitVector const& psiStates);
                
                /*!
                 * Computes the long run average rewards, i.e., the average reward collected per time unit
                 * @return a value for each state
                 */
                std::vector<ValueType> computeLongRunAverageRewards(Environment const& env, storm::models::sparse::StandardRewardModel<ValueType> const& rewardModel);
                
                /*!
                 * Computes the long run average value given the provided action-based rewards
                 * @return a value for each state
                 */
                std::vector<ValueType> computeLongRunAverageValues(Environment const& env, std::vector<ValueType> const& combinedStateActionRewards);
                
                /*!
                 * Computes the long run average value given the provided state-action-based rewards
                 * @return a value for each state
                 */
                std::vector<ValueType> computeLongRunAverageValues(Environment const& env, std::function<ValueType(uint64_t stateIndex, uint64_t globalChoiceIndex)> const& combinedStateActionRewardsGetter);
                
                /*!
                 * Sets whether an optimal scheduler shall be constructed during the computation
                 */
                void setProduceScheduler(bool value);
                
                /*!
                 * @return whether an optimal scheduler shall be constructed during the computation
                 */
                bool isProduceSchedulerSet() const;
                
                /*!
                 * @pre before calling this, a computation call should have been performed during which scheduler production was enabled.
                 * @return the produced scheduler of the most recent call.
                 */
                std::vector<uint64_t> const& getProducedOptimalChoices() const;
                
                /*!
                 * @pre before calling this, a computation call should have been performed during which scheduler production was enabled.
                 * @return the produced scheduler of the most recent call.
                 */
                std::vector<uint64_t>& getProducedOptimalChoices();
                
                /*!
                 * @pre before calling this, a computation call should have been performed during which scheduler production was enabled.
                 * @return a new scheduler containing optimal choices for each state that yield the long run average values of the most recent call.
                 */
                storm::storage::Scheduler<ValueType> extractScheduler() const;

            protected:
                /*!
                 * @pre if scheduler production is enabled, the _producedOptimalChoices vector should be initialized and sufficiently large
                 * @return the (unique) optimal LRA value for the given mec.
                 * @post _producedOptimalChoices contains choices for the states of the given MEC which yield the returned LRA value.
                 */
                ValueType computeLraForMec(Environment const& env, std::function<ValueType(uint64_t stateIndex, uint64_t globalChoiceIndex)> const& combinedStateActionRewardsGetter, storm::storage::MaximalEndComponent const& mec);
                
                /*!
                 * As computeLraForMec but uses value iteration as a solution method (independent of what is set in env)
                 */
                ValueType computeLraForMecVi(Environment const& env, std::function<ValueType(uint64_t stateIndex, uint64_t globalChoiceIndex)> const& combinedStateActionRewardsGetter, storm::storage::MaximalEndComponent const& mec);
                /*!
                 * As computeLraForMec but uses linear programming as a solution method (independent of what is set in env)
                 */
                ValueType computeLraForMecLp(Environment const& env, std::function<ValueType(uint64_t stateIndex, uint64_t globalChoiceIndex)> const& combinedStateActionRewardsGetter, storm::storage::MaximalEndComponent const& mec);
                
                /*!
                 * @return Lra values for each state
                 */
                std::vector<ValueType> buildAndSolveSsp(Environment const& env, storm::storage::MaximalEndComponentDecomposition<ValueType> const& mecDecomposition, std::vector<ValueType> const& mecLraValues);
            
            private:
                storm::storage::SparseMatrix<ValueType> const& _transitionMatrix;
                storm::storage::SparseMatrix<ValueType> const& _backwardTransitions;
                storm::storage::BitVector const* _markovianStates;
                std::vector<ValueType> const* _exitRates;
                bool _produceScheduler;
                boost::optional<std::vector<uint64_t>> _producedOptimalChoices;
            };

        
        }
    }
}