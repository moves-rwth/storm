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
#include "src/utility/matrix.h"

namespace storm {
	namespace models {

		template <class T>
		class MarkovAutomaton : public storm::models::AbstractNondeterministicModel<T> {

		public:
			MarkovAutomaton(storm::storage::SparseMatrix<T> const& transitionMatrix, storm::models::AtomicPropositionsLabeling const& stateLabeling,
							storm::storage::BitVector const& markovianStates, std::vector<T> const& exitRates,
							boost::optional<std::vector<T>> const& optionalStateRewardVector, boost::optional<storm::storage::SparseMatrix<T>> const& optionalTransitionRewardMatrix,
							boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> const& optionalChoiceLabeling)
							: AbstractNondeterministicModel<T>(transitionMatrix, stateLabeling, optionalStateRewardVector, optionalTransitionRewardMatrix, optionalChoiceLabeling),
                            markovianStates(markovianStates), exitRates(exitRates), closed(false) {
                                
                this->turnRatesToProbabilities();
                                
				if (this->hasTransitionRewards()) {
					if (!this->getTransitionRewardMatrix().isSubmatrixOf(this->getTransitionMatrix())) {
						LOG4CPLUS_ERROR(logger, "Transition reward matrix is not a submatrix of the transition matrix, i.e. there are rewards for transitions that do not exist.");
						throw storm::exceptions::InvalidArgumentException() << "There are transition rewards for nonexistent transitions.";
					}
				}
			}

			MarkovAutomaton(storm::storage::SparseMatrix<T>&& transitionMatrix,
							storm::models::AtomicPropositionsLabeling&& stateLabeling,
                            storm::storage::BitVector const& markovianStates, std::vector<T> const& exitRates,
							boost::optional<std::vector<T>>&& optionalStateRewardVector,
							boost::optional<storm::storage::SparseMatrix<T>>&& optionalTransitionRewardMatrix,
							boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>&& optionalChoiceLabeling)
							: AbstractNondeterministicModel<T>(std::move(transitionMatrix), std::move(stateLabeling), std::move(optionalStateRewardVector), std::move(optionalTransitionRewardMatrix),
                                                               std::move(optionalChoiceLabeling)), markovianStates(markovianStates), exitRates(std::move(exitRates)), closed(false) {
                                
                this->turnRatesToProbabilities();
                                
		        if (this->hasTransitionRewards()) {
		            if (!this->getTransitionRewardMatrix().isSubmatrixOf(this->getTransitionMatrix())) {
		                LOG4CPLUS_ERROR(logger, "Transition reward matrix is not a submatrix of the transition matrix, i.e. there are rewards for transitions that do not exist.");
		                throw storm::exceptions::InvalidArgumentException() << "There are transition rewards for nonexistent transitions.";
		            }
		        }
			}

			MarkovAutomaton(MarkovAutomaton<T> const& markovAutomaton) : AbstractNondeterministicModel<T>(markovAutomaton), markovianStates(markovAutomaton.markovianStates), exitRates(markovAutomaton.exitRates), closed(markovAutomaton.closed) {
				// Intentionally left empty.
			}

			MarkovAutomaton(MarkovAutomaton<T>&& markovAutomaton) : AbstractNondeterministicModel<T>(std::move(markovAutomaton)), markovianStates(std::move(markovAutomaton.markovianStates)), exitRates(std::move(markovAutomaton.exitRates)), closed(markovAutomaton.closed) {
				// Intentionally left empty.
			}

			~MarkovAutomaton() {
				// Intentionally left empty.
			}

			storm::models::ModelType getType() const {
				return MA;
			}
            
            bool isClosed() const {
                return closed;
            }
            
            bool isHybridState(uint_fast64_t state) const {
                return isMarkovianState(state) && (this->getTransitionMatrix().getRowGroupSize(state) > 1);
            }
                            
            bool isMarkovianState(uint_fast64_t state) const {
                return this->markovianStates.get(state);
            }
            
            bool isProbabilisticState(uint_fast64_t state) const {
                return !this->markovianStates.get(state);
            }
            
            std::vector<T> const& getExitRates() const {
                return this->exitRates;
            }
            
            T const& getExitRate(uint_fast64_t state) const {
                return this->exitRates[state];
            }
            
            T getMaximalExitRate() const {
                T result = storm::utility::constantZero<T>();
                for (auto markovianState : this->markovianStates) {
                    result = std::max(result, this->exitRates[markovianState]);
                }
                return result;
            }
            
            storm::storage::BitVector const& getMarkovianStates() const {
                return this->markovianStates;
            }
            
