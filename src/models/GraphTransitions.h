/*
 * GraphTransitions.h
 *
 *  Created on: 17.11.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_MODELS_GRAPHTRANSITIONS_H_
#define STORM_MODELS_GRAPHTRANSITIONS_H_

#include "src/storage/SparseMatrix.h"

#include <algorithm>
#include <memory>

namespace storm {

namespace models {

/*!
 * This class stores the successors of all states in a state space of the
 * given size.
 */
template <class T>
class GraphTransitions {

public:
	/*!
	 * Just typedef the iterator as a pointer to the index type.
	 */
	typedef const uint_fast64_t * stateSuccessorIterator;

	//! Constructor
	/*!
	 * Constructs an object representing the graph structure of the given
	 * transition relation, which is given by a sparse matrix.
	 * @param transitionMatrix The (0-based) matrix representing the transition
	 * relation.
	 * @param forward If set to true, this objects will store the graph structure
	 * of the backwards transition relation.
	 */
	GraphTransitions(storm::storage::SparseMatrix<T> const& transitionMatrix, bool forward)
			: numberOfStates(transitionMatrix.getColumnCount()), numberOfTransitions(transitionMatrix.getNonZeroEntryCount()), successorList(numberOfTransitions), stateIndications(numberOfStates + 1) {
		if (forward) {
			this->initializeForward(transitionMatrix);
		} else {
			this->initializeBackward(transitionMatrix);
		}
	}

	GraphTransitions(storm::storage::SparseMatrix<T> const& transitionMatrix, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, bool forward)
		: numberOfStates(transitionMatrix.getColumnCount()), numberOfTransitions(transitionMatrix.getNonZeroEntryCount()), successorList(numberOfTransitions), stateIndications(numberOfStates + 1) {
		if (forward) {
			this->initializeForward(transitionMatrix, nondeterministicChoiceIndices);
		} else {
			this->initializeBackward(transitionMatrix, nondeterministicChoiceIndices);
		}
	}

	GraphTransitions(GraphTransitions<T> const& transitions, std::vector<std::vector<std::uint_fast64_t>> const& stronglyConnectedComponents, std::map<uint_fast64_t, uint_fast64_t> const& stateToSccMap)
		: numberOfStates(stronglyConnectedComponents.size()), numberOfTransitions(0), successorList(), stateIndications(numberOfStates + 1) {
		this->initializeFromSccDecomposition(transitions, stronglyConnectedComponents, stateToSccMap);
	}

	GraphTransitions() : numberOfStates(0), numberOfTransitions(0), successorList(), stateIndications() {
		// Intentionally left empty.
	}

	GraphTransitions(GraphTransitions<T>&& other) {
		this->numberOfStates = other.numberOfStates;
		this->numberOfTransitions = other.numberOfTransitions;
		std::swap(successorList, other.successorList);
		std::swap(stateIndications, other.stateIndications);
	}

	GraphTransitions<T>& operator=(GraphTransitions<T>&& other) {
		this->numberOfStates = other.numberOfStates;
		this->numberOfTransitions = other.numberOfTransitions;
		std::swap(successorList, other.successorList);
		std::swap(stateIndications, other.stateIndications);
		return *this;
	}

	//! Destructor
	/*!
	 * Destructor. Frees the internal storage.
	 */
	~GraphTransitions() {
		// Intentionally left empty.
	}

	/*!
	 * Retrieves the size of the internal representation of the graph transitions in memory.
	 * @return the size of the internal representation of the graph transitions in memory
	 * measured in bytes.
	 */
	virtual uint_fast64_t getSizeInMemory() const {
		uint_fast64_t result = sizeof(this) + (numberOfStates + numberOfTransitions + 1) * sizeof(uint_fast64_t);
		return result;
	}

	/*!
	 * Returns an iterator to the successors of the given state.
	 * @param state The state for which to get the successor iterator.
	 * @return An iterator to the predecessors of the given states.
	 */
	stateSuccessorIterator beginStateSuccessorsIterator(uint_fast64_t state) const {
		return &this->successorList[this->stateIndications[state]];
	}

