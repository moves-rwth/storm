/*
 * Ctmdp.h
 *
 *  Created on: 14.01.2013
 *      Author: Philipp Berger
 */

#ifndef STORM_MODELS_CTMDP_H_
#define STORM_MODELS_CTMDP_H_

#include <memory>
#include <vector>

#include "AtomicPropositionsLabeling.h"
#include "AbstractNondeterministicModel.h"
#include "src/storage/SparseMatrix.h"
#include "src/utility/Settings.h"

namespace storm {

namespace models {

/*!
 * This class represents a Markov Decision Process (CTMDP) whose states are
 * labeled with atomic propositions.
 */
template <class T>
class Ctmdp : public storm::models::AbstractNondeterministicModel<T> {

public:
	/*!
	 * Constructs a CTMDP object from the given transition probability matrix and
	 * the given labeling of the states.
	 * All values are copied.
	 * @param probabilityMatrix The transition probability relation of the
	 * CTMDP given by a matrix.
	 * @param stateLabeling The labeling that assigns a set of atomic
	 * propositions to each state.
	 */
	Ctmdp(storm::storage::SparseMatrix<T> const& probabilityMatrix, 
			storm::models::AtomicPropositionsLabeling const& stateLabeling,
			std::vector<uint_fast64_t> const& nondeterministicChoiceIndices,
			boost::optional<std::vector<T>> const& optionalStateRewardVector, 
			boost::optional<storm::storage::SparseMatrix<T>> const& optionalTransitionRewardMatrix)
			: AbstractNondeterministicModel<T>(probabilityMatrix, stateLabeling, nondeterministicChoiceIndices, optionalStateRewardVector, optionalTransitionRewardMatrix) {
		if (!this->checkValidityOfProbabilityMatrix()) {
			LOG4CPLUS_ERROR(logger, "Probability matrix is invalid.");
			throw storm::exceptions::InvalidArgumentException() << "Probability matrix is invalid.";
		}
	}

	/*!
	 * Constructs a CTMDP object from the given transition probability matrix and
	 * the given labeling of the states.
	 * All values are moved.
	 * @param probabilityMatrix The transition probability relation of the
	 * CTMDP given by a matrix.
	 * @param stateLabeling The labeling that assigns a set of atomic
	 * propositions to each state.
	 */
	Ctmdp(storm::storage::SparseMatrix<T>&& probabilityMatrix, 
			storm::models::AtomicPropositionsLabeling&& stateLabeling,
			std::vector<uint_fast64_t>&& nondeterministicChoiceIndices,
			boost::optional<std::vector<T>>&& optionalStateRewardVector, 
			boost::optional<storm::storage::SparseMatrix<T>>&& optionalTransitionRewardMatrix)
			: AbstractNondeterministicModel<T>(probabilityMatrix, stateLabeling, nondeterministicChoiceIndices, optionalStateRewardVector, optionalTransitionRewardMatrix) {
		if (!this->checkValidityOfProbabilityMatrix()) {
			LOG4CPLUS_ERROR(logger, "Probability matrix is invalid.");
			throw storm::exceptions::InvalidArgumentException() << "Probability matrix is invalid.";
		}
	}

	/*!
	 * Copy Constructor. Performs a deep copy of the given CTMDP.
	 * @param ctmdp A reference to the CTMDP that is to be copied.
	 */
	Ctmdp(Ctmdp<T> const & ctmdp) : AbstractNondeterministicModel<T>(ctmdp) {
		if (!this->checkValidityOfProbabilityMatrix()) {
			LOG4CPLUS_ERROR(logger, "Probability matrix is invalid.");
			throw storm::exceptions::InvalidArgumentException() << "Probability matrix is invalid.";
		}
	}

	/*!
	 * Move Constructor. Performs a move on the given CTMDP.
	 * @param ctmdp A reference to the CTMDP that is to be moved.
	 */
	Ctmdp(Ctmdp<T>&& ctmdp) : AbstractNondeterministicModel<T>(ctmdp) {
		if (!this->checkValidityOfProbabilityMatrix()) {
			LOG4CPLUS_ERROR(logger, "Probability matrix is invalid.");
			throw storm::exceptions::InvalidArgumentException() << "Probability matrix is invalid.";
		}
	}

	/*!
	 * Destructor.
	 */
	~Ctmdp() {
		// Intentionally left empty.
	}

	storm::models::ModelType getType() const {
		return CTMDP;
	}

	/*!
	 * Calculates a hash over all values contained in this Model.
	 * @return size_t A Hash Value
	 */
	virtual std::size_t getHash() const override {
		return AbstractNondeterministicModel::getHash();
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
			if (std::abs(sum - 1) > precision) return false;
		}
		return true;
	}
};

} // namespace models

} // namespace storm

#endif /* STORM_MODELS_CTMDP_H_ */
