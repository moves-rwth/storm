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
	/*!
	 * Constructs a CTMC object from the given transition rate matrix and
	 * the given labeling of the states.
	 * @param rateMatrix The transition rate function of the
	 * CTMC given by a matrix.
	 * @param stateLabeling The labeling that assigns a set of atomic
	 * propositions to each state.
	 */
	Ctmc(storm::storage::SparseMatrix<T> const& rateMatrix, storm::models::AtomicPropositionsLabeling const& stateLabeling,
				boost::optional<std::vector<T>> const& optionalStateRewardVector, boost::optional<storm::storage::SparseMatrix<T>> const& optionalTransitionRewardMatrix,
                boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> const& optionalChoiceLabeling)
			: AbstractDeterministicModel<T>(rateMatrix, stateLabeling, optionalStateRewardVector, optionalTransitionRewardMatrix, optionalChoiceLabeling) {
		// Intentionally left empty.
	}

	/*!
	 * Constructs a CTMC object from the given transition rate matrix and
	 * the given labeling of the states.
	 * @param rateMatrix The transition rate function of the
	 * CTMC given by a matrix.
	 * @param stateLabeling The labeling that assigns a set of atomic
	 * propositions to each state.
	 */
	Ctmc(storm::storage::SparseMatrix<T>&& rateMatrix, storm::models::AtomicPropositionsLabeling&& stateLabeling,
				boost::optional<std::vector<T>>&& optionalStateRewardVector, boost::optional<storm::storage::SparseMatrix<T>>&& optionalTransitionRewardMatrix,
                boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>&& optionalChoiceLabeling)
			// The std::move call must be repeated here because otherwise this calls the copy constructor of the Base Class
			: AbstractDeterministicModel<T>(std::move(rateMatrix), std::move(stateLabeling), std::move(optionalStateRewardVector), std::move(optionalTransitionRewardMatrix),
                                            std::move(optionalChoiceLabeling)) {
		// Intentionally left empty.
	}

	/*!
	 * Copy Constructor. Performs a deep copy of the given CTMC.
	 * @param ctmc A reference to the CTMC that is to be copied.
	 */
	Ctmc(Ctmc<T> const & ctmc) : AbstractDeterministicModel<T>(ctmc) {
		// Intentionally left empty.
	}

	/*!
	 * Move Constructor. Performs a move on the given CTMC.
	 * @param ctmc A reference to the CTMC that is to be moved from.
	 */
	Ctmc(Ctmc<T>&& ctmc) : AbstractDeterministicModel<T>(std::move(ctmc)) {
		// Intentionally left empty.
	}

	storm::models::ModelType getType() const {
		return CTMC;
	}

	/*!
	 * Calculates a hash over all values contained in this Model.
	 * @return size_t A Hash Value
	 */
	virtual std::size_t getHash() const override {
		return AbstractDeterministicModel<T>::getHash();
	}
    
    virtual std::shared_ptr<AbstractModel<T>> applyScheduler(storm::storage::Scheduler const& scheduler) const override {
    std::vector<uint_fast64_t> nondeterministicChoiceIndices(this->getNumberOfStates() + 1);
    for (uint_fast64_t state = 0; state < this->getNumberOfStates(); ++state) {
        nondeterministicChoiceIndices[state] = state;
    }
    nondeterministicChoiceIndices[this->getNumberOfStates()] = this->getNumberOfStates();
    storm::storage::SparseMatrix<T> newTransitionMatrix = storm::utility::matrix::applyScheduler(this->getTransitionMatrix(), nondeterministicChoiceIndices, scheduler);
    
    return std::shared_ptr<AbstractModel<T>>(new Ctmc(newTransitionMatrix, this->getStateLabeling(), this->hasStateRewards() ? this->getStateRewardVector() : boost::optional<std::vector<T>>(), this->hasTransitionRewards() ? this->getTransitionRewardMatrix() :  boost::optional<storm::storage::SparseMatrix<T>>(), this->hasChoiceLabeling() ? this->getChoiceLabeling() : boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>()));
}
};

} // namespace models

} // namespace storm

#endif /* STORM_MODELS_DTMC_H_ */
