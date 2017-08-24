#include "storm/models/sparse/NondeterministicModel.h"

#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/storage/Scheduler.h"
#include "storm/storage/memorystructure/MemoryStructureBuilder.h"
#include "storm/storage/memorystructure/SparseModelMemoryProduct.h"

#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/exceptions/InvalidOperationException.h"

namespace storm {
    namespace models {
        namespace sparse {
            
             template <typename ValueType, typename RewardModelType>
            NondeterministicModel<ValueType, RewardModelType>::NondeterministicModel(ModelType modelType, storm::storage::sparse::ModelComponents<ValueType, RewardModelType> const& components)
                    : Model<ValueType, RewardModelType>(modelType, components) {
                // Intentionally left empty
            }
            
            template <typename ValueType, typename RewardModelType>
            NondeterministicModel<ValueType, RewardModelType>::NondeterministicModel(ModelType modelType, storm::storage::sparse::ModelComponents<ValueType, RewardModelType>&& components)
                    : Model<ValueType, RewardModelType>(modelType, std::move(components)) {
                // Intentionally left empty
            }
            
            template<typename ValueType, typename RewardModelType>
            uint_fast64_t NondeterministicModel<ValueType, RewardModelType>::getNumberOfChoices() const {
                return this->getTransitionMatrix().getRowCount();
            }
            
            template<typename ValueType, typename RewardModelType>
            std::vector<uint_fast64_t> const& NondeterministicModel<ValueType, RewardModelType>::getNondeterministicChoiceIndices() const {
                return this->getTransitionMatrix().getRowGroupIndices();
            }
            
            template<typename ValueType, typename RewardModelType>
            uint_fast64_t NondeterministicModel<ValueType, RewardModelType>::getNumberOfChoices(uint_fast64_t state) const {
                auto indices = this->getNondeterministicChoiceIndices();
                return indices[state+1] - indices[state];
            }

            template<typename ValueType, typename RewardModelType>
            void NondeterministicModel<ValueType, RewardModelType>::reduceToStateBasedRewards() {
                for (auto& rewardModel : this->getRewardModels()) {
                    rewardModel.second.reduceToStateBasedRewards(this->getTransitionMatrix(), false);
                }
            }
            
            template<typename ValueType, typename RewardModelType>
            std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> NondeterministicModel<ValueType, RewardModelType>::applyScheduler(storm::storage::Scheduler<ValueType> const& scheduler, bool dropUnreachableStates) {
                if (scheduler.isMemorylessScheduler()) {
                    auto memStruct = storm::storage::MemoryStructureBuilder<ValueType, RewardModelType>::buildTrivialMemoryStructure(*this);
                    auto memoryProduct = memStruct.product(*this);
                    if (!dropUnreachableStates) {
                        memoryProduct.setBuildFullProduct();
                    }
                    return memoryProduct.build(scheduler);
                } else {
                    boost::optional<storm::storage::MemoryStructure> const& memStruct = scheduler.getMemoryStructure();
                    STORM_LOG_ASSERT(memStruct, "Memoryless scheduler without memory structure.");
                    auto memoryProduct = memStruct->product(*this);
                    if (!dropUnreachableStates) {
                        memoryProduct.setBuildFullProduct();
                    }
                    return memoryProduct.build(scheduler);
                }
            }
            
            template<typename ValueType, typename RewardModelType>
            void NondeterministicModel<ValueType, RewardModelType>::printModelInformationToStream(std::ostream& out) const {
                this->printModelInformationHeaderToStream(out);
                out << "Choices: \t" << this->getNumberOfChoices() << std::endl;
                this->printModelInformationFooterToStream(out);
            }
            
