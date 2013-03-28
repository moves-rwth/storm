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

#include "AbstractDeterministicModel.h"
#include "AtomicPropositionsLabeling.h"
#include "src/storage/SparseMatrix.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/utility/Settings.h"

namespace storm {

namespace models {

/*!
 * This class represents a discrete-time Markov chain (DTMC) whose states are
 * labeled with atomic propositions.
 */
template <class T>
class Dtmc : public storm::models::AbstractDeterministicModel<T> {

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
			std::shared_ptr<std::vector<T>> stateRewardVector = nullptr,
			std::shared_ptr<storm::storage::SparseMatrix<T>> transitionRewardMatrix = nullptr)
			: AbstractDeterministicModel<T>(probabilityMatrix, stateLabeling, stateRewardVector, transitionRewardMatrix) {
		if (!this->checkValidityOfProbabilityMatrix()) {
			LOG4CPLUS_ERROR(logger, "Probability matrix is invalid.");
			throw storm::exceptions::InvalidArgumentException() << "Probability matrix is invalid.";
		}
		if (this->getTransitionRewardMatrix() != nullptr) {
			if (!this->getTransitionRewardMatrix()->containsAllPositionsOf(*this->getTransitionMatrix())) {
				LOG4CPLUS_ERROR(logger, "Transition reward matrix is not a submatrix of the transition matrix, i.e. there are rewards for transitions that do not exist.");
				throw storm::exceptions::InvalidArgumentException() << "There are transition rewards for nonexistent transitions.";
			}
		}
	}

	//! Copy Constructor
	/*!
	 * Copy Constructor. Performs a deep copy of the given DTMC.
	 * @param dtmc A reference to the DTMC that is to be copied.
	 */
	Dtmc(const Dtmc<T> &dtmc) : AbstractDeterministicModel<T>(dtmc) {
		// Intentionally left empty.
	}

	//! Destructor
	/*!
	 * Destructor.
	 */
	~Dtmc() {
		// Intentionally left empty.
	}
	
	storm::models::ModelType getType() const {
		return DTMC;
	}

private:
	/*!
	 *	@brief Perform some sanity checks.
	 *
	 *	Checks probability matrix if all rows sum up to one.
	 */
	bool checkValidityOfProbabilityMatrix() {
		// Get the settings object to customize linear solving.
		storm::settings::Settings* s = storm::settings::instance();
		double precision = s->get<double>("precision");

		if (this->getTransitionMatrix()->getRowCount() != this->getTransitionMatrix()->getColumnCount()) {
			// not square
			LOG4CPLUS_ERROR(logger, "Probability matrix is not square.");
			return false;
		}

		for (uint_fast64_t row = 0; row < this->getTransitionMatrix()->getRowCount(); row++) {
			T sum = this->getTransitionMatrix()->getRowSum(row);
			if (sum == 0) {
				LOG4CPLUS_ERROR(logger, "Row " << row << " has sum 0");
				return false;
			}
			if (std::abs(sum - 1) > precision) {
				LOG4CPLUS_ERROR(logger, "Row " << row << " has sum " << sum);
				return false;
			}
		}
		return true;
	}
};

} // namespace models

} // namespace storm

#endif /* STORM_MODELS_DTMC_H_ */
