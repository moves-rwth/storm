/*
 * backward_transitions.h
 *
 *  Created on: 17.11.2012
 *      Author: Christian Dehnert
 */

#ifndef GRAPHTRANSITIONS_H_
#define BACKWARDTRANSITIONS_H_

#include "src/storage/SquareSparseMatrix.h"

#include <string.h>

namespace mrmc {

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
	GraphTransitions(mrmc::storage::SquareSparseMatrix<T>* transitionMatrix, bool forward)
			: successorList(nullptr), stateIndications(nullptr), numberOfStates(transitionMatrix->getRowCount()), numberOfNonZeroTransitions(transitionMatrix->getNonZeroEntryCount()) {
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
	 * Returns an iterator to the predecessors of the given states.
	 * @param state The state for which to get the predecessor iterator.
	 * @return An iterator to the predecessors of the given states.
	 */
	statePredecessorIterator beginStatePredecessorIterator(uint_fast64_t state) const {
		return this->successorList + this->stateIndications[state];
	}

	/*!
	 * Returns an iterator referring to the element after the predecessors of
	 * the given state.
	 * @param row The state for which to get the iterator.
	 * @return An iterator referring to the element after the predecessors of
	 * the given state.
	 */
	statePredecessorIterator endStatePredecessorIterator(uint_fast64_t state) const {
		return this->successorList + this->stateIndications[state + 1];
	}

private:

	/*!
	 * Initializes this graph transitions object using the forward transition
	 * relation given by means of a sparse matrix.
	 */
	void initializeForward(mrmc::storage::SquareSparseMatrix<T>* transitionMatrix) {
		this->successorList = new uint_fast64_t[numberOfNonZeroTransitions];
		this->stateIndications = new uint_fast64_t[numberOfStates + 1];

		// First, we copy the index list from the sparse matrix as this will
		// stay the same.
		memcpy(this->stateIndications, transitionMatrix->getRowIndicationsPointer(), numberOfStates + 1);

		// Now we can iterate over all rows of the transition matrix and record
		// the target state.
		for (uint_fast64_t i = 0, currentNonZeroElement = 0; i < numberOfStates; i++) {
			for (auto rowIt = transitionMatrix->beginConstColumnNoDiagIterator(i); rowIt != transitionMatrix->endConstColumnNoDiagIterator(i); ++rowIt) {
				this->stateIndications[currentNonZeroElement++] = *rowIt;
			}
		}
	}

	/*!
	 * Initializes this graph transitions object using the backwards transition
	 * relation, whose forward transition relation is given by means of a sparse
	 * matrix.
	 */
	void initializeBackward(mrmc::storage::SquareSparseMatrix<T>* transitionMatrix) {
		this->successorList = new uint_fast64_t[numberOfNonZeroTransitions];
		this->stateIndications = new uint_fast64_t[numberOfStates + 1];

		// First, we need to count how many backward transitions each state has.
		// NOTE: We disregard the diagonal here, as we only consider "true"
		// predecessors.
		for (uint_fast64_t i = 0; i < numberOfStates; i++) {
			for (auto rowIt = transitionMatrix->beginConstColumnNoDiagIterator(i); rowIt != transitionMatrix->endConstColumnNoDiagIterator(i); ++rowIt) {
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
		memcpy(nextIndicesList, stateIndications, numberOfStates * sizeof(uint_fast64_t));

		// Now we are ready to actually fill in the list of predecessors for
		// every state. Again, we start by considering all but the last row.
		for (uint_fast64_t i = 0; i < numberOfStates; i++) {
			for (auto rowIt = transitionMatrix->beginConstColumnNoDiagIterator(i); rowIt != transitionMatrix->endConstColumnNoDiagIterator(i); ++rowIt) {
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

} // namespace mrmc

#endif /* GRAPHTRANSITIONS_H_ */
