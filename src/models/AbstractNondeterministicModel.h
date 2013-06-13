#ifndef STORM_MODELS_ABSTRACTNONDETERMINISTICMODEL_H_
#define STORM_MODELS_ABSTRACTNONDETERMINISTICMODEL_H_

#include "AbstractModel.h"

#include <memory>

namespace storm {

namespace models {

/*!
 *	@brief	Base class for all non-deterministic model classes.
 *
 *	This is base class defines a common interface for all non-deterministic models.
 */
template<class T>
class AbstractNondeterministicModel: public AbstractModel<T> {

	public:
		/*! Constructs an abstract non-determinstic model from the given parameters.
		 * @param transitionMatrix The matrix representing the transitions in the model.
		 * @param stateLabeling The labeling that assigns a set of atomic
		 * propositions to each state.
		 * @param choiceIndices A mapping from states to rows in the transition matrix.
		 * @param stateRewardVector The reward values associated with the states.
		 * @param transitionRewardMatrix The reward values associated with the transitions of the model.
		 */
		AbstractNondeterministicModel(std::shared_ptr<storm::storage::SparseMatrix<T>> transitionMatrix,
			std::shared_ptr<storm::models::AtomicPropositionsLabeling> stateLabeling,
			std::shared_ptr<std::vector<uint_fast64_t>> nondeterministicChoiceIndices,
			std::shared_ptr<std::vector<T>> stateRewardVector,
			std::shared_ptr<storm::storage::SparseMatrix<T>> transitionRewardMatrix)
			: AbstractModel<T>(transitionMatrix, stateLabeling, stateRewardVector, transitionRewardMatrix),
			  nondeterministicChoiceIndices(nondeterministicChoiceIndices) {
		}

		/*!
		 * Destructor.
		 */
		virtual ~AbstractNondeterministicModel() {
			// Intentionally left empty.
		}

		/*!
		 * Copy Constructor.
		 */
		AbstractNondeterministicModel(AbstractNondeterministicModel const& other) : AbstractModel<T>(other),
				nondeterministicChoiceIndices(other.nondeterministicChoiceIndices) {
			// Intentionally left empty.
		}

		/*!
		 * Returns the number of choices for all states of the MDP.
		 * @return The number of choices for all states of the MDP.
		 */
		uint_fast64_t getNumberOfChoices() const {
			return this->transitionMatrix->getRowCount();
		}
    
		/*!
		 * Retrieves the size of the internal representation of the model in memory.
		 * @return the size of the internal representation of the model in memory
		 * measured in bytes.
		 */
		virtual uint_fast64_t getSizeInMemory() const {
			return AbstractModel<T>::getSizeInMemory() + nondeterministicChoiceIndices->size() * sizeof(uint_fast64_t);
		}

		/*!
		 * Retrieves the vector indicating which matrix rows represent non-deterministic choices
		 * of a certain state.
		 * @param the vector indicating which matrix rows represent non-deterministic choices
		 * of a certain state.
		 */
		std::vector<uint_fast64_t> const& getNondeterministicChoiceIndices() const {
			return *nondeterministicChoiceIndices;
		}
    
        /*!
         * Returns an iterator to the successors of the given state.
         *
         * @param state The state for which to return the iterator.
         * @return An iterator to the successors of the given state.
         */
        virtual typename storm::storage::SparseMatrix<T>::ConstIndexIterator constStateSuccessorIteratorBegin(uint_fast64_t state) const {
            return this->transitionMatrix->constColumnIteratorBegin((*nondeterministicChoiceIndices)[state]);
        }
    
        /*!
         * Returns an iterator pointing to the element past the successors of the given state.
         *
         * @param state The state for which to return the iterator.
         * @return An iterator pointing to the element past the successors of the given state.
         */
        virtual typename storm::storage::SparseMatrix<T>::ConstIndexIterator constStateSuccessorIteratorEnd(uint_fast64_t state) const {
            return this->transitionMatrix->constColumnIteratorEnd((*nondeterministicChoiceIndices)[state + 1] - 1);
        }

        virtual void writeDotToStream(std::ostream& outStream, bool includeLabeling = true, storm::storage::BitVector const* subsystem = nullptr, std::vector<T> const* firstValue = nullptr, std::vector<T> const* secondValue = nullptr, std::vector<uint_fast64_t> const* stateColoring = nullptr, std::vector<std::string> const* colors = nullptr, std::vector<uint_fast64_t>* scheduler = nullptr, bool finalizeOutput = true) const override {
            AbstractModel<T>::writeDotToStream(outStream, includeLabeling, subsystem, firstValue, secondValue, stateColoring, colors, scheduler, false);
        
            // Initialize the two iterators that we are going to use.
            typename storm::storage::SparseMatrix<T>::ConstRowsIterator transitionIt = this->getTransitionMatrix().begin();
            typename storm::storage::SparseMatrix<T>::ConstRowsIterator transitionIte = this->getTransitionMatrix().begin();

            for (uint_fast64_t state = 0, highestStateIndex = this->getNumberOfStates() - 1; state <= highestStateIndex; ++state) {
                uint_fast64_t rowCount = (*nondeterministicChoiceIndices)[state + 1] - (*nondeterministicChoiceIndices)[state];
                bool highlightChoice = true;
                for (uint_fast64_t row = 0; row < rowCount; ++row) {
                    if (scheduler != nullptr) {
                        // If the scheduler picked the current choice, we will not make it dotted, but highlight it.
                        if ((*scheduler)[state] == row) {
                            highlightChoice = true;
                        } else {
                            highlightChoice = false;
                        }
                    }
                    
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
                    transitionIte.moveToNextRow();
                    for (; transitionIt != transitionIte; ++transitionIt) {
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
                }
            }
        
            if (finalizeOutput) {
                outStream << "}" << std::endl;
            }
        }
    
	private:
		/*! A vector of indices mapping states to the choices (rows) in the transition matrix. */
		std::shared_ptr<std::vector<uint_fast64_t>> nondeterministicChoiceIndices;
};

} // namespace models

} // namespace storm

#endif /* STORM_MODELS_ABSTRACTDETERMINISTICMODEL_H_ */
