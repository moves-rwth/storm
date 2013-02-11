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
 * This class stores the predecessors of all states in a state space of the
 * given size.
 */
template <class T>
class GraphTransitions {

public:
	/*!
	 * Just typedef the iterator as a pointer to the index type.
	 */
	typedef const uint_fast64_t * const statePredecessorIterator;

	//! Constructor
	/*!
	 * Constructs an object representing the graph structure of the given
	 * transition relation, which is given by a sparse matrix.
	 * @param transitionMatrix The (0-based) matrix representing the transition
	 * relation.
	 * @param forward If set to true, this objects will store the graph structure
	 * of the backwards transition relation.
	 */
	GraphTransitions(std::shared_ptr<storm::storage::SparseMatrix<T>> transitionMatrix, bool forward)
			: successorList(nullptr), stateIndications(nullptr), numberOfStates(transitionMatrix->getColumnCount()), numberOfNonZeroTransitions(transitionMatrix->getNonZeroEntryCount()) {
		if (forward) {
			this->initializeForward(transitionMatrix);
		} else {
			this->initializeBackward(transitionMatrix);
		}
	}

	//! Destructor
	/*!
	 * Destructor. Frees the internal storage.
	 */
	~GraphTransitions() {
		if (this->successorList != nullptr) {
			delete[] this->successorList;
		}
		if (this->stateIndications != nullptr) {
			delete[] this->stateIndications;
		}
	}

	/*!
	 * Retrieves the size of the internal representation of the graph transitions in memory.
	 * @return the size of the internal representation of the graph transitions in memory
	 * measured in bytes.
	 */
	virtual uint_fast64_t getSizeInMemory() const {
		uint_fast64_t result = sizeof(this) + (numberOfStates + numberOfNonZeroTransitions + 1) * sizeof(uint_fast64_t);
		return result;
	}

	/*!
	 * Returns an iterator to the successors of the given state.
	 * @param state The state for which to get the successor iterator.
	 * @return An iterator to the predecessors of the given states.
	 */
	statePredecessorIterator beginStateSuccessorsIterator(uint_fast64_t state) const {
		return this->successorList + this->stateIndications[state];
	}

	/*!
	 * Returns an iterator referring to the element after the successors of
	 * the given state.
	 * @param state The state for which to get the iterator.
	 * @return An iterator referring to the element after the successors of
	 * the given state.
	 */
	statePredecessorIterator endStateSuccessorsIterator(uint_fast64_t state) const {
		return this->successorList + this->stateIndications[state + 1];
	}

private:

	/*!
	 * Initializes this graph transitions object using the forward transition
	 * relation given by means of a sparse matrix.
	 */
	void initializeForward(std::shared_ptr<storm::storage::SparseMatrix<T>> transitionMatrix) {
		this->successorList = new uint_fast64_t[numberOfNonZeroTransitions];
		this->stateIndications = new uint_fast64_t[numberOfStates + 1];

		// First, we copy the index list from the sparse matrix as this will
		// stay the same.
		std::copy(transitionMatrix->getRowIndicationsPointer().begin(), transitionMatrix->getRowIndicationsPointer().end(), this->stateIndications);

		// Now we can iterate over all rows of the transition matrix and record
		// the target state.
		for (uint_fast64_t i = 0, currentNonZeroElement = 0; i < numberOfStates; i++) {
			for (auto rowIt = transitionMatrix->beginConstColumnIterator(i); rowIt != transitionMatrix->endConstColumnIterator(i); ++rowIt) {
				this->stateIndications[currentNonZeroElement++] = *rowIt;
			}
		}
	}

	/*!
	 * Initializes this graph transitions object using the backwards transition
	 * relation, whose forward transition relation is given by means of a sparse
	 * matrix.
	 */
	void initializeBackward(std::shared_ptr<storm::storage::SparseMatrix<T>> transitionMatrix) {
		this->successorList = new uint_fast64_t[numberOfNonZeroTransitions];
		this->stateIndications = new uint_fast64_t[numberOfStates + 1]();

		// First, we need to count how many backward transitions each state has.
		// NOTE: We disregard the diagonal here, as we only consider "true"
		// predecessors.
		for (uint_fast64_t i = 0; i < numberOfStates; i++) {
			for (auto rowIt = transitionMatrix->beginConstColumnIterator(i); rowIt != transitionMatrix->endConstColumnIterator(i); ++rowIt) {
				this->stateIndications[*rowIt + 1]++;
			}
		}

		// Now compute the accumulated offsets.
		for (uint_fast64_t i = 1; i < numberOfStates; i++) {
			this->stateIndications[i] = this->stateIndications[i - 1] + this->stateIndications[i];
		}

		// Put a sentinel element at the end of the indices list. This way,
		// for each state i the range of indices can be read off between
		// state_indices_list[i] and state_indices_list[i + 1].
		this->stateIndications[numberOfStates] = numberOfNonZeroTransitions;

		// Create an array that stores the next index for each state. Initially
		// this corresponds to the previously computed accumulated offsets.
		uint_fast64_t* nextIndicesList = new uint_fast64_t[numberOfStates];
		std::copy(stateIndications, stateIndications + numberOfStates, nextIndicesList);

		// Now we are ready to actually fill in the list of predecessors for
		// every state. Again, we start by considering all but the last row.
		for (uint_fast64_t i = 0; i < numberOfStates; i++) {
			for (auto rowIt = transitionMatrix->beginConstColumnIterator(i); rowIt != transitionMatrix->endConstColumnIterator(i); ++rowIt) {
				this->successorList[nextIndicesList[*rowIt]++] = i;
			}
		}

		// Now we can dispose of the auxiliary array.
		delete[] nextIndicesList;
	}

	/*! A list of successors for *all* states. */
	uint_fast64_t* successorList;

	/*!
	 * A list of indices indicating at which position in the global array the
	 * successors of a state can be found.
	 */
	uint_fast64_t* stateIndications;

	/*!
	 * Store the number of states to determine the highest index at which the
	 * state_indices_list may be accessed.
	 */
	uint_fast64_t numberOfStates;

	/*!
	 * Store the number of non-zero transition entries to determine the highest
	 * index at which the predecessor_list may be accessed.
	 */
	uint_fast64_t numberOfNonZeroTransitions;
};

} // namespace models

} // namespace storm

#endif /* STORM_MODELS_GRAPHTRANSITIONS_H_ */
