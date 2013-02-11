/*
 * Ctmc.h
 *
 *  Created on: 14.11.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_MODELS_CTMC_H_
#define STORM_MODELS_CTMC_H_

#include <memory>
#include <vector>

#include "AbstractDeterministicModel.h"
#include "AtomicPropositionsLabeling.h"
#include "src/storage/SparseMatrix.h"

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

	storm::models::ModelType getType() const {
		return CTMC;
	}
};

} // namespace models

} // namespace storm

#endif /* STORM_MODELS_DTMC_H_ */
