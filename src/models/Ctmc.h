/*
 * Ctmc.h
 *
 *  Created on: 14.11.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_MODELS_CTMC_H_
#define STORM_MODELS_CTMC_H_

#include <ostream>
#include <iostream>
#include <memory>
#include <cstdlib>

#include "AtomicPropositionsLabeling.h"
#include "GraphTransitions.h"
#include "src/storage/SparseMatrix.h"
#include "src/exceptions/InvalidArgumentException.h"

#include "src/models/AbstractModel.h"

namespace storm {

namespace models {

/*!
 * This class represents a discrete-time Markov chain (DTMC) whose states are
 * labeled with atomic propositions.
 */
template <class T>
class Ctmc : public storm::models::AbstractModel {

public:
	//! Constructor
	/*!
	 * Constructs a CTMC object from the given transition rate matrix and
	 * the given labeling of the states.
	 * @param rateMatrix The transition rate function of the
	 * CTMC given by a matrix.
	 * @param stateLabeling The labeling that assigns a set of atomic
	 * propositions to each state.
	 */
	Ctmc(std::shared_ptr<storm::storage::SparseMatrix<T>> rateMatrix,
			std::shared_ptr<storm::models::AtomicPropositionsLabeling> stateLabeling,
			std::shared_ptr<std::vector<T>> stateRewards = nullptr,
			std::shared_ptr<storm::storage::SparseMatrix<T>> transitionRewardMatrix = nullptr)
			: rateMatrix(rateMatrix), stateLabeling(stateLabeling),
			  stateRewards(stateRewards), transitionRewardMatrix(transitionRewardMatrix),
			  backwardTransitions(nullptr) {
	}

	//! Copy Constructor
	/*!
	 * Copy Constructor. Performs a deep copy of the given CTMC.
	 * @param ctmc A reference to the CTMC that is to be copied.
	 */
	Ctmc(const Ctmc<T> &ctmc) : rateMatrix(ctmc.rateMatrix),
			stateLabeling(ctmc.stateLabeling), stateRewards(ctmc.stateRewards),
			transitionRewardMatrix(ctmc.transitionRewardMatrix) {
		if (ctmc.backwardTransitions != nullptr) {
			this->backwardTransitions = new storm::models::GraphTransitions<T>(*ctmc.backwardTransitions);
		}
	}

	//! Destructor
	/*!
	 * Destructor. Frees the matrix and labeling associated with this CTMC.
	 */
	~Ctmc() {
		if (this->backwardTransitions != nullptr) {
			delete this->backwardTransitions;
		}
	}
	
	/*!
	 * Returns the state space size of the CTMC.
	 * @return The size of the state space of the CTMC.
	 */
	uint_fast64_t getNumberOfStates() const {
		return this->rateMatrix->getRowCount();
	}

	/*!
	 * Returns the number of (non-zero) transitions of the CTMC.
	 * @return The number of (non-zero) transitions of the CTMC.
	 */
	uint_fast64_t getNumberOfTransitions() const {
		return this->rateMatrix->getNonZeroEntryCount();
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
	std::shared_ptr<storm::storage::SparseMatrix<T>> getTransitionRateMatrix() const {
		return this->rateMatrix;
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
	 * Prints information about the model to the specified stream.
	 * @param out The stream the information is to be printed to.
	 */
	void printModelInformationToStream(std::ostream& out) const {
		out << "-------------------------------------------------------------- "
			<< std::endl;
		out << "Model type: \t\tCTMC" << std::endl;
		out << "States: \t\t" << this->getNumberOfStates() << std::endl;
		out << "Transitions: \t\t" << this->getNumberOfTransitions() << std::endl;
		this->stateLabeling->printAtomicPropositionsInformationToStream(out);
		out << "Size in memory: \t"
			<< (this->rateMatrix->getSizeInMemory() +
				this->stateLabeling->getSizeInMemory() +
				sizeof(*this))/1024 << " kbytes" << std::endl;
		out << "-------------------------------------------------------------- "
			<< std::endl;
	}

	storm::models::ModelType getType() {
		return CTMC;
	}

private:

	/*! A matrix representing the transition rate function of the CTMC. */
	std::shared_ptr<storm::storage::SparseMatrix<T>> rateMatrix;

	/*! The labeling of the states of the CTMC. */
	std::shared_ptr<storm::models::AtomicPropositionsLabeling> stateLabeling;

	/*! The state-based rewards of the CTMC. */
	std::shared_ptr<std::vector<T>> stateRewards;

	/*! The transition-based rewards of the CTMC. */
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
