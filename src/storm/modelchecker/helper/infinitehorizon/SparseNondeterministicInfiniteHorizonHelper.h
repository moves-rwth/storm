#pragma once
#include "storm/modelchecker/helper/SingleValueModelCheckerHelper.h"


namespace storm {
    class Environment;
    
    namespace models {
        namespace sparse {
            template <VT> class StandardRewardModel;
        }
    }
    namespace storage {
        template <typename C> class Decomposition<C>;
        class MaximalEndComponent;
        template <typename VT> class SparseMatrix;
        class StronglyConnectedComponent;
    }
    
    namespace modelchecker {
        namespace helper {
        
            /*!
             * Helper class for model checking queries that depend on the long run behavior of the (nondeterministic) system.
             * @tparam ValueType the type a value can have
             * @tparam Nondeterministic true if there is nondeterminism in the Model (MDP or MA)
             */
            template <typename ValueType, bool Nondeterministic>
            class SparseNondeterministicInfiniteHorizonHelper : public SingleValueModelCheckerHelper<ValueType> {

            public:
                
                /*!
                 * The type of a component in which the system resides in the long run (BSCC for deterministic models, MEC for nondeterministic models)
                 */
                using LongRunComponentType = typename std::conditional<Nondeterministic, storm::storage::MaximalEndComponent, storm::storage::StronglyConnectedComponent>::type;
                
                /*!
                 * Function mapping from indices to values
                 */
                typedef std::function<ValueType(uint64_t)> ValueGetter;
                
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
                 * Provides the decomposition into long run components (BSCCs/MECs) that can be used during the computation.
                 * Providing the decomposition is optional. If it is not provided, they will be computed internally.
                 * Be aware that this class does not take ownership, i.e. the caller has to make sure that the reference to the decomposition remains valid.
                 */
                void provideLongRunComponentDecomposition(storm::storage::Decomposition<ComponentType> const& decomposition);
                
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
                 * Computes the long run average value given the provided state and action-based rewards.
                 * @param stateValues a vector containing a value for every state
                 * @param actionValues a vector containing a value for every choice
                 * @return a value for each state
                 */
                std::vector<ValueType> computeLongRunAverageValues(Environment const& env, std::vector<ValueType> const* stateValues = nullptr, std::vector<ValueType> const* actionValues = nullptr);
                
                /*!
                 * Computes the long run average value given the provided state and action based rewards
                 * @param stateValuesGetter a function returning a value for a given state index
                 * @param actionValuesGetter a function returning a value for a given (global) choice index
                 * @return a value for each state
                 */
                std::vector<ValueType> computeLongRunAverageValues(Environment const& env, ValueGetter const& stateValuesGetter,  sValueGetter const& actionValuesGetter);
                
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

                /*!
                 * @param stateValuesGetter a function returning a value for a given state index
                 * @param actionValuesGetter a function returning a value for a given (global) choice index
                 * @return the (unique) optimal LRA value for the given component.
                 * @post if scheduler production is enabled and Nondeterministic is true, getProducedOptimalChoices() contains choices for the states of the given component which yield the returned LRA value. Choices for states outside of the component are not affected.
                 */
                template < typename = typename std::enable_if< true >::type >
                ValueType computeLraForComponent(Environment const& env, ValueGetter const& stateValuesGetter,  ValueGetter const& actionValuesGetter, LongRunComponentType const& component);
                template < typename = typename std::enable_if< false >::type >
                ValueType computeLraForComponent(Environment const& env, ValueGetter const& stateValuesGetter,  ValueGetter const& actionValuesGetter, LongRunComponentType const& component);
                
            protected:
                
                /*!
                 * @return true iff this is a computation on a continuous time model (i.e. MA)
                 */
                bool isContinuousTime() const;
                
                /*!
                 * Checks if the component can trivially be solved without much overhead.
                 * @return either true and the (unique) optimal LRA value for the given component or false and an arbitrary value
                 * @post if scheduler production is enabled and Nondeterministic is true, getProducedOptimalChoices() contains choices for the states of the given component which yield the returned LRA value. Choices for states outside of the component are not affected.
                 */
                std::pair<bool, ValueType> computeLraForTrivialComponent(Environment const& env, ValueGetter const& stateValuesGetter,  ValueGetter const& actionValuesGetter, LongRunComponentType const& component);
                
                /*!
                 * As computeLraForMec but uses value iteration as a solution method (independent of what is set in env)
                 */
                ValueType computeLraForMecVi(Environment const& env, ValueGetter const& stateValuesGetter, ValueGetter const& actionValuesGetter, LongRunComponentType const& mec);
                /*!
                 * As computeLraForMec but uses linear programming as a solution method (independent of what is set in env)
                 * @see Guck et al.: Modelling and Analysis of Markov Reward Automata (ATVA'14), https://doi.org/10.1007/978-3-319-11936-6_13
                 */
                ValueType computeLraForMecLp(Environment const& env, ValueGetter const& stateValuesGetter, ValueGetter const& actionValuesGetter, LongRunComponentType const& mec);
                
                /*!
                 * @return Lra values for each state
                 */
                std::vector<ValueType> buildAndSolveSsp(Environment const& env, std::vector<ValueType> const& mecLraValues);
            
            private:
                storm::storage::SparseMatrix<ValueType> const& _transitionMatrix;
                storm::storage::SparseMatrix<ValueType> const* _backwardTransitions;
                std::unique_ptr<storm::storage::SparseMatrix<ValueType>> _computedBackwardTransitions;
                storm::storage::Decomposition<LongRunComponentType> const* _longRunComponentDecomposition;
                std::unique_ptr<storm::storage::Decomposition<LongRunComponentType>> _computedLongRunComponentDecomposition;
                storm::storage::BitVector const* _markovianStates;
                std::vector<ValueType> const* _exitRates;
                boost::optional<std::vector<uint64_t>> _producedOptimalChoices;
            };

        
        }
    }
}