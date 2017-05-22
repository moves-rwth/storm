#include "storm/models/sparse/MarkovAutomaton.h"

#include "storm/adapters/CarlAdapter.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/solver/stateelimination/StateEliminator.h"
#include "storm/storage/FlexibleSparseMatrix.h"
#include "storm/utility/constants.h"
#include "storm/utility/vector.h"
#include "storm/utility/macros.h"

#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            template <typename ValueType, typename RewardModelType>
            MarkovAutomaton<ValueType, RewardModelType>::MarkovAutomaton(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                        storm::models::sparse::StateLabeling const& stateLabeling,
                                                        storm::storage::BitVector const& markovianStates,
                                                        std::vector<ValueType> const& exitRates,
                                                        std::unordered_map<std::string, RewardModelType> const& rewardModels,
                                                        boost::optional<storm::models::sparse::ChoiceLabeling> const& optionalChoiceLabeling)
            : NondeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::MarkovAutomaton, transitionMatrix, stateLabeling, rewardModels, optionalChoiceLabeling), markovianStates(markovianStates), exitRates(exitRates), closed(this->checkIsClosed()) {
                this->turnRatesToProbabilities();
            }
            
            template <typename ValueType, typename RewardModelType>
            MarkovAutomaton<ValueType, RewardModelType>::MarkovAutomaton(storm::storage::SparseMatrix<ValueType>&& transitionMatrix,
                                                        storm::models::sparse::StateLabeling&& stateLabeling,
                                                        storm::storage::BitVector const& markovianStates,
                                                        std::vector<ValueType> const& exitRates,
                                                        std::unordered_map<std::string, RewardModelType>&& rewardModels,
                                                        boost::optional<storm::models::sparse::ChoiceLabeling>&& optionalChoiceLabeling)
            : NondeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::MarkovAutomaton, std::move(transitionMatrix), std::move(stateLabeling), std::move(rewardModels), std::move(optionalChoiceLabeling)), markovianStates(markovianStates), exitRates(std::move(exitRates)), closed(this->checkIsClosed()) {
                this->turnRatesToProbabilities();
            }
        
            
            template <typename ValueType, typename RewardModelType>
            MarkovAutomaton<ValueType, RewardModelType>::MarkovAutomaton(storm::storage::SparseMatrix<ValueType> const& rateMatrix,
                                                                         storm::models::sparse::StateLabeling const& stateLabeling,
                                                                         storm::storage::BitVector const& markovianStates,
                                                                         std::unordered_map<std::string, RewardModelType> const& rewardModels,
                                                                         boost::optional<storm::models::sparse::ChoiceLabeling> const& optionalChoiceLabeling)
            : NondeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::MarkovAutomaton, rateMatrix, stateLabeling, rewardModels, optionalChoiceLabeling), markovianStates(markovianStates) {
                turnRatesToProbabilities();
                this->closed = checkIsClosed();
            }
            
            template <typename ValueType, typename RewardModelType>
            MarkovAutomaton<ValueType, RewardModelType>::MarkovAutomaton(storm::storage::SparseMatrix<ValueType>&& rateMatrix,
                                                                         storm::models::sparse::StateLabeling&& stateLabeling,
                                                                         storm::storage::BitVector&& markovianStates,
                                                                         std::unordered_map<std::string, RewardModelType>&& rewardModels,
                                                                         boost::optional<storm::models::sparse::ChoiceLabeling>&& optionalChoiceLabeling)
            : NondeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::MarkovAutomaton, rateMatrix, stateLabeling, rewardModels, optionalChoiceLabeling), markovianStates(markovianStates) {
                turnRatesToProbabilities();
                this->closed = checkIsClosed();
            }
            
            
            template <typename ValueType, typename RewardModelType>
            MarkovAutomaton<ValueType, RewardModelType>::MarkovAutomaton(storm::storage::SparseMatrix<ValueType>&& transitionMatrix,
                                                                         storm::models::sparse::StateLabeling&& stateLabeling,
                                                                         storm::storage::BitVector const& markovianStates,
                                                                         std::vector<ValueType> const& exitRates,
                                                                         bool probabilities,
                                                                         std::unordered_map<std::string, RewardModelType>&& rewardModels,
                                                                         boost::optional<storm::models::sparse::ChoiceLabeling>&& optionalChoiceLabeling)
            : NondeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::MarkovAutomaton, std::move(transitionMatrix), std::move(stateLabeling), std::move(rewardModels), std::move(optionalChoiceLabeling)), markovianStates(markovianStates), exitRates(std::move(exitRates)), closed(this->checkIsClosed()) {
                STORM_LOG_ASSERT(probabilities, "Matrix must be probabilistic.");
                STORM_LOG_ASSERT(this->getTransitionMatrix().isProbabilistic(), "Matrix must be probabilistic.");
            }
            
            template <typename ValueType, typename RewardModelType>
            bool MarkovAutomaton<ValueType, RewardModelType>::isClosed() const {
                return closed;
            }
            
            template <typename ValueType, typename RewardModelType>
            bool MarkovAutomaton<ValueType, RewardModelType>::isHybridState(storm::storage::sparse::state_type state) const {
                return isMarkovianState(state) && (this->getTransitionMatrix().getRowGroupSize(state) > 1);
            }
            
            template <typename ValueType, typename RewardModelType>
            bool MarkovAutomaton<ValueType, RewardModelType>::isMarkovianState(storm::storage::sparse::state_type state) const {
                return this->markovianStates.get(state);
            }
            
            template <typename ValueType, typename RewardModelType>
            bool MarkovAutomaton<ValueType, RewardModelType>::isProbabilisticState(storm::storage::sparse::state_type state) const {
                return !this->markovianStates.get(state);
            }
            
            template <typename ValueType, typename RewardModelType>
            std::vector<ValueType> const& MarkovAutomaton<ValueType, RewardModelType>::getExitRates() const {
                return this->exitRates;
            }
            
            template <typename ValueType, typename RewardModelType>
            std::vector<ValueType>& MarkovAutomaton<ValueType, RewardModelType>::getExitRates() {
                return this->exitRates;
            }
            
            template <typename ValueType, typename RewardModelType>
            ValueType const& MarkovAutomaton<ValueType, RewardModelType>::getExitRate(storm::storage::sparse::state_type state) const {
                return this->exitRates[state];
            }
            
            template <typename ValueType, typename RewardModelType>
            ValueType MarkovAutomaton<ValueType, RewardModelType>::getMaximalExitRate() const {
                ValueType result = storm::utility::zero<ValueType>();
                for (auto markovianState : this->markovianStates) {
                    result = std::max(result, this->exitRates[markovianState]);
                }
                return result;
            }
            
            template <typename ValueType, typename RewardModelType>
            storm::storage::BitVector const& MarkovAutomaton<ValueType, RewardModelType>::getMarkovianStates() const {
                return this->markovianStates;
            }
            
            template <typename ValueType, typename RewardModelType>
            void MarkovAutomaton<ValueType, RewardModelType>::close() {
                if (!closed) {
                    // Get the choices that we will keep
                    storm::storage::BitVector keptChoices(this->getNumberOfChoices(), true);
                    for(auto state : this->getMarkovianStates()) {
                        if(this->getTransitionMatrix().getRowGroupSize(state) > 1) {
                            // The state is hybrid, hence, we remove the first choice.
                            keptChoices.set(this->getTransitionMatrix().getRowGroupIndices()[state], false);
                            // Afterwards, the state will no longer be Markovian.
                            this->markovianStates.set(state, false);
                            exitRates[state] = storm::utility::zero<ValueType>();
                        }
                    }
                    // Remove the Markovian choices for the different model ingredients
                    this->getTransitionMatrix() = this->getTransitionMatrix().restrictRows(keptChoices);
                    for(auto& rewModel : this->getRewardModels()) {
                        if(rewModel.second.hasStateActionRewards()) {
                            storm::utility::vector::filterVectorInPlace(rewModel.second.getStateActionRewardVector(), keptChoices);
                        }
                        if(rewModel.second.hasTransitionRewards()) {
                            rewModel.second.getTransitionRewardMatrix() = rewModel.second.getTransitionRewardMatrix().restrictRows(keptChoices);
                        }
                    }
                    if(this->hasChoiceLabeling()) {
                        this->getChoiceLabeling().getSubLabeling(keptChoices);
                    }

                    // Mark the automaton as closed.
                    closed = true;
                }
            }
            
            template <typename ValueType, typename RewardModelType>
            void MarkovAutomaton<ValueType, RewardModelType>::writeDotToStream(std::ostream& outStream, bool includeLabeling, storm::storage::BitVector const* subsystem, std::vector<ValueType> const* firstValue, std::vector<ValueType> const* secondValue, std::vector<uint_fast64_t> const* stateColoring, std::vector<std::string> const* colors, std::vector<uint_fast64_t>* scheduler, bool finalizeOutput) const {
                Model<ValueType, RewardModelType>::writeDotToStream(outStream, includeLabeling, subsystem, firstValue, secondValue, stateColoring, colors, scheduler, false);
                
                // Write the probability distributions for all the states.
                for (uint_fast64_t state = 0; state < this->getNumberOfStates(); ++state) {
                    uint_fast64_t rowCount = this->getTransitionMatrix().getRowGroupIndices()[state + 1] - this->getTransitionMatrix().getRowGroupIndices()[state];
                    bool highlightChoice = true;
                    
                    // For this, we need to iterate over all available nondeterministic choices in the current state.
                    for (uint_fast64_t choice = 0; choice < rowCount; ++choice) {
                        uint_fast64_t rowIndex = this->getTransitionMatrix().getRowGroupIndices()[state] + choice;
                        typename storm::storage::SparseMatrix<ValueType>::const_rows row = this->getTransitionMatrix().getRow(rowIndex);
                        
                        if (scheduler != nullptr) {
                            // If the scheduler picked the current choice, we will not make it dotted, but highlight it.
                            if ((*scheduler)[state] == choice) {
                                highlightChoice = true;
                            } else {
                                highlightChoice = false;
                            }
                        }
                        
                        // If it's not a Markovian state or the current row is the first choice for this state, then we
                        // are dealing with a probabilitic choice.
                        if (!markovianStates.get(state) || choice != 0) {
                            // For each nondeterministic choice, we draw an arrow to an intermediate node to better display
                            // the grouping of transitions.
                            outStream << "\t\"" << state << "c" << choice << "\" [shape = \"point\"";
                            
                            // If we were given a scheduler to highlight, we do so now.
                            if (scheduler != nullptr) {
                                if (highlightChoice) {
                                    outStream << ", fillcolor=\"red\"";
                                }
                            }
                            outStream << "];" << std::endl;
                            
                            outStream << "\t" << state << " -> \"" << state << "c" << choice << "\" [ label= \"" << rowIndex << "\"";
                            
                            // If we were given a scheduler to highlight, we do so now.
                            if (scheduler != nullptr) {
                                if (highlightChoice) {
                                    outStream << ", color=\"red\", penwidth = 2";
                                } else {
                                    outStream << ", style = \"dotted\"";
                                }
                            }
                            outStream << "];" << std::endl;
                            
                            // Now draw all probabilitic arcs that belong to this nondeterminstic choice.
                            for (auto const& transition : row) {
                                if (subsystem == nullptr || subsystem->get(transition.getColumn())) {
                                    outStream << "\t\"" << state << "c" << choice << "\" -> " << transition.getColumn() << " [ label= \"" << transition.getValue() << "\" ]";
                                    
                                    // If we were given a scheduler to highlight, we do so now.
                                    if (scheduler != nullptr) {
                                        if (highlightChoice) {
                                            outStream << " [color=\"red\", penwidth = 2]";
                                        } else {
                                            outStream << " [style = \"dotted\"]";
                                        }
                                    }
                                    outStream << ";" << std::endl;
                                }
                            }
                        } else {
                            // In this case we are emitting a Markovian choice, so draw the arrows directly to the target states.
                            for (auto const& transition : row) {
                                if (subsystem == nullptr || subsystem->get(transition.getColumn())) {
                                    outStream << "\t\"" << state << "\" -> " << transition.getColumn() << " [ label= \"" << transition.getValue() << " (" << this->exitRates[state] << ")\" ]";
                                }
                            }
                        }
                    }
                }
                
                if (finalizeOutput) {
                    outStream << "}" << std::endl;
                }
            }
            
            template <typename ValueType, typename RewardModelType>
            void MarkovAutomaton<ValueType, RewardModelType>::turnRatesToProbabilities() {
                this->exitRates.resize(this->getNumberOfStates());
                for (uint_fast64_t state = 0; state< this->getNumberOfStates(); ++state) {
                    uint_fast64_t row = this->getTransitionMatrix().getRowGroupIndices()[state];
                    if (this->markovianStates.get(state)) {
                        this->exitRates[state] = this->getTransitionMatrix().getRowSum(row);
                        for (auto& transition : this->getTransitionMatrix().getRow(row)) {
                            transition.setValue(transition.getValue() / this->exitRates[state]);
                        }
                        ++row;
                    }
                    for (; row < this->getTransitionMatrix().getRowGroupIndices()[state+1]; ++row) {
                        STORM_LOG_THROW(storm::utility::isOne(this->getTransitionMatrix().getRowSum(row)), storm::exceptions::InvalidArgumentException, "Entries of transition matrix do not sum up to one for (non-Markovian) choice " << row << " of state " << state << " (sum is " << this->getTransitionMatrix().getRowSum(row) << ").");
                    }
                }
            }
            
            template <typename ValueType, typename RewardModelType>
            bool MarkovAutomaton<ValueType, RewardModelType>::hasOnlyTrivialNondeterminism() const {
                // Check every state
                for (uint_fast64_t state = 0; state < this->getNumberOfStates(); ++state) {
                    // Get number of choices in current state
                    uint_fast64_t numberChoices = this->getTransitionMatrix().getRowGroupIndices()[state + 1] - this->getTransitionMatrix().getRowGroupIndices()[state];
                    if (isMarkovianState(state)) {
                        STORM_LOG_ASSERT(numberChoices == 1, "Wrong number of choices for markovian state.");
                    }
                    if (numberChoices > 1) {
                        STORM_LOG_ASSERT(isProbabilisticState(state), "State is not probabilistic.");
                        return false;
                    }
                }
                return true;
            }
            
            template <typename ValueType, typename RewardModelType>
            bool MarkovAutomaton<ValueType, RewardModelType>::checkIsClosed() const {
                for (auto state : markovianStates) {
                    if (this->getTransitionMatrix().getRowGroupSize(state) > 1) {
                        return false;
                    }
                }
                return true;
            }
            
            template <typename ValueType, typename RewardModelType>
            std::shared_ptr<storm::models::sparse::Ctmc<ValueType, RewardModelType>> MarkovAutomaton<ValueType, RewardModelType>::convertToCTMC() const {
                STORM_LOG_TRACE("MA matrix:" << std::endl << this->getTransitionMatrix());
                STORM_LOG_TRACE("Markovian states: " << getMarkovianStates());

                // Eliminate all probabilistic states by state elimination
                // Initialize
                storm::storage::FlexibleSparseMatrix<ValueType> flexibleMatrix(this->getTransitionMatrix());
                storm::storage::FlexibleSparseMatrix<ValueType> flexibleBackwardTransitions(this->getTransitionMatrix().transpose());
                storm::solver::stateelimination::StateEliminator<ValueType> stateEliminator(flexibleMatrix, flexibleBackwardTransitions);
                
                for (uint_fast64_t state = 0; state < this->getNumberOfStates(); ++state) {
                    STORM_LOG_ASSERT(!this->isHybridState(state), "State is hybrid.");
                    if (this->isProbabilisticState(state)) {
                        // Eliminate this probabilistic state
                        stateEliminator.eliminateState(state, true);
                        STORM_LOG_TRACE("Flexible matrix after eliminating state " << state << ":" << std::endl << flexibleMatrix);
                    }
                }
                
                // Create the rate matrix for the CTMC
                storm::storage::SparseMatrixBuilder<ValueType> transitionMatrixBuilder(0, 0, 0, false, false);
                // Remember state to keep
                storm::storage::BitVector keepStates(this->getNumberOfStates(), true);
                for (uint_fast64_t state = 0; state < this->getNumberOfStates(); ++state) {
                    if (storm::utility::isZero(flexibleMatrix.getRowSum(state))) {
                        // State is eliminated and can be discarded
                        keepStates.set(state, false);
                    } else {
                        STORM_LOG_ASSERT(this->isMarkovianState(state), "State is not markovian.");
                        // Copy transitions
                        for (uint_fast64_t row = flexibleMatrix.getRowGroupIndices()[state]; row < flexibleMatrix.getRowGroupIndices()[state + 1]; ++row) {
                            for (auto const& entry : flexibleMatrix.getRow(row)) {
                                // Convert probabilities into rates
                                transitionMatrixBuilder.addNextValue(state, entry.getColumn(), entry.getValue() * exitRates[state]);
                            }
                        }
                    }
                }
                
                storm::storage::SparseMatrix<ValueType> rateMatrix = transitionMatrixBuilder.build();
                rateMatrix = rateMatrix.getSubmatrix(false, keepStates, keepStates, false);
                STORM_LOG_TRACE("New CTMC matrix:" << std::endl << rateMatrix);
                // Construct CTMC
                storm::models::sparse::StateLabeling stateLabeling = this->getStateLabeling().getSubLabeling(keepStates);
                
                //TODO update reward models and choice labels according to kept states
                STORM_LOG_WARN_COND(this->getRewardModels().empty(), "Conversion of MA to CTMC does not preserve rewards.");
                std::unordered_map<std::string, RewardModelType> rewardModels = this->getRewardModels();
                STORM_LOG_WARN_COND(!this->hasChoiceLabeling(), "Conversion of MA to CTMC does not preserve choice labels.");
                
                return std::make_shared<storm::models::sparse::Ctmc<ValueType, RewardModelType>>(std::move(rateMatrix), std::move(stateLabeling), std::move(rewardModels));
            }

            
            template<typename ValueType, typename RewardModelType>
            void MarkovAutomaton<ValueType, RewardModelType>::printModelInformationToStream(std::ostream& out) const {
                this->printModelInformationHeaderToStream(out);
                out << "Choices: \t" << this->getNumberOfChoices() << std::endl;
                out << "Markovian St.: \t" << this->getMarkovianStates().getNumberOfSetBits() << std::endl;
                out << "Max. Rate.: \t" << this->getMaximalExitRate() << std::endl;
                this->printModelInformationFooterToStream(out);
            }
            
            
            template class MarkovAutomaton<double>;
//            template class MarkovAutomaton<float>;
#ifdef STORM_HAVE_CARL
            template class MarkovAutomaton<storm::RationalNumber>;
            
            template class MarkovAutomaton<double, storm::models::sparse::StandardRewardModel<storm::Interval>>;
            template class MarkovAutomaton<storm::RationalFunction>;
#endif
        } // namespace sparse
    } // namespace models
} // namespace storm
