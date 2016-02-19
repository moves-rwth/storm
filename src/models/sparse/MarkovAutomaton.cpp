#include "src/models/sparse/MarkovAutomaton.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/utility/constants.h"
#include "src/adapters/CarlAdapter.h"
#include "src/storage/FlexibleSparseMatrix.h"
#include "src/models/sparse/Dtmc.h"
#include "src/solver/stateelimination/MAEliminator.h"
#include "src/utility/vector.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            template <typename ValueType, typename RewardModelType>
            MarkovAutomaton<ValueType, RewardModelType>::MarkovAutomaton(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                        storm::models::sparse::StateLabeling const& stateLabeling,
                                                        storm::storage::BitVector const& markovianStates,
                                                        std::vector<ValueType> const& exitRates,
                                                        std::unordered_map<std::string, RewardModelType> const& rewardModels,
                                                        boost::optional<std::vector<LabelSet>> const& optionalChoiceLabeling)
            : NondeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::MarkovAutomaton, transitionMatrix, stateLabeling, rewardModels, optionalChoiceLabeling), markovianStates(markovianStates), exitRates(exitRates), closed(false) {
                this->turnRatesToProbabilities();
            }
            
            template <typename ValueType, typename RewardModelType>
            MarkovAutomaton<ValueType, RewardModelType>::MarkovAutomaton(storm::storage::SparseMatrix<ValueType>&& transitionMatrix,
                                                        storm::models::sparse::StateLabeling&& stateLabeling,
                                                        storm::storage::BitVector const& markovianStates,
                                                        std::vector<ValueType> const& exitRates,
                                                        std::unordered_map<std::string, RewardModelType>&& rewardModels,
                                                        boost::optional<std::vector<LabelSet>>&& optionalChoiceLabeling)
            : NondeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::MarkovAutomaton, std::move(transitionMatrix), std::move(stateLabeling), std::move(rewardModels), std::move(optionalChoiceLabeling)), markovianStates(markovianStates), exitRates(std::move(exitRates)), closed(false) {
                this->turnRatesToProbabilities();
            }
            
            template <typename ValueType, typename RewardModelType>
            MarkovAutomaton<ValueType, RewardModelType>::MarkovAutomaton(storm::storage::SparseMatrix<ValueType>&& transitionMatrix,
                                                                         storm::models::sparse::StateLabeling&& stateLabeling,
                                                                         storm::storage::BitVector const& markovianStates,
                                                                         std::vector<ValueType> const& exitRates,
                                                                         bool probabilities,
                                                                         std::unordered_map<std::string, RewardModelType>&& rewardModels,
                                                                         boost::optional<std::vector<LabelSet>>&& optionalChoiceLabeling)
            : NondeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::MarkovAutomaton, std::move(transitionMatrix), std::move(stateLabeling), std::move(rewardModels), std::move(optionalChoiceLabeling)), markovianStates(markovianStates), exitRates(std::move(exitRates)), closed(false) {
                assert(probabilities);
                assert(this->getTransitionMatrix().isProbabilistic());
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
                    // First, count the number of hybrid states to know how many Markovian choices
                    // will be removed.
                    uint_fast64_t numberOfHybridStates = 0;
                    for (uint_fast64_t state = 0; state < this->getNumberOfStates(); ++state) {
                        if (this->isHybridState(state)) {
                            ++numberOfHybridStates;
                        }
                    }
                    
                    // Create the matrix for the new transition relation and the corresponding nondeterministic choice vector.
                    storm::storage::SparseMatrixBuilder<ValueType> newTransitionMatrixBuilder(0, 0, 0, false, true, this->getNumberOfStates());
                    
                    // Now copy over all choices that need to be kept.
                    uint_fast64_t currentChoice = 0;
                    for (uint_fast64_t state = 0; state < this->getNumberOfStates(); ++state) {
                        // If the state is a hybrid state, closing it will make it a probabilistic state, so we remove the Markovian marking.
                        if (this->isHybridState(state)) {
                            this->markovianStates.set(state, false);
                        }
                        
                        // Record the new beginning of choices of this state.
                        newTransitionMatrixBuilder.newRowGroup(currentChoice);
                        
                        // If we are currently treating a hybrid state, we need to skip its first choice.
                        if (this->isHybridState(state)) {
                            // Remove the Markovian state marking.
                            this->markovianStates.set(state, false);
                        }
                        
                        for (uint_fast64_t row = this->getTransitionMatrix().getRowGroupIndices()[state] + (this->isHybridState(state) ? 1 : 0); row < this->getTransitionMatrix().getRowGroupIndices()[state + 1]; ++row) {
                            for (auto const& entry : this->getTransitionMatrix().getRow(row)) {
                                newTransitionMatrixBuilder.addNextValue(currentChoice, entry.getColumn(), entry.getValue());
                            }
                            ++currentChoice;
                        }
                    }
                    
                    // Finalize the matrix and put the new transition data in place.
                    this->setTransitionMatrix(newTransitionMatrixBuilder.build());
                    
                    // Mark the automaton as closed.
                    closed = true;
                }
            }
            
            template <typename ValueType, typename RewardModelType>
            void MarkovAutomaton<ValueType, RewardModelType>::writeDotToStream(std::ostream& outStream, bool includeLabeling, storm::storage::BitVector const* subsystem, std::vector<ValueType> const* firstValue, std::vector<ValueType> const* secondValue, std::vector<uint_fast64_t> const* stateColoring, std::vector<std::string> const* colors, std::vector<uint_fast64_t>* scheduler, bool finalizeOutput) const {
                NondeterministicModel<ValueType, RewardModelType>::writeDotToStream(outStream, includeLabeling, subsystem, firstValue, secondValue, stateColoring, colors, scheduler, false);
                
                // Write the probability distributions for all the states.
                for (uint_fast64_t state = 0; state < this->getNumberOfStates(); ++state) {
                    uint_fast64_t rowCount = this->getTransitionMatrix().getRowGroupIndices()[state + 1] - this->getTransitionMatrix().getRowGroupIndices()[state];
                    bool highlightChoice = true;
                    
                    // For this, we need to iterate over all available nondeterministic choices in the current state.
                    for (uint_fast64_t choice = 0; choice < rowCount; ++choice) {
                        typename storm::storage::SparseMatrix<ValueType>::const_rows row = this->getTransitionMatrix().getRow(this->getTransitionMatrix().getRowGroupIndices()[state] + choice);
                        
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
                            
                            outStream << "\t" << state << " -> \"" << state << "c" << choice << "\"";
                            
                            // If we were given a scheduler to highlight, we do so now.
                            if (scheduler != nullptr) {
                                if (highlightChoice) {
                                    outStream << " [color=\"red\", penwidth = 2]";
                                } else {
                                    outStream << " [style = \"dotted\"]";
                                }
                            }
                            outStream << ";" << std::endl;
                            
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
            std::size_t MarkovAutomaton<ValueType, RewardModelType>::getSizeInBytes() const {
                return NondeterministicModel<ValueType, RewardModelType>::getSizeInBytes() + markovianStates.getSizeInBytes() + exitRates.size() * sizeof(ValueType);
            }
            
            template <typename ValueType, typename RewardModelType>
            void MarkovAutomaton<ValueType, RewardModelType>::turnRatesToProbabilities() {
                for (auto state : this->markovianStates) {
                    for (auto& transition : this->getTransitionMatrix().getRowGroup(state)) {
                        transition.setValue(transition.getValue() / this->exitRates[state]);
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
                        assert(numberChoices == 1);
                    }
                    if (numberChoices > 1) {
                        assert(isProbabilisticState(state));
                        return false;
                    }
                }
                return true;
            }
            
            template <typename ValueType, typename RewardModelType>
            std::shared_ptr<storm::models::sparse::Ctmc<ValueType, RewardModelType>> MarkovAutomaton<ValueType, RewardModelType>::convertToCTMC() {
                STORM_LOG_TRACE("MA matrix:" << std::endl << this->getTransitionMatrix());
                STORM_LOG_TRACE("Markovian states: " << getMarkovianStates());

                // Eliminate all probabilistic states by state elimination
                // Initialize
                storm::storage::FlexibleSparseMatrix<ValueType> flexibleMatrix(this->getTransitionMatrix());
                storm::storage::FlexibleSparseMatrix<ValueType> flexibleBackwardTransitions(this->getTransitionMatrix().transpose());
                storm::solver::stateelimination::MAEliminator<storm::models::sparse::Dtmc<ValueType>> stateEliminator(flexibleMatrix, flexibleBackwardTransitions);
                
                for (uint_fast64_t state = 0; state < this->getNumberOfStates(); ++state) {
                    assert(!this->isHybridState(state));
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
                        assert(this->isMarkovianState(state));
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
                boost::optional<std::vector<LabelSet>> optionalChoiceLabeling = this->getOptionalChoiceLabeling();
                if (optionalChoiceLabeling) {
                    optionalChoiceLabeling = storm::utility::vector::filterVector(optionalChoiceLabeling.get(), keepStates);
                }
                //TODO update reward models according to kept states
                std::unordered_map<std::string, RewardModelType> rewardModels = this->getRewardModels();
                
                return std::make_shared<storm::models::sparse::Ctmc<ValueType, RewardModelType>>(std::move(rateMatrix), std::move(stateLabeling), std::move(rewardModels), std::move(optionalChoiceLabeling));
            }

            
            template class MarkovAutomaton<double>;
//            template class MarkovAutomaton<float>;
            
#ifdef STORM_HAVE_CARL
            template class MarkovAutomaton<double, storm::models::sparse::StandardRewardModel<storm::Interval>>;

            template class MarkovAutomaton<storm::RationalFunction>;
#endif

        } // namespace sparse
    } // namespace models
} // namespace storm