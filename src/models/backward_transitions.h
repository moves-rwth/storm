/*
 * backward_transitions.h
 *
 *  Created on: 17.11.2012
 *      Author: Christian Dehnert
 */

#ifndef BACKWARD_TRANSITIONS_H_
#define BACKWARD_TRANSITIONS_H_

#include <iostream>
#include "src/sparse/static_sparse_matrix.h"

namespace mrmc {

namespace models {

/*!
 * This class stores the predecessors of all states in a state space of the
 * given size.
 */
template <class T>
class BackwardTransitions {

public:
	/*!
	 * Just typedef the iterator as a pointer to the index type.
	 */
	typedef const uint_fast64_t * const state_predecessor_iterator;

	//! Constructor
	/*!
	 * Constructs a backward transitions object from the given sparse matrix
	 * representing the (forward) transition relation.
	 * @param transition_matrix The (0-based) matrix representing the transition
	 * relation.
	 */
	BackwardTransitions(mrmc::sparse::StaticSparseMatrix<T>* transitionMatrix)
			: numberOfStates(transitionMatrix->getRowCount()),
			  numberOfNonZeroTransitions(transitionMatrix->getNonZeroEntryCount()) {
		this->state_indices_list = new uint_fast64_t[numberOfStates + 1];
		this->predecessor_list = new uint_fast64_t[numberOfNonZeroTransitions];

		// First, we need to count how many backward transitions each state has.
		// NOTE: We disregard the diagonal here, as we only consider "true"
		// predecessors.
		for (uint_fast64_t i = 0; i <= numberOfStates; i++) {
			for (auto rowIt = transitionMatrix->beginConstColumnNoDiagIterator(i); rowIt != transitionMatrix->endConstColumnNoDiagIterator(i); ++rowIt) {
				this->state_indices_list[*rowIt + 1]++;
			}
		}

		// Now compute the accumulated offsets.
		for (uint_fast64_t i = 1; i <= numberOfStates; i++) {
			this->state_indices_list[i] = this->state_indices_list[i - 1] + this->state_indices_list[i];
		}

		// Put a sentinel element at the end of the indices list. This way,
		// for each state i the range of indices can be read off between
		// state_indices_list[i] and state_indices_list[i + 1].
		this->state_indices_list[numberOfStates + 1] = numberOfNonZeroTransitions;

		// Create an array that stores the next index for each state. Initially
		// this corresponds to the previously computed accumulated offsets.
		uint_fast64_t* next_state_index_list = new uint_fast64_t[numberOfStates + 1];
		memcpy(next_state_index_list, state_indices_list, (numberOfStates + 1) * sizeof(uint_fast64_t));

		// Now we are ready to actually fill in the list of predecessors for
		// every state. Again, we start by considering all but the last row.
		for (uint_fast64_t i = 0; i <= numberOfStates; i++) {
			for (auto rowIt = transitionMatrix->beginConstColumnNoDiagIterator(i); rowIt != transitionMatrix->endConstColumnNoDiagIterator(i); ++rowIt) {
				this->predecessor_list[next_state_index_list[*rowIt]++] = i;
			}
		}
	}

	//! Destructor
	/*!
	 * Destructor. Frees the internal storage.
	 */
	~BackwardTransitions() {
		if (this->predecessor_list != NULL) {
			delete[] this->predecessor_list;
		}
		if (this->state_indices_list != NULL) {
			delete[] this->state_indices_list;
		}
	}

	/*!
	 * Returns an iterator to the predecessors of the given states.
	 * @param state The state for which to get the predecessor iterator.
	 * @return An iterator to the predecessors of the given states.
	 */
	state_predecessor_iterator beginStatePredecessorIterator(uint_fast64_t state) const {
		return this->predecessor_list + this->state_indices_list[state];
	}


	/*!
	 * Returns an iterator referring to the element after the predecessors of
	 * the given state.
	 * @param row The state for which to get the iterator.
	 * @return An iterator referring to the element after the predecessors of
	 * the given state.
	 */
	state_predecessor_iterator endStatePredecessorIterator(uint_fast64_t state) const {
		return this->predecessor_list + this->state_indices_list[state + 1];
	}

private:
	/*! A list of predecessors for *all* states. */
	uint_fast64_t* predecessor_list;

	/*!
	 * A list of indices indicating at which position in the global array the
	 * predecessors of a state can be found.
	 */
	uint_fast64_t* state_indices_list;

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

#endif /* BACKWARD_TRANSITIONS_H_ */
