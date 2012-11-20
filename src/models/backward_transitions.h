/*
 * backward_transitions.h
 *
 *  Created on: 17.11.2012
 *      Author: Christian Dehnert
 */

#ifndef BACKWARD_TRANSITIONS_H_
#define BACKWARD_TRANSITIONS_H_

#include <iterator>
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
	//! Constructor
	/*!
	 * Constructs a backward transitions object from the given sparse matrix
	 * representing the (forward) transition relation.
	 * @param transition_matrix The (1-based) matrix representing the transition
	 * relation.
	 */
	BackwardTransitions(mrmc::sparse::StaticSparseMatrix<T>* transition_matrix) {
		this->state_indices_list = new uint_fast64_t[transition_matrix->getRowCount() + 2];
		this->predecessor_list = new uint_fast64_t[transition_matrix->getNonZeroEntryCount()];

		// First, we need to count how many backward transitions each state has.
		// NOTE: We disregard the diagonal here, as we only consider "true"
		// predecessors.
		// Start by counting all but the last row.
		uint_fast64_t* row_indications = transition_matrix->getRowIndicationsPointer();
		uint_fast64_t* column_indications = transition_matrix->getColumnIndicationsPointer();
		for (uint_fast64_t i = 1; i < transition_matrix->getRowCount(); i++) {
			for (uint_fast64_t j = row_indications[i]; j < row_indications[i + 1]; j++) {
				this->state_indices_list[column_indications[j]]++;
			}
		}
		// Add the last row individually, as the comparison bound in the for-loop
		// is different in this case.
		for (uint_fast64_t j = row_indications[transition_matrix->getRowCount()]; j < transition_matrix->getNonZeroEntryCount(); j++) {
			this->state_indices_list[column_indications[j]]++;
		}

		// Now compute the accumulated offsets.
		for (uint_fast64_t i = 1; i < transition_matrix->getRowCount() + 1; i++) {
			this->state_indices_list[i] = this->state_indices_list[i - 1] + this->state_indices_list[i];
		}

		// Put a sentinel element at the end of the indices list. This way,
		// for each state i the range of indices can be read off between
		// state_indices_list[i] and state_indices_list[i + 1].
		this->state_indices_list[transition_matrix->getRowCount() + 1] = this->state_indices_list[transition_matrix->getRowCount()];

		// Create an array that stores the next index for each state. Initially
		// this corresponds to the previously computed accumulated offsets.
		uint_fast64_t* next_state_index_list = new uint_fast64_t[transition_matrix->getRowCount() + 1];
		memcpy(next_state_index_list, state_indices_list, (transition_matrix->getRowCount() + 1) * sizeof(uint_fast64_t));

		// Now we are ready to actually fill in the list of predecessors for
		// every state. Again, we start by considering all but the last row.
		for (uint_fast64_t i = 1; i < transition_matrix->getRowCount(); i++) {
			for (uint_fast64_t j = row_indications[i]; j < row_indications[i + 1]; j++) {
				this->predecessor_list[next_state_index_list[i]++] = column_indications[j];
			}
		}
		// Add the last row individually, as the comparison bound in the for-loop
		// is different in this case.
		for (uint_fast64_t j = row_indications[transition_matrix->getRowCount()]; j < transition_matrix->getNonZeroEntryCount(); j++) {
			this->state_indices_list[next_state_index_list[transition_matrix->getRowCount()]++] = column_indications[j];
		}
	}

	~BackwardTransitions() {
		if (this->predecessor_list != NULL) {
			delete[] this->predecessor_list;
		}
		if (this->state_indices_list != NULL) {
			delete[] this->state_indices_list;
		}
	}

	class const_iterator : public std::iterator<std::input_iterator_tag, uint_fast64_t> {
		public:
			const_iterator(uint_fast64_t* ptr) : ptr_(ptr) {	}

			const_iterator operator++() { const_iterator i = *this; ptr_++; return i; }
			const_iterator operator++(int offset) { ptr_ += offset; return *this; }
			const uint_fast64_t& operator*() { return *ptr_; }
			const uint_fast64_t* operator->() { return ptr_; }
			bool operator==(const const_iterator& rhs) { return ptr_ == rhs.ptr_; }
			bool operator!=(const const_iterator& rhs) { return ptr_ != rhs.ptr_; }
		private:
			uint_fast64_t* ptr_;
	};

	const_iterator beginPredecessorIterator(uint_fast64_t state) const {
		return const_iterator(&(this->predecessor_list[this->state_indices_list[state]]));
	}

	const_iterator endPredecessorIterator(uint_fast64_t state) const {
		return const_iterator(&(this->predecessor_list[this->state_indices_list[state + 1]]));
	}

private:
	/*! A list of predecessors for *all* states. */
	uint_fast64_t* predecessor_list;

	/*!
	 * A list of indices indicating at which position in the global array the
	 * predecessors of a state can be found.
	 */
	uint_fast64_t* state_indices_list;

};

} // namespace models

} // namespace mrmc

#endif /* BACKWARD_TRANSITIONS_H_ */
