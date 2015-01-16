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
#include "src/settings/SettingsManager.h"

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
			boost::optional<std::vector<T>> const& optionalStateRewardVector,
			boost::optional<storm::storage::SparseMatrix<T>> const& optionalTransitionRewardMatrix,
            boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> const& optionalChoiceLabeling)
			: AbstractNondeterministicModel<T>(probabilityMatrix, stateLabeling, optionalStateRewardVector, optionalTransitionRewardMatrix,
                                               optionalChoiceLabeling) {
                // Intentionally left empty.
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
			boost::optional<std::vector<T>>&& optionalStateRewardVector,
			boost::optional<storm::storage::SparseMatrix<T>>&& optionalTransitionRewardMatrix,
            boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> const& optionalChoiceLabeling)
			// The std::move call must be repeated here because otherwise this calls the copy constructor of the Base Class
			: AbstractNondeterministicModel<T>(std::move(probabilityMatrix), std::move(stateLabeling), std::move(optionalStateRewardVector), std::move(optionalTransitionRewardMatrix), std::move(optionalChoiceLabeling)) {
                // Intentionally left empty.
	}

	/*!
	 * Copy Constructor. Performs a deep copy of the given CTMDP.
	 * @param ctmdp A reference to the CTMDP that is to be copied.
	 */
	Ctmdp(Ctmdp<T> const & ctmdp) : AbstractNondeterministicModel<T>(ctmdp) {
        // Intentionally left empty.
	}

	/*!
	 * Move Constructor. Performs a move on the given CTMDP.
	 * @param ctmdp A reference to the CTMDP that is to be moved.
	 */
	Ctmdp(Ctmdp<T>&& ctmdp) : AbstractNondeterministicModel<T>(std::move(ctmdp)) {
        // Intentionally left empty.
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
		return AbstractNondeterministicModel<T>::getHash();
	}

    virtual std::shared_ptr<AbstractModel<T>> applyScheduler(storm::storage::Scheduler const& scheduler) const override {
        storm::storage::SparseMatrix<T> newTransitionMatrix = storm::utility::matrix::applyScheduler(this->getTransitionMatrix(), scheduler);
        return std::shared_ptr<AbstractModel<T>>(new Ctmdp(newTransitionMatrix, this->getStateLabeling(), this->hasStateRewards() ? this->getStateRewardVector() : boost::optional<std::vector<T>>(), this->hasTransitionRewards() ? this->getTransitionRewardMatrix() :  boost::optional<storm::storage::SparseMatrix<T>>(), this->hasChoiceLabeling() ? this->getChoiceLabeling() : boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>()));
    }
};

} // namespace models

} // namespace storm

#endif /* STORM_MODELS_CTMDP_H_ */
