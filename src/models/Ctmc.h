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

#include "AbstractDeterministicModel.h"

namespace storm {

namespace models {

/*!
 * This class represents a continuous-time Markov chain (CTMC) whose states are
 * labeled with atomic propositions.
 */
template <class T>
class Ctmc : public storm::models::AbstractDeterministicModel<T> {

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
			std::shared_ptr<std::vector<T>> stateRewardVector = nullptr,
			std::shared_ptr<storm::storage::SparseMatrix<T>> transitionRewardMatrix = nullptr)
			: AbstractDeterministicModel<T>(rateMatrix, stateLabeling, stateRewardVector, transitionRewardMatrix) {
	}

	//! Copy Constructor
	/*!
	 * Copy Constructor. Performs a deep copy of the given CTMC.
	 * @param ctmc A reference to the CTMC that is to be copied.
	 */
	Ctmc(const Ctmc<T> &ctmc) : AbstractDeterministicModel<T>(ctmc) {
		// Intentionally left empty.
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
		this->getStateLabeling()->printAtomicPropositionsInformationToStream(out);
		out << "Size in memory: \t"
			<< (this->getTransitionMatrix()->getSizeInMemory() +
				this->stateLabeling()->getSizeInMemory() +
				sizeof(*this))/1024 << " kbytes" << std::endl;
		out << "-------------------------------------------------------------- "
			<< std::endl;
	}

	storm::models::ModelType getType() {
		return CTMC;
	}
};

} // namespace models

} // namespace storm

#endif /* STORM_MODELS_DTMC_H_ */
