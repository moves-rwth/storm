#include "src/models/sparse/NondeterministicModel.h"

#include "src/adapters/CarlAdapter.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            template<typename ValueType>
            NondeterministicModel<ValueType>::NondeterministicModel(storm::models::ModelType const& modelType,
                                                                    storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                                    storm::models::sparse::StateLabeling const& stateLabeling,
                                                                    boost::optional<std::vector<ValueType>> const& optionalStateRewardVector,
                                                                    boost::optional<storm::storage::SparseMatrix<ValueType>> const& optionalTransitionRewardMatrix,
                                                                    boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> const& optionalChoiceLabeling)
            : Model<ValueType>(modelType, transitionMatrix, stateLabeling, optionalStateRewardVector, optionalTransitionRewardMatrix, optionalChoiceLabeling) {
                // Intentionally left empty.
            }
            
            template<typename ValueType>
            NondeterministicModel<ValueType>::NondeterministicModel(storm::models::ModelType const& modelType,
                                                                    storm::storage::SparseMatrix<ValueType>&& transitionMatrix,
                                                                    storm::models::sparse::StateLabeling&& stateLabeling,
                                                                    boost::optional<std::vector<ValueType>>&& optionalStateRewardVector,
                                                                    boost::optional<storm::storage::SparseMatrix<ValueType>>&& optionalTransitionRewardMatrix,
                                                                    boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>&& optionalChoiceLabeling)
            : Model<ValueType>(modelType, std::move(transitionMatrix), std::move(stateLabeling), std::move(optionalStateRewardVector), std::move(optionalTransitionRewardMatrix),
                               std::move(optionalChoiceLabeling)) {
                // Intentionally left empty.
            }
            
            template<typename ValueType>
            uint_fast64_t NondeterministicModel<ValueType>::getNumberOfChoices() const {
                return this->getTransitionMatrix().getRowCount();
            }
            
            template<typename ValueType>
            std::vector<uint_fast64_t> const& NondeterministicModel<ValueType>::getNondeterministicChoiceIndices() const {
                return this->getTransitionMatrix().getRowGroupIndices();
            }
            
            template<typename ValueType>
            uint_fast64_t NondeterministicModel<ValueType>::getNumberOfChoices(uint_fast64_t state) const {
                auto indices = this->getNondeterministicChoiceIndices();
                return indices[state+1] - indices[state];
            }
            
            template<typename ValueType>
            void NondeterministicModel<ValueType>::printModelInformationToStream(std::ostream& out) const {
                out << "-------------------------------------------------------------- " << std::endl;
                out << "Model type: \t\t" << this->getType() << " (sparse)" << std::endl;
                out << "States: \t\t" << this->getNumberOfStates() << std::endl;
                out << "Transitions: \t\t" << this->getNumberOfTransitions() << std::endl;
                out << "Choices: \t\t" << this->getNumberOfChoices() << std::endl;
                this->getStateLabeling().printLabelingInformationToStream(out);
                out << "Size in memory: \t" << (this->getSizeInBytes())/1024 << " kbytes" << std::endl;
                out << "-------------------------------------------------------------- " << std::endl;
            }
            
            template<typename ValueType>
            void NondeterministicModel<ValueType>::writeDotToStream(std::ostream& outStream, bool includeLabeling, storm::storage::BitVector const* subsystem, std::vector<ValueType> const* firstValue, std::vector<ValueType> const* secondValue, std::vector<uint_fast64_t> const* stateColoring, std::vector<std::string> const* colors, std::vector<uint_fast64_t>* scheduler, bool finalizeOutput) const {
                Model<ValueType>::writeDotToStream(outStream, includeLabeling, subsystem, firstValue, secondValue, stateColoring, colors, scheduler, false);
                
                // Write the probability distributions for all the states.
                for (uint_fast64_t state = 0; state < this->getNumberOfStates(); ++state) {
                    uint_fast64_t rowCount = this->getNondeterministicChoiceIndices()[state + 1] - this->getNondeterministicChoiceIndices()[state];
                    bool highlightChoice = true;
                    
                    // For this, we need to iterate over all available nondeterministic choices in the current state.
                    for (uint_fast64_t choice = 0; choice < rowCount; ++choice) {
                        typename storm::storage::SparseMatrix<ValueType>::const_rows row = this->getTransitionMatrix().getRow(this->getNondeterministicChoiceIndices()[state] + choice);
                        
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
                    }
                }
                
                if (finalizeOutput) {
                    outStream << "}" << std::endl;
                }
            }
            
            template class NondeterministicModel<double>;
            template class NondeterministicModel<float>;

#ifdef STORM_HAVE_CARL
            template class NondeterministicModel<storm::RationalFunction>;
#endif

        }
    }
}