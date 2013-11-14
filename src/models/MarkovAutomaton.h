/*
 * MarkovAutomaton.h
 *
 *  Created on: 07.11.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_MODELS_MA_H_
#define STORM_MODELS_MA_H_

#include "AbstractNondeterministicModel.h"
#include "AtomicPropositionsLabeling.h"
#include "src/storage/SparseMatrix.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/settings/Settings.h"
#include "src/utility/vector.h"

namespace storm {
	namespace models {

		template <class T>
		class MarkovAutomaton : public storm::models::AbstractNondeterministicModel<T> {

		public:
			MarkovAutomaton(storm::storage::SparseMatrix<T> const& transitionMatrix, storm::models::AtomicPropositionsLabeling const& stateLabeling,
							std::vector<uint_fast64_t>&& nondeterministicChoiceIndices, storm::storage::BitVector const& markovianStates,
							storm::storage::BitVector const& markovianChoices, std::vector<T> const& exitRates,
							boost::optional<std::vector<T>> const& optionalStateRewardVector, boost::optional<storm::storage::SparseMatrix<T>> const& optionalTransitionRewardMatrix,
							boost::optional<std::vector<storm::storage::VectorSet<uint_fast64_t>>> const& optionalChoiceLabeling)
							: AbstractNondeterministicModel<T>(transitionMatrix, stateLabeling, nondeterministicChoiceIndices, optionalStateRewardVector, optionalTransitionRewardMatrix, optionalChoiceLabeling),
							  markovianStates(markovianStates), markovianChoices(markovianChoices), exitRates(exitRates) {
				if (this->hasTransitionRewards()) {
					if (!this->getTransitionRewardMatrix().isSubmatrixOf(this->getTransitionMatrix())) {
						LOG4CPLUS_ERROR(logger, "Transition reward matrix is not a submatrix of the transition matrix, i.e. there are rewards for transitions that do not exist.");
						throw storm::exceptions::InvalidArgumentException() << "There are transition rewards for nonexistent transitions.";
					}
				}
			}

			MarkovAutomaton(storm::storage::SparseMatrix<T>&& transitionMatrix,
							storm::models::AtomicPropositionsLabeling&& stateLabeling,
							std::vector<uint_fast64_t>&& nondeterministicChoiceIndices,
                            storm::storage::BitVector const& markovianStates,
							storm::storage::BitVector const& markovianChoices, std::vector<T> const& exitRates,
							boost::optional<std::vector<T>>&& optionalStateRewardVector,
							boost::optional<storm::storage::SparseMatrix<T>>&& optionalTransitionRewardMatrix,
							boost::optional<std::vector<storm::storage::VectorSet<uint_fast64_t>>>&& optionalChoiceLabeling)
							: AbstractNondeterministicModel<T>(std::move(transitionMatrix), std::move(stateLabeling), std::move(nondeterministicChoiceIndices), std::move(optionalStateRewardVector), std::move(optionalTransitionRewardMatrix),
																std::move(optionalChoiceLabeling)), markovianStates(markovianStates), markovianChoices(std::move(markovianChoices)), exitRates(std::move(exitRates)) {
		        if (this->hasTransitionRewards()) {
		            if (!this->getTransitionRewardMatrix().isSubmatrixOf(this->getTransitionMatrix())) {
		                LOG4CPLUS_ERROR(logger, "Transition reward matrix is not a submatrix of the transition matrix, i.e. there are rewards for transitions that do not exist.");
		                throw storm::exceptions::InvalidArgumentException() << "There are transition rewards for nonexistent transitions.";
		            }
		        }
			}

			MarkovAutomaton(MarkovAutomaton<T> const& markovAutomaton) : AbstractNondeterministicModel<T>(markovAutomaton), exitRates(markovAutomaton.exitRates), markovianStates(markovAutomaton.markovianStates), markovianChoices(markovAutomaton.markovianChoices) {
				// Intentionally left empty.
			}

			MarkovAutomaton(MarkovAutomaton<T>&& markovAutomaton) : AbstractNondeterministicModel<T>(std::move(markovAutomaton)), markovianStates(std::move(markovAutomaton.markovianStates)), markovianChoices(std::move(markovAutomaton.markovianChoices)), exitRates(std::move(markovAutomaton.exitRates)) {
				// Intentionally left empty.
			}

			~MarkovAutomaton() {
				// Intentionally left empty.
			}

			storm::models::ModelType getType() const {
				return MA;
			}

            virtual void writeDotToStream(std::ostream& outStream, bool includeLabeling = true, storm::storage::BitVector const* subsystem = nullptr, std::vector<T> const* firstValue = nullptr, std::vector<T> const* secondValue = nullptr, std::vector<uint_fast64_t> const* stateColoring = nullptr, std::vector<std::string> const* colors = nullptr, std::vector<uint_fast64_t>* scheduler = nullptr, bool finalizeOutput = true) const override {
                AbstractModel<T>::writeDotToStream(outStream, includeLabeling, subsystem, firstValue, secondValue, stateColoring, colors, scheduler, false);

                // Write the probability distributions for all the states.
                auto rowIt = this->transitionMatrix.begin();
                for (uint_fast64_t state = 0, highestStateIndex = this->getNumberOfStates() - 1; state <= highestStateIndex; ++state) {
                    uint_fast64_t rowCount = this->getNondeterministicChoiceIndices()[state + 1] - this->getNondeterministicChoiceIndices()[state];
                    bool highlightChoice = true;
                
                    // For this, we need to iterate over all available nondeterministic choices in the current state.
                    for (uint_fast64_t row = 0; row < rowCount; ++row, ++rowIt) {
                        if (scheduler != nullptr) {
                            // If the scheduler picked the current choice, we will not make it dotted, but highlight it.
                            if ((*scheduler)[state] == row) {
                                highlightChoice = true;
                            } else {
                                highlightChoice = false;
                            }
                        }
                        
                        if (!markovianChoices.get(this->getNondeterministicChoiceIndices()[state] + row)) {
                            // For each nondeterministic choice, we draw an arrow to an intermediate node to better display
                            // the grouping of transitions.
                            outStream << "\t\"" << state << "c" << row << "\" [shape = \"point\"";
                            
                            // If we were given a scheduler to highlight, we do so now.
                            if (scheduler != nullptr) {
                                if (highlightChoice) {
                                    outStream << ", fillcolor=\"red\"";
                                }
                            }
                            outStream << "];" << std::endl;
                            
                            outStream << "\t" << state << " -> \"" << state << "c" << row << "\"";
                            
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
                            for (auto transitionIt = rowIt.begin(), transitionIte = rowIt.end(); transitionIt != transitionIte; ++transitionIt) {
                                if (subsystem == nullptr || subsystem->get(transitionIt.column())) {
                                    outStream << "\t\"" << state << "c" << row << "\" -> " << transitionIt.column() << " [ label= \"" << transitionIt.value() << "\" ]";
                                    
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
                            for (auto transitionIt = rowIt.begin(), transitionIte = rowIt.end(); transitionIt != transitionIte; ++transitionIt) {
                                if (subsystem == nullptr || subsystem->get(transitionIt.column())) {
                                    outStream << "\t\"" << state << "\" -> " << transitionIt.column() << " [ label= \"" << transitionIt.value() << "\" ]";
                                }
                            }
                        }
                    }
                }
                        
                if (finalizeOutput) {
                    outStream << "}" << std::endl;
                }
            }
            
		private:

            storm::storage::BitVector markovianStates;
			storm::storage::BitVector markovianChoices;
			std::vector<T> exitRates;

		};
	}
}

#endif /* STORM_MODELS_MA_H_ */