	/*!
	 * Returns an iterator referring to the element after the successors of
	 * the given state.
	 * @param state The state for which to get the iterator.
	 * @return An iterator referring to the element after the successors of
	 * the given state.
	 */
	stateSuccessorIterator endStateSuccessorsIterator(uint_fast64_t state) const {
		std::cout << "size: " << this->stateIndications.size() << std::endl;
		std::cout << "idx accessed " << state << std::endl;
		std::cout << this->stateIndications[state + 1] << std::endl;
		std::cout << "other size: " << this->successorList.size() << std::endl;
		std::cout << this->successorList[this->stateIndications[state + 1]] << std::endl;
		return &this->successorList[this->stateIndications[state + 1]];
	}

	/*!
	 * Returns a (naive) string representation of the transitions in this object.
	 * @returns a (naive) string representation of the transitions in this object.
	 */
	std::string toString() const {
		std::stringstream stream;
		stream << successorList << std::endl;
		stream << stateIndications << std::endl;
		return stream.str();
	}

private:
	void initializeFromSccDecomposition(GraphTransitions<T> const& transitions, std::vector<std::vector<uint_fast64_t>> const& stronglyConnectedComponents, std::map<uint_fast64_t, uint_fast64_t> const& stateToSccMap) {
		for (uint_fast64_t currentSccIndex = 0; currentSccIndex < stronglyConnectedComponents.size(); ++currentSccIndex) {
			// Mark beginning of current SCC.
			stateIndications[currentSccIndex] = successorList.size();

			// Get the actual SCC.
			std::vector<uint_fast64_t> const& scc = stronglyConnectedComponents[currentSccIndex];

			// Now, we determine the SCCs which are reachable (in one step) from the current SCC.
			std::set<uint_fast64_t> allTargetSccs;
			for (auto state : scc) {
				for (stateSuccessorIterator succIt = beginStateSuccessorsIterator(state), succIte = endStateSuccessorsIterator(state); succIt != succIte; ++succIt) {
					uint_fast64_t targetScc = stateToSccMap.find(*succIt)->second;

					// We only need to consider transitions that are actually leaving the SCC.
					if (targetScc != currentSccIndex) {
						allTargetSccs.insert(targetScc);
					}
				}
			}

			// Now we can just enumerate all the target SCCs and insert the corresponding transitions.
			for (auto targetScc : allTargetSccs) {
				successorList.push_back(targetScc);
			}
		}

		// Put the sentinel element at the end and initialize the number of transitions.
		stateIndications[numberOfStates] = successorList.size();
		numberOfTransitions = successorList.size();

		std::cout << this->toString() << std::endl;
	}

	/*!
	 * Initializes this graph transitions object using the forward transition
	 * relation given by means of a sparse matrix.
	 */
	void initializeForward(storm::storage::SparseMatrix<T> const& transitionMatrix) {
		// First, we copy the index list from the sparse matrix as this will
		// stay the same.
		std::copy(transitionMatrix.getRowIndications().begin(), transitionMatrix.getRowIndications().end(), this->stateIndications.begin());

		// Now we can iterate over all rows of the transition matrix and record the target state.
		for (uint_fast64_t i = 0, currentNonZeroElement = 0; i < numberOfStates; i++) {
			for (auto rowIt = transitionMatrix.beginConstColumnIterator(i); rowIt != transitionMatrix.endConstColumnIterator(i); ++rowIt) {
				this->successorList[currentNonZeroElement++] = *rowIt;
			}
		}
	}

	void initializeForward(storm::storage::SparseMatrix<T> const& transitionMatrix, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices) {
		for (uint_fast64_t i = 0; i < numberOfStates; ++i) {
			this->stateIndications[i] = transitionMatrix.getRowIndications().at(nondeterministicChoiceIndices[i]);
		}
		this->stateIndications[numberOfStates] = numberOfTransitions;

		// Now we can iterate over all rows of the transition matrix and record
		// the target state.
		for (uint_fast64_t i = 0, currentNonZeroElement = 0; i < numberOfStates; i++) {
			for (uint_fast64_t j = nondeterministicChoiceIndices[i]; j < nondeterministicChoiceIndices[i + 1]; ++j) {
				for (auto rowIt = transitionMatrix.beginConstColumnIterator(j); rowIt != transitionMatrix.endConstColumnIterator(j); ++rowIt) {
					this->successorList[currentNonZeroElement++] = *rowIt;
				}
			}
		}
	}

