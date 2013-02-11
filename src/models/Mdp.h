/*
 * Mdp.h
 *
 *  Created on: 14.01.2013
 *      Author: Philipp Berger
 */

#ifndef STORM_MODELS_MDP_H_
#define STORM_MODELS_MDP_H_

#include <ostream>
#include <iostream>
#include <memory>
#include <cstdlib>

#include "AtomicPropositionsLabeling.h"
#include "src/storage/SparseMatrix.h"
#include "src/utility/Settings.h"
#include "src/models/AbstractNondeterministicModel.h"

namespace storm {

namespace models {

/*!
 * This class represents a Markov Decision Process (MDP) whose states are
 * labeled with atomic propositions.
 */
template <class T>
class Mdp : public storm::models::AbstractNondeterministicModel<T> {

public:
	//! Constructor
	/*!
	 * Constructs a MDP object from the given transition probability matrix and
	 * the given labeling of the states.
	 * @param probabilityMatrix The transition probability relation of the
	 * MDP given by a matrix.
	 * @param stateLabeling The labeling that assigns a set of atomic
	 * propositions to each state.
	 */
	Mdp(std::shared_ptr<storm::storage::SparseMatrix<T>> probabilityMatrix,
			std::shared_ptr<storm::models::AtomicPropositionsLabeling> stateLabeling,
			std::shared_ptr<std::vector<uint_fast64_t>> choiceIndices,
			std::shared_ptr<std::vector<T>> stateRewardVector = nullptr,
			std::shared_ptr<storm::storage::SparseMatrix<T>> transitionRewardMatrix = nullptr)
			: AbstractNondeterministicModel<T>(probabilityMatrix, stateLabeling, choiceIndices, stateRewardVector, transitionRewardMatrix) {
		if (!this->checkValidityOfProbabilityMatrix()) {
			LOG4CPLUS_ERROR(logger, "Probability matrix is invalid.");
			throw storm::exceptions::InvalidArgumentException() << "Probability matrix is invalid.";
		}
	}

	//! Copy Constructor
	/*!
	 * Copy Constructor. Performs a deep copy of the given MDP.
	 * @param mdp A reference to the MDP that is to be copied.
	 */
	Mdp(const Mdp<T> &mdp) : AbstractNondeterministicModel<T>(mdp) {
		if (!this->checkValidityOfProbabilityMatrix()) {
			LOG4CPLUS_ERROR(logger, "Probability matrix is invalid.");
			throw storm::exceptions::InvalidArgumentException() << "Probability matrix is invalid.";
		}
	}

	//! Destructor
	/*!
	 * Destructor.
	 */
	~Mdp() {
		// Intentionally left empty.
	}

	storm::models::ModelType getType() const {
		return MDP;
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
		for (uint_fast64_t row = 0; row < this->getTransitionMatrix()->getRowCount(); row++) {
			T sum = this->getTransitionMatrix()->getRowSum(row);
			if (sum == 0) continue;
			if (std::abs(sum - 1) > precision) return false;
		}
		return true;
	}
};

} // namespace models

} // namespace storm

#endif /* STORM_MODELS_MDP_H_ */