            void close() {
                if (!closed) {
                    // First, count the number of hybrid states to know how many Markovian choices
                    // will be removed.
                    uint_fast64_t numberOfHybridStates = 0;
                    for (uint_fast64_t state = 0; state < this->getNumberOfStates(); ++state) {
                        if (this->isHybridState(state)) {
                            ++numberOfHybridStates;
                        }
                    }
                    
                    // Then compute how many rows the new matrix is going to have.
                    //uint_fast64_t newNumberOfRows = this->getNumberOfChoices() - numberOfHybridStates;
                    
                    // Create the matrix for the new transition relation and the corresponding nondeterministic choice vector.
                    storm::storage::SparseMatrixBuilder<T> newTransitionMatrixBuilder(0, 0, 0, true, this->getNumberOfStates() + 1);
                    
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
                            for (auto const& entry : this->transitionMatrix.getRow(row)) {
                                newTransitionMatrixBuilder.addNextValue(currentChoice, entry.first, entry.second);
                            }
                            ++currentChoice;
                        }
                    }
                    
                    // Finalize the matrix and put the new transition data in place.
                    this->transitionMatrix = newTransitionMatrixBuilder.build();
                    
                    // Mark the automaton as closed.
                    closed = true;
                }
            }
            
            virtual std::shared_ptr<AbstractModel<T>> applyScheduler(storm::storage::Scheduler const& scheduler) const override {
                if (!closed) {
                    throw storm::exceptions::InvalidStateException() << "Applying a scheduler to a non-closed Markov automaton is illegal; it needs to be closed first.";
                }
                
                storm::storage::SparseMatrix<T> newTransitionMatrix = storm::utility::matrix::applyScheduler(this->getTransitionMatrix(), scheduler);
                return std::shared_ptr<AbstractModel<T>>(new MarkovAutomaton(newTransitionMatrix, this->getStateLabeling(), markovianStates, exitRates, this->hasStateRewards() ? this->getStateRewardVector() : boost::optional<std::vector<T>>(), this->hasTransitionRewards() ? this->getTransitionRewardMatrix() :  boost::optional<storm::storage::SparseMatrix<T>>(), this->hasChoiceLabeling() ? this->getChoiceLabeling() : boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>()));
            }
        
        virtual void writeDotToStream(std::ostream& outStream, bool includeLabeling = true, storm::storage::BitVector const* subsystem = nullptr, std::vector<T> const* firstValue = nullptr, std::vector<T> const* secondValue = nullptr, std::vector<uint_fast64_t> const* stateColoring = nullptr, std::vector<std::string> const* colors = nullptr, std::vector<uint_fast64_t>* scheduler = nullptr, bool finalizeOutput = true) const override {
        AbstractModel<T>::writeDotToStream(outStream, includeLabeling, subsystem, firstValue, secondValue, stateColoring, colors, scheduler, false);
        
        // Write the probability distributions for all the states.
        for (uint_fast64_t state = 0; state < this->getNumberOfStates(); ++state) {
            uint_fast64_t rowCount = this->getTransitionMatrix().getRowGroupIndices()[state + 1] - this->getTransitionMatrix().getRowGroupIndices()[state];
            bool highlightChoice = true;
            
            // For this, we need to iterate over all available nondeterministic choices in the current state.
            for (uint_fast64_t choice = 0; choice < rowCount; ++choice) {
                typename storm::storage::SparseMatrix<T>::const_rows row = this->transitionMatrix.getRow(this->getTransitionMatrix().getRowGroupIndices()[state] + choice);
                
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
                        if (subsystem == nullptr || subsystem->get(transition.first)) {
                            outStream << "\t\"" << state << "c" << choice << "\" -> " << transition.first << " [ label= \"" << transition.second << "\" ]";
                            
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
                            if (subsystem == nullptr || subsystem->get(transition.first)) {
                                outStream << "\t\"" << state << "\" -> " << transition.first << " [ label= \"" << transition.second << " (" << this->exitRates[state] << ")\" ]";
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

            /*!
             * Under the assumption that the Markovian choices of this Markov automaton are expressed in terms of rates in the transition matrix, this procedure turns
             * the rates into the corresponding probabilities by dividing each entry by the exit rate of the state.
             */
            void turnRatesToProbabilities() {
                for (auto state : this->markovianStates) {
                    for (auto& transition : this->transitionMatrix.getRowGroup(state)) {
                        transition.second /= this->exitRates[state];
                    }
                }
            }
                    
            storm::storage::BitVector markovianStates;
			std::vector<T> exitRates;
            bool closed;

		};
	}
}

#endif /* STORM_MODELS_MA_H_ */
