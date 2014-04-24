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
#include <algorithm>

#include "AtomicPropositionsLabeling.h"
#include "src/storage/SparseMatrix.h"
#include "src/settings/Settings.h"
#include "src/models/AbstractNondeterministicModel.h"
#include "src/utility/matrix.h"

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
	 * Constructs a MDP object from the given transition probability matrix and the given labeling of the states.
	 * All values are copied.
     *
	 * @param probabilityMatrix The transition probability relation of the MDP given by a matrix.
	 * @param stateLabeling The labeling that assigns a set of atomic propositions to each state.
     * @param optionalStateRewardVector A vector assigning rewards to states.
     * @param optionalTransitionRewardVector A sparse matrix that represents an assignment of rewards to the transitions.
     * @param optionalChoiceLabeling A vector that represents the labels associated with each nondeterministic choice of
     * a state.
	 */
	Mdp(storm::storage::SparseMatrix<T> const& transitionMatrix, 
			storm::models::AtomicPropositionsLabeling const& stateLabeling,
			boost::optional<std::vector<T>> const& optionalStateRewardVector,
			boost::optional<storm::storage::SparseMatrix<T>> const& optionalTransitionRewardMatrix,
            boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> const& optionalChoiceLabeling)
			: AbstractNondeterministicModel<T>(transitionMatrix, stateLabeling, optionalStateRewardVector, optionalTransitionRewardMatrix, optionalChoiceLabeling) {
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
			boost::optional<std::vector<T>>&& optionalStateRewardVector,
			boost::optional<storm::storage::SparseMatrix<T>>&& optionalTransitionRewardMatrix,
            boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>&& optionalChoiceLabeling)
			// The std::move call must be repeated here because otherwise this calls the copy constructor of the Base Class
			: AbstractNondeterministicModel<T>(std::move(transitionMatrix), std::move(stateLabeling), std::move(optionalStateRewardVector), std::move(optionalTransitionRewardMatrix),
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
     * Constructs an MDP by copying the current MDP and restricting the choices of each state to the ones whose label set
     * is contained in the given label set.
     *
     * @param enabledChoiceLabels A set of labels that determines which choices of the original model can be taken
     * and which ones need to be ignored.
     * @return A restricted version of the current MDP that only uses choice labels from the given set.
     */
    Mdp<T> restrictChoiceLabels(boost::container::flat_set<uint_fast64_t> const& enabledChoiceLabels) const {
        // Only perform this operation if the given model has choice labels.
        if (!this->hasChoiceLabeling()) {
            throw storm::exceptions::InvalidArgumentException() << "Restriction to label set is impossible for unlabeled model.";
        }
        
        std::vector<boost::container::flat_set<uint_fast64_t>> const& choiceLabeling = this->getChoiceLabeling();
        
        storm::storage::SparseMatrixBuilder<T> transitionMatrixBuilder;
        std::vector<boost::container::flat_set<uint_fast64_t>> newChoiceLabeling;
        
        // Check for each choice of each state, whether the choice labels are fully contained in the given label set.
        uint_fast64_t currentRow = 0;
        for(uint_fast64_t state = 0; state < this->getNumberOfStates(); ++state) {
            bool stateHasValidChoice = false;
            for (uint_fast64_t choice = this->getTransitionMatrix().getRowGroupIndices()[state]; choice < this->getTransitionMatrix().getRowGroupIndices()[state + 1]; ++choice) {
                bool choiceValid = std::includes(enabledChoiceLabels.begin(), enabledChoiceLabels.end(), choiceLabeling[choice].begin(), choiceLabeling[choice].end());
                
                // If the choice is valid, copy over all its elements.
                if (choiceValid) {
                    if (!stateHasValidChoice) {
                        transitionMatrixBuilder.newRowGroup(currentRow);
                    }
                    stateHasValidChoice = true;
                    for (auto const& entry : this->getTransitionMatrix().getRow(choice)) {
                        transitionMatrixBuilder.addNextValue(currentRow, entry.first, entry.second);
                    }
                    newChoiceLabeling.emplace_back(choiceLabeling[choice]);
                    ++currentRow;
                } 
            }
            
            // If no choice of the current state may be taken, we insert a self-loop to the state instead.
            if (!stateHasValidChoice) {
                transitionMatrixBuilder.newRowGroup(currentRow);
                transitionMatrixBuilder.addNextValue(currentRow, state, storm::utility::constantOne<T>());
                newChoiceLabeling.emplace_back();
                ++currentRow;
            }
        }
        
        Mdp<T> restrictedMdp(transitionMatrixBuilder.build(), storm::models::AtomicPropositionsLabeling(this->getStateLabeling()), this->hasStateRewards() ? boost::optional<std::vector<T>>(this->getStateRewardVector()) : boost::optional<std::vector<T>>(), this->hasTransitionRewards() ? boost::optional<storm::storage::SparseMatrix<T>>(this->getTransitionRewardMatrix()) : boost::optional<storm::storage::SparseMatrix<T>>(), boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>(newChoiceLabeling));
        return restrictedMdp;
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
        return std::shared_ptr<AbstractModel<T>>(new Mdp(newTransitionMatrix, this->getStateLabeling(), this->hasStateRewards() ? this->getStateRewardVector() : boost::optional<std::vector<T>>(), this->hasTransitionRewards() ? this->getTransitionRewardMatrix() :  boost::optional<storm::storage::SparseMatrix<T>>(), this->hasChoiceLabeling() ? this->getChoiceLabeling() : boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>()));
    }
    
private:

	/*!
	 *	@brief Perform some sanity checks.
	 *
	 *	Checks probability matrix if all rows sum up to one.
	 */
	bool checkValidityOfProbabilityMatrix() {
		// Get the settings object to customize linear solving.
		for (uint_fast64_t row = 0; row < this->getTransitionMatrix().getRowCount(); row++) {
			T sum = this->getTransitionMatrix().getRowSum(row);
                        
			if (sum == T(0)) continue;
			if (!storm::utility::isOne(sum))  {
				return false;
			}
																																																																																																																																											}
		return true;
	}
};

} // namespace models

} // namespace storm

#endif /* STORM_MODELS_MDP_H_ */
