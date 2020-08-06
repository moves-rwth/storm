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
                SparseNondeterministicInfiniteHorizonHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix);
                
                /*!
                 * Initializes the helper for a continuous time (i.e. MA)
                 */
                SparseNondeterministicInfiniteHorizonHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& markovianStates, std::vector<ValueType> const& exitRates);
                
                /*!
                 * Provides backward transitions that can be used during the computation.
                 * Providing them is optional. If they are not provided, they will be computed internally
                 * Be aware that this class does not take ownership, i.e. the caller has to make sure that the reference to the backwardstransitions remains valid.
                 */
                void provideBackwardTransitions(storm::storage::SparseMatrix<ValueType> const& backwardsTransitions);
    
                /*!
                 * Provides the maximal end component decomposition that can be used during the computation.
                 * Providing the decomposition is optional. If they are not provided, they will be computed internally
                 * Be aware that this class does not take ownership, i.e. the caller has to make sure that the reference to the decomposition remains valid.
                 */
                void provideMaximalEndComponentDecomposition(storm::storage::MaximalEndComponentDecomposition<ValueType> const& decomposition);
                
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
                std::vector<ValueType> computeLongRunAverageValues(Environment const& env, std::vector<ValueType> const* stateValues = nullptr, std::vector<ValueType> const* actionValues = nullptr);
                
                /*!
                 * Computes the long run average value given the provided state-action-based rewards
                 * @return a value for each state
                 */
                std::vector<ValueType> computeLongRunAverageValues(Environment const& env, std::function<ValueType(uint64_t stateIndex)> const& stateValuesGetter,  std::function<ValueType(uint64_t globalChoiceIndex)> const& actionValuesGetter);
                
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
                 * @return true iff this is a computation on a continuous time model (i.e. MA)
                 */
                bool isContinuousTime() const;
                
                /*!
                 * @pre if scheduler production is enabled, the _producedOptimalChoices vector should be initialized and sufficiently large
                 * @return the (unique) optimal LRA value for the given mec.
                 * @post _producedOptimalChoices contains choices for the states of the given MEC which yield the returned LRA value.
                 */
                ValueType computeLraForMec(Environment const& env, std::function<ValueType(uint64_t stateIndex)> const& stateValuesGetter,  std::function<ValueType(uint64_t globalChoiceIndex)> const& actionValuesGetter, storm::storage::MaximalEndComponent const& mec);
                
                /*!
                 * As computeLraForMec but uses value iteration as a solution method (independent of what is set in env)
                 */
                ValueType computeLraForMecVi(Environment const& env, std::function<ValueType(uint64_t stateIndex)> const& stateValuesGetter,  std::function<ValueType(uint64_t globalChoiceIndex)> const& actionValuesGetter, storm::storage::MaximalEndComponent const& mec);
                /*!
                 * As computeLraForMec but uses linear programming as a solution method (independent of what is set in env)
                 * @see Guck et al.: Modelling and Analysis of Markov Reward Automata (ATVA'14), https://doi.org/10.1007/978-3-319-11936-6_13
                 */
                ValueType computeLraForMecLp(Environment const& env, std::function<ValueType(uint64_t stateIndex)> const& stateValuesGetter,  std::function<ValueType(uint64_t globalChoiceIndex)> const& actionValuesGetter, storm::storage::MaximalEndComponent const& mec);
                
                /*!
                 * @return Lra values for each state
                 */
                std::vector<ValueType> buildAndSolveSsp(Environment const& env, std::vector<ValueType> const& mecLraValues);
            
            private:
                storm::storage::SparseMatrix<ValueType> const& _transitionMatrix;
                storm::storage::SparseMatrix<ValueType> const* _backwardTransitions;
                storm::storage::SparseMatrix<ValueType> _computedBackwardTransitions;
                storm::storage::MaximalEndComponentDecomposition<ValueType> const* _mecDecomposition;
                storm::storage::MaximalEndComponentDecomposition<ValueType> _computedMecDecomposition;
                storm::storage::BitVector const* _markovianStates;
                std::vector<ValueType> const* _exitRates;
                boost::optional<std::vector<uint64_t>> _producedOptimalChoices;
            };

        
        }
    }
}