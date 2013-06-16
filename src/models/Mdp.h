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
	/*!
	 * Constructs a MDP object from the given transition probability matrix and
	 * the given labeling of the states.
	 * All values are copied.
	 * @param probabilityMatrix The transition probability relation of the
	 * MDP given by a matrix.
	 * @param stateLabeling The labeling that assigns a set of atomic
	 * propositions to each state.
	 */
	Mdp(storm::storage::SparseMatrix<T> const& transitionMatrix, 
			storm::models::AtomicPropositionsLabeling const& stateLabeling,
			std::vector<uint_fast64_t> const& nondeterministicChoiceIndices,
			boost::optional<std::vector<T>> const& optionalStateRewardVector, 
			boost::optional<storm::storage::SparseMatrix<T>> const& optionalTransitionRewardMatrix)
			: AbstractNondeterministicModel<T>(transitionMatrix, stateLabeling, nondeterministicChoiceIndices, optionalStateRewardVector, optionalTransitionRewardMatrix) {
		if (!this->checkValidityOfProbabilityMatrix()) {
			LOG4CPLUS_ERROR(logger, "Probability matrix is invalid.");
			throw storm::exceptions::InvalidArgumentException() << "Probability matrix is invalid.";
		}
	}

	/*!
	 * Constructs a MDP object from the given transition probability matrix and
	 * the given labeling of the states.
	 * All values are moved.
	 * @param probabilityMatrix The transition probability relation of the
	 * MDP given by a matrix.
	 * @param stateLabeling The labeling that assigns a set of atomic
	 * propositions to each state.
	 */
	Mdp(storm::storage::SparseMatrix<T>&& transitionMatrix, 
			storm::models::AtomicPropositionsLabeling&& stateLabeling,
			std::vector<uint_fast64_t>&& nondeterministicChoiceIndices,
			boost::optional<std::vector<T>>&& optionalStateRewardVector, 
			boost::optional<storm::storage::SparseMatrix<T>>&& optionalTransitionRewardMatrix)
			// The std::move call must be repeated here because otherwise this calls the copy constructor of the Base Class
			: AbstractNondeterministicModel<T>(std::move(transitionMatrix), std::move(stateLabeling), std::move(nondeterministicChoiceIndices), std::move(optionalStateRewardVector), std::move(optionalTransitionRewardMatrix)) {
		if (!this->checkValidityOfProbabilityMatrix()) {
			LOG4CPLUS_ERROR(logger, "Probability matrix is invalid.");
			throw storm::exceptions::InvalidArgumentException() << "Probability matrix is invalid.";
		}
	}

	/*!
	 * Copy Constructor. Performs a deep copy of the given MDP.
	 * @param mdp A reference to the MDP that is to be copied.
	 */
	Mdp(Mdp<T> const & mdp) : AbstractNondeterministicModel<T>(mdp) {
		if (!this->checkValidityOfProbabilityMatrix()) {
			LOG4CPLUS_ERROR(logger, "Probability matrix is invalid.");
			throw storm::exceptions::InvalidArgumentException() << "Probability matrix is invalid.";
		}
	}

	/*!
	 * Move Constructor. Performs a move on the given MDP.
	 * @param mdp A reference to the MDP that is to be moved.
	 */
	Mdp(Mdp<T>&& mdp) : AbstractNondeterministicModel<T>(std::move(mdp)) {
		if (!this->checkValidityOfProbabilityMatrix()) {
			LOG4CPLUS_ERROR(logger, "Probability matrix is invalid.");
			throw storm::exceptions::InvalidArgumentException() << "Probability matrix is invalid.";
		}
	}

	/*!
	 * Destructor.
	 */
	~Mdp() {
		// Intentionally left empty.
	}

	storm::models::ModelType getType() const {
		return MDP;
	}

	/*!
	 * Calculates a hash over all values contained in this Model.
	 * @return size_t A Hash Value
	 */
	virtual std::size_t getHash() const override {
		return AbstractNondeterministicModel<T>::getHash();
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
		for (uint_fast64_t row = 0; row < this->getTransitionMatrix().getRowCount(); row++) {
			T sum = this->getTransitionMatrix().getRowSum(row);
			if (sum == 0) continue;
			if (std::abs(sum - 1) > precision)  {
				return false;
			}
		}
		return true;
	}
};

} // namespace models

} // namespace storm

#endif /* STORM_MODELS_MDP_H_ */
