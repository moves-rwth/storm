/*
 * Dtmc.h
 *
 *  Created on: 14.11.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_MODELS_DTMC_H_
#define STORM_MODELS_DTMC_H_

#include <ostream>
#include <iostream>
#include <memory>
#include <cstdlib>

#include "AtomicPropositionsLabeling.h"
#include "GraphTransitions.h"
#include "src/storage/SparseMatrix.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/utility/CommandLine.h"

namespace storm {

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
	Dtmc(std::shared_ptr<storm::storage::SparseMatrix<T>> probabilityMatrix,
			std::shared_ptr<storm::models::AtomicPropositionsLabeling> stateLabeling,
			std::shared_ptr<std::vector<T>> stateRewards = nullptr,
			std::shared_ptr<storm::storage::SparseMatrix<T>> transitionRewardMatrix = nullptr)
			: probabilityMatrix(probabilityMatrix), stateLabeling(stateLabeling),
			  stateRewards(stateRewards), transitionRewardMatrix(transitionRewardMatrix),
			  backwardTransitions(nullptr) {
		if (!this->checkValidityProbabilityMatrix()) {
			std::cerr << "Probability matrix is invalid" << std::endl;
		}
	}

	//! Copy Constructor
	/*!
	 * Copy Constructor. Performs a deep copy of the given DTMC.
	 * @param dtmc A reference to the DTMC that is to be copied.
	 */
	Dtmc(const Dtmc<T> &dtmc) : probabilityMatrix(dtmc.probabilityMatrix),
			stateLabeling(dtmc.stateLabeling), stateRewards(dtmc.stateRewards),
			transitionRewardMatrix(dtmc.transitionRewardMatrix) {
		if (dtmc.backardTransitions != nullptr) {
			this->backwardTransitions = new storm::models::GraphTransitions<T>(*dtmc.backwardTransitions);
		}
		if (!this->checkValidityProbabilityMatrix()) {
			std::cerr << "Probability matrix is invalid" << std::endl;
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
	storm::storage::BitVector* getLabeledStates(std::string ap) const {
		return this->stateLabeling->getAtomicProposition(ap);
	}

	/*!
	 * Returns a pointer to the matrix representing the transition probability
	 * function.
	 * @return A pointer to the matrix representing the transition probability
	 * function.
	 */
	std::shared_ptr<storm::storage::SparseMatrix<T>> getTransitionProbabilityMatrix() const {
		return this->probabilityMatrix;
	}

	/*!
	 * Returns a pointer to the matrix representing the transition rewards.
	 * @return A pointer to the matrix representing the transition rewards.
	 */
	std::shared_ptr<storm::storage::SparseMatrix<T>> getTransitionRewardMatrix() const {
		return this->transitionRewardMatrix;
	}

	/*!
	 * Returns a pointer to the vector representing the state rewards.
	 * @return A pointer to the vector representing the state rewards.
	 */
	std::shared_ptr<std::vector<T>> getStateRewards() const {
		return this->stateRewards;
	}

	/*!
	 *
	 */
	std::set<std::string> const getPropositionsForState(uint_fast64_t const &state) const {
		return stateLabeling->getPropositionsForState(state);
	}

	/*!
	 * Retrieves a reference to the backwards transition relation.
	 * @return A reference to the backwards transition relation.
	 */
	storm::models::GraphTransitions<T>& getBackwardTransitions() {
		if (this->backwardTransitions == nullptr) {
			this->backwardTransitions = new storm::models::GraphTransitions<T>(this->probabilityMatrix, false);
		}
		return *this->backwardTransitions;
	}

	/*!
	 * Retrieves whether this DTMC has a state reward model.
	 * @return True if this DTMC has a state reward model.
	 */
	bool hasStateRewards() {
		return this->stateRewards != nullptr;
	}

	/*!
	 * Retrieves whether this DTMC has a transition reward model.
	 * @return True if this DTMC has a transition reward model.
	 */
	bool hasTransitionRewards() {
		return this->transitionRewardMatrix != nullptr;
	}

	/*!
	 * Retrieves whether the given atomic proposition is a valid atomic proposition in this model.
	 * @param atomicProposition The atomic proposition to be checked for validity.
	 * @return True if the given atomic proposition is valid in this model.
	 */
	bool hasAtomicProposition(std::string const& atomicProposition) {
		return this->stateLabeling->containsAtomicProposition(atomicProposition);
	}

	/*!
	 * Prints information about the model to the specified stream.
	 * @param out The stream the information is to be printed to.
	 */
	void printModelInformationToStream(std::ostream& out) const {
		storm::utility::printSeparationLine(out);
		out << std::endl;
		out << "Model type: \t\tDTMC" << std::endl;
		out << "States: \t\t" << this->getNumberOfStates() << std::endl;
		out << "Transitions: \t\t" << this->getNumberOfTransitions() << std::endl;
		this->stateLabeling->printAtomicPropositionsInformationToStream(out);
		out << "Size in memory: \t"
			<< (this->probabilityMatrix->getSizeInMemory() +
				this->stateLabeling->getSizeInMemory() +
				sizeof(*this))/1024 << " kbytes" << std::endl;
		out << std::endl;
		storm::utility::printSeparationLine(out);
	}

private:

	/*!
	 *	@brief Perform some sanity checks.
	 *
	 *	Checks probability matrix if all rows sum up to one.
	 */
	bool checkValidityProbabilityMatrix() {
		for (uint_fast64_t row = 0; row < this->probabilityMatrix->getRowCount(); row++) {
			T sum = this->probabilityMatrix->getRowSum(row);
			if (sum == 0) continue;
			if (std::abs(sum - 1) > 1e-10) return false;
		}
		return true;
	}

	/*! A matrix representing the transition probability function of the DTMC. */
	std::shared_ptr<storm::storage::SparseMatrix<T>> probabilityMatrix;

	/*! The labeling of the states of the DTMC. */
	std::shared_ptr<storm::models::AtomicPropositionsLabeling> stateLabeling;

	/*! The state-based rewards of the DTMC. */
	std::shared_ptr<std::vector<T>> stateRewards;

	/*! The transition-based rewards of the DTMC. */
	std::shared_ptr<storm::storage::SparseMatrix<T>> transitionRewardMatrix;

	/*!
	 * A data structure that stores the predecessors for all states. This is
	 * needed for backwards directed searches.
	 */
	storm::models::GraphTransitions<T>* backwardTransitions;
};

} // namespace models

} // namespace storm

#endif /* STORM_MODELS_DTMC_H_ */
