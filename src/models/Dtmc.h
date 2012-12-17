/*
 * Dtmc.h
 *
 *  Created on: 14.11.2012
 *      Author: Christian Dehnert
 */

#ifndef MRMC_MODELS_DTMC_H_
#define MRMC_MODELS_DTMC_H_

#include <ostream>
#include <memory>

#include "AtomicPropositionsLabeling.h"
#include "GraphTransitions.h"
#include "src/storage/SquareSparseMatrix.h"

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
	 * @param probabilityMatrix The transition probability function of the
	 * DTMC given by a matrix.
	 * @param stateLabeling The labeling that assigns a set of atomic
	 * propositions to each state.
	 */
	Dtmc(std::shared_ptr<mrmc::storage::SquareSparseMatrix<T>> probabilityMatrix, std::shared_ptr<mrmc::models::AtomicPropositionsLabeling> stateLabeling)
			: probabilityMatrix(probabilityMatrix), stateLabeling(stateLabeling), backwardTransitions(nullptr) {
	}

	//! Copy Constructor
	/*!
	 * Copy Constructor. Performs a deep copy of the given DTMC.
	 * @param dtmc A reference to the DTMC that is to be copied.
	 */
	Dtmc(const Dtmc<T> &dtmc) : probabilityMatrix(dtmc.probabilityMatrix),
			stateLabeling(dtmc.stateLabeling) {
		if (dtmc.backardTransitions != nullptr) {
			this->backwardTransitions = new mrmc::models::GraphTransitions<T>(*dtmc.backwardTransitions);
		}
	}

	//! Destructor
	/*!
	 * Destructor. Frees the matrix and labeling associated with this DTMC.
	 */
	~Dtmc() {
		if (this->backwardTransitions != nullptr) {
			delete this->backwardTransitions;
		}
	}

	/*!
	 * Returns the state space size of the DTMC.
	 * @return The size of the state space of the DTMC.
	 */
	uint_fast64_t getNumberOfStates() const {
		return this->probabilityMatrix->getRowCount();
	}

	/*!
	 * Returns the number of (non-zero) transitions of the DTMC.
	 * @return The number of (non-zero) transitions of the DTMC.
	 */
	uint_fast64_t getNumberOfTransitions() const {
		return this->probabilityMatrix->getNonZeroEntryCount();
	}

	/*!
	 * Returns a bit vector in which exactly those bits are set to true that
	 * correspond to a state labeled with the given atomic proposition.
	 * @param ap The atomic proposition for which to get the bit vector.
	 * @return A bit vector in which exactly those bits are set to true that
	 * correspond to a state labeled with the given atomic proposition.
	 */
	mrmc::storage::BitVector* getLabeledStates(std::string ap) const {
		return this->stateLabeling->getAtomicProposition(ap);
	}

	/*!
	 * Returns a pointer to the matrix representing the transition probability
	 * function.
	 * @return A pointer to the matrix representing the transition probability
	 * function.
	 */
	std::shared_ptr<mrmc::storage::SquareSparseMatrix<T>> getTransitionProbabilityMatrix() const {
		return this->probabilityMatrix;
	}

	/*!
	 *
	 */
	std::set<std::string> getPropositionsForState(uint_fast64_t state) {
		return stateLabeling->getPropositionsForState(state);
	}

	/*!
	 * Retrieves a reference to the backwards transition relation.
	 * @return A reference to the backwards transition relation.
	 */
	mrmc::models::GraphTransitions<T>& getBackwardTransitions() {
		if (this->backwardTransitions == nullptr) {
			this->backwardTransitions = new mrmc::models::GraphTransitions<T>(this->probabilityMatrix, false);
		}
		return *this->backwardTransitions;
	}

	/*!
	 * Prints information about the model to the specified stream.
	 * @param out The stream the information is to be printed to.
	 */
	void printModelInformationToStream(std::ostream& out) const {
		out << "-------------------------------------------------------------- "
			<< std::endl;
		out << "Model type: \t\tDTMC" << std::endl;
		out << "States: \t\t" << this->getNumberOfStates() << std::endl;
		out << "Transitions: \t\t" << this->getNumberOfTransitions() << std::endl;
		this->stateLabeling->printAtomicPropositionsInformationToStream(out);
		out << "Size in memory: \t"
			<< (this->probabilityMatrix->getSizeInMemory() +
				this->stateLabeling->getSizeInMemory() +
				sizeof(*this))/1024 << " kbytes" << std::endl;
		out << "-------------------------------------------------------------- "
			<< std::endl;
	}

private:

	/*! A matrix representing the transition probability function of the DTMC. */
	std::shared_ptr<mrmc::storage::SquareSparseMatrix<T>> probabilityMatrix;

	/*! The labeling of the states of the DTMC. */
	std::shared_ptr<mrmc::models::AtomicPropositionsLabeling> stateLabeling;

	/*!
	 * A data structure that stores the predecessors for all states. This is
	 * needed for backwards directed searches.
	 */
	mrmc::models::GraphTransitions<T>* backwardTransitions;
};

} // namespace models

} // namespace mrmc

#endif /* MRMC_MODELS_DTMC_H_ */