	/*!
	 * Initializes this graph transitions object using the backwards transition
	 * relation, whose forward transition relation is given by means of a sparse
	 * matrix.
	 */
	void initializeBackward(storm::storage::SparseMatrix<T> const& transitionMatrix) {
		// First, we need to count how many backward transitions each state has.
		for (uint_fast64_t i = 0; i < numberOfStates; ++i) {
			for (auto rowIt = transitionMatrix.beginConstColumnIterator(i); rowIt != transitionMatrix.endConstColumnIterator(i); ++rowIt) {
				this->stateIndications[*rowIt + 1]++;
			}
		}

		// Now compute the accumulated offsets.
		for (uint_fast64_t i = 1; i < numberOfStates; ++i) {
			this->stateIndications[i] = this->stateIndications[i - 1] + this->stateIndications[i];
		}

		// Put a sentinel element at the end of the indices list. This way,
		// for each state i the range of indices can be read off between
		// state_indices_list[i] and state_indices_list[i + 1].
		this->stateIndications[numberOfStates] = numberOfTransitions;

		// Create an array that stores the next index for each state. Initially
		// this corresponds to the previously computed accumulated offsets.
		std::vector<uint_fast64_t> nextIndices = stateIndications;

		// Now we are ready to actually fill in the list of predecessors for
		// every state. Again, we start by considering all but the last row.
		for (uint_fast64_t i = 0; i < numberOfStates; ++i) {
			for (auto rowIt = transitionMatrix.beginConstColumnIterator(i); rowIt != transitionMatrix.endConstColumnIterator(i); ++rowIt) {
				this->successorList[nextIndices[*rowIt]++] = i;
			}
		}
	}

	void initializeBackward(storm::storage::SparseMatrix<T> const& transitionMatrix, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices) {
		// First, we need to count how many backward transitions each state has.
		for (uint_fast64_t i = 0; i < numberOfStates; ++i) {
			for (uint_fast64_t j = nondeterministicChoiceIndices[i]; j < nondeterministicChoiceIndices[i + 1]; ++j) {
				for (auto rowIt = transitionMatrix.beginConstColumnIterator(j); rowIt != transitionMatrix.endConstColumnIterator(j); ++rowIt) {
					this->stateIndications[*rowIt + 1]++;
				}
			}
		}

		// Now compute the accumulated offsets.
		for (uint_fast64_t i = 1; i < numberOfStates; i++) {
			this->stateIndications[i] = this->stateIndications[i - 1] + this->stateIndications[i];
		}

		// Put a sentinel element at the end of the indices list. This way,
		// for each state i the range of indices can be read off between
		// state_indices_list[i] and state_indices_list[i + 1].
		this->stateIndications[numberOfStates] = numberOfTransitions;

		// Create an array that stores the next index for each state. Initially
		// this corresponds to the previously computed accumulated offsets.
		std::vector<uint_fast64_t> nextIndices = stateIndications;

		// Now we are ready to actually fill in the list of predecessors for
		// every state. Again, we start by considering all but the last row.
		for (uint_fast64_t i = 0; i < numberOfStates; i++) {
			for (uint_fast64_t j = nondeterministicChoiceIndices[i]; j < nondeterministicChoiceIndices[i + 1]; ++j) {
				for (auto rowIt = transitionMatrix.beginConstColumnIterator(j); rowIt != transitionMatrix.endConstColumnIterator(j); ++rowIt) {
					this->successorList[nextIndices[*rowIt]++] = i;
				}
			}
		}
	}

	/*!
	 * Store the number of states to determine the highest index at which the
	 * state_indices_list may be accessed.
	 */
	uint_fast64_t numberOfStates;

	/*!
	 * Store the number of non-zero transition entries to determine the highest
	 * index at which the predecessor_list may be accessed.
	 */
	uint_fast64_t numberOfTransitions;

	/*! A list of successors for *all* states. */
	std::vector<uint_fast64_t> successorList;

	/*!
	 * A list of indices indicating at which position in the global array the
	 * successors of a state can be found.
	 */
	std::vector<uint_fast64_t> stateIndications;
};

} // namespace models

} // namespace storm

#endif /* STORM_MODELS_GRAPHTRANSITIONS_H_ */
