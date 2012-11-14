/*
 * dtmc.h
 *
 *  Created on: 14.11.2012
 *      Author: Christian Dehnert
 */

#ifndef DTMC_H_
#define DTMC_H_

#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/integer.hpp>

#include <ostream>

#include "atomic_propositions_labeling.h"
#include "src/sparse/static_sparse_matrix.h"

namespace mrmc {

namespace models {

/*!
 * This class represents a discrete-time Markov chain (DTMC) whose states are
 * labeled with atomic propositions.
 */
template <class T>
class Dtmc {

public:
	//! Constructor
	/*!
	 * Constructs a DTMC object from the given transition probability matrix and
	 * the given labeling of the states.
	 * @param probability_matrix The transition probability function of the
	 * DTMC given by a matrix.
	 * @param state_labeling The labeling that assigns a set of atomic
	 * propositions to each state.
	 */
	Dtmc(mrmc::sparse::StaticSparseMatrix<T>* probability_matrix,
			mrmc::models::AtomicPropositionsLabeling* state_labeling) {
		this->probability_matrix = probability_matrix;
		this->state_labeling = state_labeling;
	}

	//! Copy Constructor
	/*!
	 * Copy Constructor. Performs a deep copy of the given DTMC.
	 * @param dtmc A reference to the DTMC that is to be copied.
	 */
	Dtmc(const Dtmc<T> &dtmc) : probability_matrix(dtmc.probability_matrix),
			state_labeling(dtmc.state_labeling) {
		pantheios::log_DEBUG("Copy constructor of DTMC invoked.");
	}

	/*!
	 * Returns the state space size of the DTMC.
	 * @return The size of the state space of the DTMC.
	 */
	uint_fast64_t getStateSpaceSize() {
		return this->probability_matrix->getRowCount();
	}

	/*!
	 * Returns the number of (non-zero) transitions of the DTMC.
	 * @return The number of (non-zero) transitions of the DTMC.
	 */
	uint_fast64_t getNumberOfTransitions() {
		return this->probability_matrix->getNonZeroEntryCount();
	}

	/*!
	 * Returns a pointer to the matrix representing the transition probability
	 * function.
	 * @return A pointer to the matrix representing the transition probability
	 * function.
	 */
	mrmc::sparse::StaticSparseMatrix<T>* getTransitionProbabilityMatrix() {
		return this->probability_matrix;
	}

	/*!
	 * Prints information about the model to the specified stream.
	 * @param out The stream the information is to be printed to.
	 */
	void printModelInformationToStream(std::ostream& out) {
		out << "-------------------------------------------------------------- "
			<< std::endl;
		out << "Model type: \t\tDTMC" << std::endl;
		out << "States: \t\t" << this->getStateSpaceSize() << std::endl;
		out << "Transitions: \t\t" << this->getNumberOfTransitions()
			<< std::endl;
		this->state_labeling->printAtomicPropositionsInformationToStream(out);
		out << "Size in memory: \t"
			<< (this->probability_matrix->getSizeInMemory() +
				this->state_labeling->getSizeInMemory() +
				sizeof(*this))/1024 << " kbytes" << std::endl;
		out << "-------------------------------------------------------------- "
			<< std::endl;
	}

private:

	/*! A matrix representing the transition probability function of the DTMC. */
	mrmc::sparse::StaticSparseMatrix<T>* probability_matrix;

	/*! The labeling of the states of the DTMC. */
	mrmc::models::AtomicPropositionsLabeling* state_labeling;

};

} // namespace models

} // namespace mrmc

#endif /* DTMC_H_ */
