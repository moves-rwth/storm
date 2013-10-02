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
#include "src/settings/Settings.h"

namespace storm {

namespace models {

/*!
 * This class represents a discrete-time Markov chain (DTMC) whose states are
 * labeled with atomic propositions.
 */
template <class T>
class Dtmc : public storm::models::AbstractDeterministicModel<T> {

public:
	/*!
	 * Constructs a DTMC object from the given transition probability matrix and
	 * the given labeling of the states.
	 * All values are copied.
	 * @param probabilityMatrix The matrix representing the transitions in the model.
	 * @param stateLabeling The labeling that assigns a set of atomic
	 * propositions to each state.
	 * @param stateRewardVector The reward values associated with the states.
	 * @param transitionRewardMatrix The reward values associated with the transitions of the model.
	 */
	Dtmc(storm::storage::SparseMatrix<T> const& probabilityMatrix, storm::models::AtomicPropositionsLabeling const& stateLabeling,
				boost::optional<std::vector<T>> const& optionalStateRewardVector, boost::optional<storm::storage::SparseMatrix<T>> const& optionalTransitionRewardMatrix,
                boost::optional<std::vector<std::set<uint_fast64_t>>> const& optionalChoiceLabeling)
			: AbstractDeterministicModel<T>(probabilityMatrix, stateLabeling, optionalStateRewardVector, optionalTransitionRewardMatrix, optionalChoiceLabeling) {
		if (!this->checkValidityOfProbabilityMatrix()) {
			LOG4CPLUS_ERROR(logger, "Probability matrix is invalid.");
			throw storm::exceptions::InvalidArgumentException() << "Probability matrix is invalid.";
		}
		if (this->hasTransitionRewards()) {
			if (!this->getTransitionRewardMatrix().isSubmatrixOf(this->getTransitionMatrix())) {
				LOG4CPLUS_ERROR(logger, "Transition reward matrix is not a submatrix of the transition matrix, i.e. there are rewards for transitions that do not exist.");
				throw storm::exceptions::InvalidArgumentException() << "There are transition rewards for nonexistent transitions.";
			}
		}
	}

	/*!
	 * Constructs a DTMC object from the given transition probability matrix and
	 * the given labeling of the states.
	 * All values are moved.
	 * @param probabilityMatrix The matrix representing the transitions in the model.
	 * @param stateLabeling The labeling that assigns a set of atomic
	 * propositions to each state.
	 * @param stateRewardVector The reward values associated with the states.
	 * @param transitionRewardMatrix The reward values associated with the transitions of the model.
	 */
	Dtmc(storm::storage::SparseMatrix<T>&& probabilityMatrix, storm::models::AtomicPropositionsLabeling&& stateLabeling,
				boost::optional<std::vector<T>>&& optionalStateRewardVector, boost::optional<storm::storage::SparseMatrix<T>>&& optionalTransitionRewardMatrix,
                boost::optional<std::vector<std::set<uint_fast64_t>>>&& optionalChoiceLabeling)
				// The std::move call must be repeated here because otherwise this calls the copy constructor of the Base Class
			: AbstractDeterministicModel<T>(std::move(probabilityMatrix), std::move(stateLabeling), std::move(optionalStateRewardVector), std::move(optionalTransitionRewardMatrix),
                                            std::move(optionalChoiceLabeling)) {
		if (!this->checkValidityOfProbabilityMatrix()) {
			LOG4CPLUS_ERROR(logger, "Probability matrix is invalid.");
			throw storm::exceptions::InvalidArgumentException() << "Probability matrix is invalid.";
		}
		if (this->hasTransitionRewards()) {
			if (!this->getTransitionRewardMatrix().isSubmatrixOf(this->getTransitionMatrix())) {
				LOG4CPLUS_ERROR(logger, "Transition reward matrix is not a submatrix of the transition matrix, i.e. there are rewards for transitions that do not exist.");
				throw storm::exceptions::InvalidArgumentException() << "There are transition rewards for nonexistent transitions.";
			}
		}
	}

	/*!
	 * Copy Constructor. Performs a deep copy of the given DTMC.
	 * @param dtmc A reference to the DTMC that is to be copied.
	 */
	Dtmc(Dtmc<T> const & dtmc) : AbstractDeterministicModel<T>(dtmc) {
		// Intentionally left empty.
	}

	/*!
	 * Move Constructor. Performs a move on the given DTMC.
	 * @param dtmc A reference to the DTMC that is to be moved.
	 */
	Dtmc(Dtmc<T>&& dtmc) : AbstractDeterministicModel<T>(std::move(dtmc)) {
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

	/*!
	 * Calculates a hash over all values contained in this Model.
	 * @return size_t A Hash Value
	 */
	virtual std::size_t getHash() const override {
		return AbstractDeterministicModel<T>::getHash();
	}

private:
	/*!
	 *	@brief Perform some sanity checks.
	 *
	 *	Checks probability matrix if all rows sum up to one.
	 */
	bool checkValidityOfProbabilityMatrix() {
		// Get the settings object to customize linear solving.
		storm::settings::Settings* s = storm::settings::Settings::getInstance();
		double precision = s->getOptionByLongName("precision").getArgument(0).getValueAsDouble();

		if (this->getTransitionMatrix().getRowCount() != this->getTransitionMatrix().getColumnCount()) {
			// not square
			LOG4CPLUS_ERROR(logger, "Probability matrix is not square.");
			return false;
		}

		for (uint_fast64_t row = 0; row < this->getTransitionMatrix().getRowCount(); ++row) {
			T sum = this->getTransitionMatrix().getRowSum(row);
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
