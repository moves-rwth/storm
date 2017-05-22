#ifndef STORM_MODELS_SPARSE_MARKOVAUTOMATON_H_
#define STORM_MODELS_SPARSE_MARKOVAUTOMATON_H_

#include "storm/models/sparse/NondeterministicModel.h"
#include "storm/models/sparse/Ctmc.h"
#include "storm/utility/OsDetection.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            /*!
             * This class represents a Markov automaton.
             */
            template<class ValueType, typename RewardModelType = StandardRewardModel<ValueType>>
            class MarkovAutomaton : public NondeterministicModel<ValueType, RewardModelType> {
            public:
                /*!
                 * Constructs a model from the given data.
                 *
                 * @param transitionMatrix The matrix representing the transitions in the model in terms of rates.
                 * @param stateLabeling The labeling of the states.
                 * @param markovianStates A bit vector indicating the Markovian states of the automaton.
                 * @param exitRates A vector storing the exit rates of the states.
                 * @param rewardModels A mapping of reward model names to reward models.
                 * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
                 */
                MarkovAutomaton(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                storm::models::sparse::StateLabeling const& stateLabeling,
                                storm::storage::BitVector const& markovianStates,
                                std::vector<ValueType> const& exitRates,
                                std::unordered_map<std::string, RewardModelType> const& rewardModels = std::unordered_map<std::string, RewardModelType>(),
                                boost::optional<storm::models::sparse::ChoiceLabeling> const& optionalChoiceLabeling = boost::none);
            
                /*!
                 * Constructs a model from the given data.
                 *
                 * For hybrid states (i.e., states with Markovian and probabilistic transitions), it is assumed that the first
                 * choice corresponds to the markovian transitions.
                 *
                 * @param rateMatrix The matrix representing the transitions in the model in terms of rates.
                 * @param stateLabeling The labeling of the states.
                 * @param markovianStates A bit vector indicating the Markovian states of the automaton (i.e., states with at least one markovian transition).
                 * @param rewardModels A mapping of reward model names to reward models.
                 * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
                 */
                MarkovAutomaton(storm::storage::SparseMatrix<ValueType> const& rateMatrix,
                                storm::models::sparse::StateLabeling const& stateLabeling,
                                storm::storage::BitVector const& markovianStates,
                                std::unordered_map<std::string, RewardModelType> const& rewardModels = std::unordered_map<std::string, RewardModelType>(),
                                boost::optional<storm::models::sparse::ChoiceLabeling> const& optionalChoiceLabeling = boost::none);
                
                /*!
                 * Constructs a model from the given data.
                 *
                 * For hybrid states (i.e., states with Markovian and probabilistic transitions), it is assumed that the first
                 * choice corresponds to the markovian transitions.
                 *
                 * @param rateMatrix The matrix representing the transitions in the model in terms of rates.
                 * @param stateLabeling The labeling of the states.
                 * @param markovianStates A bit vector indicating the Markovian states of the automaton (i.e., states with at least one markovian transition).
                 * @param rewardModels A mapping of reward model names to reward models.
                 * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
                 */
                MarkovAutomaton(storm::storage::SparseMatrix<ValueType>&& rateMatrix,
                                storm::models::sparse::StateLabeling&& stateLabeling,
                                storm::storage::BitVector&& markovianStates,
                                std::unordered_map<std::string, RewardModelType>&& rewardModels = std::unordered_map<std::string, RewardModelType>(),
                                boost::optional<storm::models::sparse::ChoiceLabeling>&& optionalChoiceLabeling = boost::none);
             
                /*!
                 * Constructs a model by moving the given data.
                 *
                 * @param transitionMatrix The matrix representing the transitions in the model in terms of rates.
                 * @param stateLabeling The labeling of the states.
                 * @param markovianStates A bit vector indicating the Markovian states of the automaton.
                 * @param exitRates A vector storing the exit rates of the states.
                 * @param rewardModels A mapping of reward model names to reward models.
                 * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
                 */
                MarkovAutomaton(storm::storage::SparseMatrix<ValueType>&& transitionMatrix,
                                storm::models::sparse::StateLabeling&& stateLabeling,
                                storm::storage::BitVector const& markovianStates,
                                std::vector<ValueType> const& exitRates,
                                std::unordered_map<std::string, RewardModelType>&& rewardModels = std::unordered_map<std::string, RewardModelType>(),
                                boost::optional<storm::models::sparse::ChoiceLabeling>&& optionalChoiceLabeling = boost::none);
         
                /*!
                 * Constructs a model by moving the given data.
                 *
                 * @param transitionMatrix The matrix representing the transitions in the model in terms of rates.
                 * @param stateLabeling The labeling of the states.
                 * @param markovianStates A bit vector indicating the Markovian states of the automaton.
                 * @param exitRates A vector storing the exit rates of the states.
                 * @param probabilities Flag if transitions matrix contains probabilities or rates
                 * @param rewardModels A mapping of reward model names to reward models.
                 * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
                 */
                MarkovAutomaton(storm::storage::SparseMatrix<ValueType>&& transitionMatrix,
                                storm::models::sparse::StateLabeling&& stateLabeling,
                                storm::storage::BitVector const& markovianStates,
                                std::vector<ValueType> const& exitRates,
                                bool probabilities,
                                std::unordered_map<std::string, RewardModelType>&& rewardModels = std::unordered_map<std::string, RewardModelType>(),
                                boost::optional<storm::models::sparse::ChoiceLabeling>&& optionalChoiceLabeling = boost::none);
                
                MarkovAutomaton(MarkovAutomaton<ValueType, RewardModelType> const& other) = default;
                MarkovAutomaton& operator=(MarkovAutomaton<ValueType, RewardModelType> const& other) = default;
                
                MarkovAutomaton(MarkovAutomaton<ValueType, RewardModelType>&& other) = default;
                MarkovAutomaton& operator=(MarkovAutomaton<ValueType, RewardModelType>&& other) = default;

                /*!
                 * Retrieves whether the Markov automaton is closed.
                 *
                 * @return True iff the Markov automaton is closed.
                 */
                bool isClosed() const;
                
                /*!
                 * Retrieves whether the given state is a hybrid state, i.e. Markovian and probabilistic.
                 *
                 * @param state The state for which determine whether it's hybrid.
                 * @return True iff the state is hybrid.
                 */
                bool isHybridState(storm::storage::sparse::state_type state) const;
                
                /*!
                 * Retrieves whether the given state is a Markovian state.
                 *
                 * @param state The state for which determine whether it's Markovian.
                 * @return True iff the state is Markovian.
                 */
                bool isMarkovianState(storm::storage::sparse::state_type state) const;
                
                /*!
                 * Retrieves whether the given state is a probabilistic state.
                 *
                 * @param state The state for which determine whether it's probabilistic.
                 * @return True iff the state is probabilistic.
                 */
                bool isProbabilisticState(storm::storage::sparse::state_type state) const;
                
                /*!
                 * Retrieves the vector representing the exit rates of the states.
                 *
                 * @return The exit rate vector of the model.
                 */
                std::vector<ValueType> const& getExitRates() const;
                
                /*!
                 * Retrieves the vector representing the exit rates of the states.
                 *
                 * @return The exit rate vector of the model.
                 */
                std::vector<ValueType>& getExitRates();
                
                /*!
                 * Retrieves the exit rate of the given state.
                 *
                 * @param state The state for which retrieve the exit rate.
                 * @return The exit rate of the state.
                 */
                ValueType const& getExitRate(storm::storage::sparse::state_type state) const;
                
                /*!
                 * Retrieves the maximal exit rate over all states of the model.
                 *
                 * @return The maximal exit rate of any state of the model.
                 */
                ValueType getMaximalExitRate() const;
                
                /*!
                 * Retrieves the set of Markovian states of the model.
                 *
                 * @return A bit vector representing the Markovian states of the model.
                 */
                storm::storage::BitVector const& getMarkovianStates() const;
                
                /*!
                 * Closes the Markov automaton. That is, this applies the maximal progress assumption to all hybrid states.
                 */
                void close();
                
                bool hasOnlyTrivialNondeterminism() const;
                
                /*!
                 * Convert the MA into a MA by eliminating all states with probabilistic choices.
                 *
                 * @return Ctmc.
                 */
                std::shared_ptr<storm::models::sparse::Ctmc<ValueType, RewardModelType>> convertToCTMC() const;
                
                virtual void writeDotToStream(std::ostream& outStream, bool includeLabeling = true, storm::storage::BitVector const* subsystem = nullptr, std::vector<ValueType> const* firstValue = nullptr, std::vector<ValueType> const* secondValue = nullptr, std::vector<uint_fast64_t> const* stateColoring = nullptr, std::vector<std::string> const* colors = nullptr, std::vector<uint_fast64_t>* scheduler = nullptr, bool finalizeOutput = true) const override;
                                
                virtual void printModelInformationToStream(std::ostream& out) const override;
                
            private:
                /*!
                 * Under the assumption that the Markovian choices of this Markov automaton are expressed in terms of
                 * rates in the transition matrix, this procedure turns the rates into the corresponding probabilities by
                 * dividing each entry by the sum of the rates for that choice.
                 * Also sets the exitRates accordingly and throws an exception if the values for a non-markovian choice do not sum up to one.
                 */
                void turnRatesToProbabilities();
                
                /*!
                 * Checks whether the automaton is closed by actually looking at the transition information.
                 */
                bool checkIsClosed() const;
                
                // A bit vector representing the set of Markovian states.
                storm::storage::BitVector markovianStates;
                
                // A vector storing the exit rates of all states of the model.
                std::vector<ValueType> exitRates;
                
                // A flag indicating whether the Markov automaton has been closed, which is typically a prerequisite
                // for model checking.
                bool closed;
            };
            
        } // namespace sparse
    } // namespace models
} // namespace storm

#endif /* STORM_MODELS_SPARSE_MARKOVAUTOMATON_H_ */
