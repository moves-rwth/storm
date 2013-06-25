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
		 * All values are copied.
		 * @param transitionMatrix The matrix representing the transitions in the model.
		 * @param stateLabeling The labeling that assigns a set of atomic
		 * propositions to each state.
		 * @param choiceIndices A mapping from states to rows in the transition matrix.
		 * @param stateRewardVector The reward values associated with the states.
		 * @param transitionRewardMatrix The reward values associated with the transitions of the model.
		 */
		AbstractNondeterministicModel(
			storm::storage::SparseMatrix<T> const& transitionMatrix, 
			storm::models::AtomicPropositionsLabeling const& stateLabeling,
			std::vector<uint_fast64_t> const& nondeterministicChoiceIndices,
			boost::optional<std::vector<T>> const& optionalStateRewardVector, 
			boost::optional<storm::storage::SparseMatrix<T>> const& optionalTransitionRewardMatrix)
			: AbstractModel<T>(transitionMatrix, stateLabeling, optionalStateRewardVector, optionalTransitionRewardMatrix) {
				this->nondeterministicChoiceIndices = nondeterministicChoiceIndices;
		}

		/*! Constructs an abstract non-determinstic model from the given parameters.
		 * All values are moved.
		 * @param transitionMatrix The matrix representing the transitions in the model.
		 * @param stateLabeling The labeling that assigns a set of atomic
		 * propositions to each state.
		 * @param choiceIndices A mapping from states to rows in the transition matrix.
		 * @param stateRewardVector The reward values associated with the states.
		 * @param transitionRewardMatrix The reward values associated with the transitions of the model.
		 */
		AbstractNondeterministicModel(
			storm::storage::SparseMatrix<T>&& transitionMatrix, 
			storm::models::AtomicPropositionsLabeling&& stateLabeling,
			std::vector<uint_fast64_t>&& nondeterministicChoiceIndices,
			boost::optional<std::vector<T>>&& optionalStateRewardVector, 
			boost::optional<storm::storage::SparseMatrix<T>>&& optionalTransitionRewardMatrix)
			// The std::move call must be repeated here because otherwise this calls the copy constructor of the Base Class
			: AbstractModel<T>(std::move(transitionMatrix), std::move(stateLabeling), std::move(optionalStateRewardVector), std::move(optionalTransitionRewardMatrix)), 
			nondeterministicChoiceIndices(std::move(nondeterministicChoiceIndices)) {
			// Intentionally left empty.	
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
		 * Move Constructor.
		 */
		AbstractNondeterministicModel(AbstractNondeterministicModel&& other) : AbstractModel<T>(std::move(other)),
				nondeterministicChoiceIndices(std::move(other.nondeterministicChoiceIndices)) {
			// Intentionally left empty.
		}

		/*!
		 * Returns the number of choices for all states of the MDP.
		 * @return The number of choices for all states of the MDP.
		 */
		uint_fast64_t getNumberOfChoices() const {
			return this->transitionMatrix.getRowCount();
		}
    
		/*!
		 * Retrieves the size of the internal representation of the model in memory.
		 * @return the size of the internal representation of the model in memory
		 * measured in bytes.
		 */
		virtual uint_fast64_t getSizeInMemory() const {
			return AbstractModel<T>::getSizeInMemory() + nondeterministicChoiceIndices.size() * sizeof(uint_fast64_t);
		}

		/*!
		 * Retrieves the vector indicating which matrix rows represent non-deterministic choices
		 * of a certain state.
		 * @param the vector indicating which matrix rows represent non-deterministic choices
		 * of a certain state.
		 */
		std::vector<uint_fast64_t> const& getNondeterministicChoiceIndices() const {
			return nondeterministicChoiceIndices;
		}
    
        virtual typename storm::storage::SparseMatrix<T>::Rows getRows(uint_fast64_t state) const override {
            return this->transitionMatrix.getRows(nondeterministicChoiceIndices[state], nondeterministicChoiceIndices[state + 1] - 1);
        }
    
        virtual typename storm::storage::SparseMatrix<T>::ConstRowIterator rowIteratorBegin(uint_fast64_t state) const override {
            return this->transitionMatrix.begin(nondeterministicChoiceIndices[state]);
        }
    
        virtual typename storm::storage::SparseMatrix<T>::ConstRowIterator rowIteratorEnd(uint_fast64_t state) const override {
            return this->transitionMatrix.end(nondeterministicChoiceIndices[state + 1] - 1);
        }

		/*!
		 * Calculates a hash over all values contained in this Model.
		 * @return size_t A Hash Value
		 */
		virtual size_t getHash() const override {
			std::size_t result = 0;
			std::size_t hashTmp = storm::utility::Hash<uint_fast64_t>::getHash(nondeterministicChoiceIndices);
			std::cout << "nondeterministicChoiceIndices Hash: " << hashTmp << std::endl;

			boost::hash_combine(result, AbstractModel<T>::getHash());
			boost::hash_combine(result, hashTmp);
			return result;
		}

        virtual void writeDotToStream(std::ostream& outStream, bool includeLabeling = true, storm::storage::BitVector const* subsystem = nullptr, std::vector<T> const* firstValue = nullptr, std::vector<T> const* secondValue = nullptr, std::vector<uint_fast64_t> const* stateColoring = nullptr, std::vector<std::string> const* colors = nullptr, std::vector<uint_fast64_t>* scheduler = nullptr, bool finalizeOutput = true) const override {
            AbstractModel<T>::writeDotToStream(outStream, includeLabeling, subsystem, firstValue, secondValue, stateColoring, colors, scheduler, false);

            // Write the probability distributions for all the states.
            auto rowIt = this->transitionMatrix.begin();
            for (uint_fast64_t state = 0, highestStateIndex = this->getNumberOfStates() - 1; state <= highestStateIndex; ++state) {
                uint_fast64_t rowCount = nondeterministicChoiceIndices[state + 1] - nondeterministicChoiceIndices[state];
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
                }
            }
        
            if (finalizeOutput) {
                outStream << "}" << std::endl;
            }
        }
    
	private:
		/*! A vector of indices mapping states to the choices (rows) in the transition matrix. */
		std::vector<uint_fast64_t> nondeterministicChoiceIndices;
};

} // namespace models

} // namespace storm

#endif /* STORM_MODELS_ABSTRACTDETERMINISTICMODEL_H_ */