            template<typename ValueType, typename RewardModelType>
            void NondeterministicModel<ValueType, RewardModelType>::writeDotToStream(std::ostream& outStream, bool includeLabeling, storm::storage::BitVector const* subsystem, std::vector<ValueType> const* firstValue, std::vector<ValueType> const* secondValue, std::vector<uint_fast64_t> const* stateColoring, std::vector<std::string> const* colors, std::vector<uint_fast64_t>* scheduler, bool finalizeOutput) const {
                Model<ValueType, RewardModelType>::writeDotToStream(outStream, includeLabeling, subsystem, firstValue, secondValue, stateColoring, colors, scheduler, false);
                
                // Write the probability distributions for all the states.
                for (uint_fast64_t state = 0; state < this->getNumberOfStates(); ++state) {
                    uint_fast64_t rowCount = this->getNondeterministicChoiceIndices()[state + 1] - this->getNondeterministicChoiceIndices()[state];
                    bool highlightChoice = true;
                    
                    // For this, we need to iterate over all available nondeterministic choices in the current state.
                    for (uint_fast64_t choice = 0; choice < rowCount; ++choice) {
                        uint_fast64_t rowIndex = this->getNondeterministicChoiceIndices()[state] + choice;
                        typename storm::storage::SparseMatrix<ValueType>::const_rows row = this->getTransitionMatrix().getRow(rowIndex);
                        
                        if (scheduler != nullptr) {
                            // If the scheduler picked the current choice, we will not make it dotted, but highlight it.
                            if ((*scheduler)[state] == choice) {
                                highlightChoice = true;
                            } else {
                                highlightChoice = false;
                            }
                        }
                        
                        // For each nondeterministic choice, we draw an arrow to an intermediate node to better display
                        // the grouping of transitions.
                        
                        // The intermediate node:
                        outStream << "\t\"" << state << "c" << choice << "\" [shape = \"point\"";
                        // If we were given a scheduler to highlight, we do so now.
                        if (scheduler != nullptr) {
                            if (highlightChoice) {
                                outStream << ", fillcolor=\"red\"";
                            }
                        }
                        outStream << "];" << std::endl;
                        
                        // The arrow to the intermediate node:
                        outStream << "\t" << state << " -> \"" << state << "c" << choice << "\"";
                        bool arrowHasLabel = false;
                        if (this->isOfType(ModelType::MarkovAutomaton)) {
                            // If this is a Markov automaton, we have to check whether the current choice is a Markovian choice and correspondingly print the exit rate
                            MarkovAutomaton<ValueType, RewardModelType> const* ma = dynamic_cast<MarkovAutomaton<ValueType, RewardModelType> const*>(this);
                            if (ma->isMarkovianState(state) && choice == 0) {
                                arrowHasLabel = true;
                                outStream << " [ label = \"" << ma->getExitRate(state);
                            }
                        }
                        if (this->hasChoiceLabeling()) {
                            if (arrowHasLabel) {
                                outStream << " | {";
                            } else {
                                outStream << " [ label = \"{";
                            }
                            arrowHasLabel = true;
                            bool firstLabel = true;
                            for (auto const& label : this->getChoiceLabeling().getLabelsOfChoice(rowIndex)) {
                                if (!firstLabel) {
                                    outStream << ", ";
                                }
                                firstLabel = false;
                                outStream << label;
                            }
                            outStream << "}";
                        }
                        if (arrowHasLabel) {
                            outStream << "\"";
                        }
                        // If we were given a scheduler to highlight, we do so now.
                        if (scheduler != nullptr) {
                            if (arrowHasLabel) {
                                outStream << ", ";
                            } else {
                                outStream << "[ ";
                            }
                            if (highlightChoice) {
                                outStream << "color=\"red\", penwidth = 2";
                            } else {
                                outStream << "style = \"dotted\"";
                            }
                        }
                        
                        if (arrowHasLabel || scheduler != nullptr) {
                            outStream << "]" << std::endl;
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
                    }
                }
                
                if (finalizeOutput) {
                    outStream << "}" << std::endl;
                }
            }

            template<typename ValueType, typename RewardModelType>
            uint_least64_t NondeterministicModel<ValueType, RewardModelType>::getChoiceIndex(storm::storage::StateActionPair const& stateactPair) const {
                return this->getNondeterministicChoiceIndices()[stateactPair.getState()]+stateactPair.getAction();
            }

            template class NondeterministicModel<double>;

#ifdef STORM_HAVE_CARL
            template class NondeterministicModel<storm::RationalNumber>;
            template class NondeterministicModel<double, storm::models::sparse::StandardRewardModel<storm::Interval>>;
            template class NondeterministicModel<storm::RationalFunction>;
#endif
        }
    }
}
