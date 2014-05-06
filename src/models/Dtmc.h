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
#include "src/utility/vector.h"
#include "src/utility/matrix.h"

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
	 * @param optionalStateRewardVector The reward values associated with the states.
	 * @param optionalTransitionRewardMatrix The reward values associated with the transitions of the model.
	 * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
	 */
	Dtmc(storm::storage::SparseMatrix<T> const& probabilityMatrix, storm::models::AtomicPropositionsLabeling const& stateLabeling,
				boost::optional<std::vector<T>> const& optionalStateRewardVector, boost::optional<storm::storage::SparseMatrix<T>> const& optionalTransitionRewardMatrix,
                boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> const& optionalChoiceLabeling)
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
	 * @param optionalStateRewardVector The reward values associated with the states.
	 * @param optionalTransitionRewardMatrix The reward values associated with the transitions of the model.
	 * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
	 */
	Dtmc(storm::storage::SparseMatrix<T>&& probabilityMatrix, storm::models::AtomicPropositionsLabeling&& stateLabeling,
				boost::optional<std::vector<T>>&& optionalStateRewardVector, boost::optional<storm::storage::SparseMatrix<T>>&& optionalTransitionRewardMatrix,
                boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>&& optionalChoiceLabeling)
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

	/*!
	 * Generates a sub-Dtmc of this Dtmc induced by the states specified by the bitvector.
	 * E.g. a Dtmc that is partial isomorph (on the given states) to this one.
	 * @param subSysStates A BitVector where all states that should be kept are indicated 
	 *                     by a set bit of the corresponding index.
	 *                     Waring: If the vector does not have the correct size, it will be resized.
	 * @return The sub-Dtmc.
	 */
	storm::models::Dtmc<T> getSubDtmc(storm::storage::BitVector& subSysStates) {


		// Is there any state in the subsystem?
		if(subSysStates.getNumberOfSetBits() == 0) {
			LOG4CPLUS_ERROR(logger, "No states in subsystem!");
			return storm::models::Dtmc<T>(storm::storage::SparseMatrix<T>(),
					  	  	  	  	  	  storm::models::AtomicPropositionsLabeling(this->getStateLabeling(), subSysStates),
					  	  	  	  	  	  boost::optional<std::vector<T>>(),
					  	  	  	  	  	  boost::optional<storm::storage::SparseMatrix<T>>(),
					  	  	  	  	  	  boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>());
		}

		// Does the vector have the right size?
		if(subSysStates.size() != this->getNumberOfStates()) {
			LOG4CPLUS_INFO(logger, "BitVector has wrong size. Resizing it...");
			subSysStates.resize(this->getNumberOfStates());
		}

		// Test if it is a proper subsystem of this Dtmc, i.e. if there is at least one state to be left out.
		if(subSysStates.getNumberOfSetBits() == subSysStates.size()) {
			LOG4CPLUS_INFO(logger, "All states are kept. This is no proper subsystem.");
			return storm::models::Dtmc<T>(*this);
		}

		// 1. Get all necessary information from the old transition matrix
		storm::storage::SparseMatrix<T> const& origMat = this->getTransitionMatrix();

		// Iterate over all rows. Count the number of all transitions from the old system to be 
		// transfered to the new one. Also build a mapping from the state number of the old system 
		// to the state number of the new system.
		uint_fast64_t subSysTransitionCount = 0;
		uint_fast64_t newRow = 0;
		std::vector<uint_fast64_t> stateMapping;
		for(uint_fast64_t row = 0; row < origMat.getRowCount(); ++row) {
			if(subSysStates.get(row)){
				for(auto const& entry : origMat.getRow(row)) {
					if(subSysStates.get(entry.getColumn())) {
						subSysTransitionCount++;	
					} 
				}
				stateMapping.push_back(newRow);
				newRow++;
			} else {
				stateMapping.push_back((uint_fast64_t) -1);
			}
		}

		// 2. Construct transition matrix

		// Take all states indicated by the vector as well as one additional state s_b as target of
		// all transitions that target a state that is not kept.
		uint_fast64_t const newStateCount = subSysStates.getNumberOfSetBits() + 1;

		// The number of transitions of the new Dtmc is the number of transitions transfered
		// from the old one plus one transition for each state to s_b.
		storm::storage::SparseMatrixBuilder<T> newMatBuilder(newStateCount, subSysTransitionCount + newStateCount);

		// Now fill the matrix.
		newRow = 0;
		T rest = 0;
		for(uint_fast64_t row = 0; row < origMat.getRowCount(); ++row) {
			if(subSysStates.get(row)){
				// Transfer transitions
				for(auto& entry : origMat.getRow(row)) {
					if(subSysStates.get(entry.getColumn())) {
						newMatBuilder.addNextValue(newRow, stateMapping[entry.getColumn()], entry.getValue());
					} else {
						rest += entry.getValue();
					}
				}

				// Insert the transition taking care of the remaining outgoing probability.
				newMatBuilder.addNextValue(newRow, newStateCount - 1, rest);
				rest = storm::utility::constantZero<T>();

				newRow++;
			}
		}

		// Insert last transition: self loop on s_b
		newMatBuilder.addNextValue(newStateCount - 1, newStateCount - 1, storm::utility::constantOne<T>());

		// 3. Take care of the labeling.
		storm::models::AtomicPropositionsLabeling newLabeling = storm::models::AtomicPropositionsLabeling(this->getStateLabeling(), subSysStates);
		newLabeling.addState();
		if(!newLabeling.containsAtomicProposition("s_b")) {
			newLabeling.addAtomicProposition("s_b");
		}
		newLabeling.addAtomicPropositionToState("s_b", newStateCount - 1);

		// 4. Handle the optionals

		boost::optional<std::vector<T>> newStateRewards;
		if(this->hasStateRewards()) {

			// Get the rewards and move the needed values to the front.
			std::vector<T> newRewards(this->getStateRewardVector());
			storm::utility::vector::selectVectorValues(newRewards, subSysStates, newRewards);

			// Throw away all values after the last state and set the reward for s_b to 0.
			newRewards.resize(newStateCount);
			newRewards[newStateCount - 1] = (T) 0;

			newStateRewards = newRewards;
		}

		boost::optional<storm::storage::SparseMatrix<T>> newTransitionRewards;
		if(this->hasTransitionRewards()) {

			storm::storage::SparseMatrixBuilder<T> newTransRewardsBuilder(newStateCount, subSysTransitionCount + newStateCount);

			// Copy the rewards for the kept states
			newRow = 0;
			for(uint_fast64_t row = 0; row < this->getTransitionRewardMatrix().getRowCount(); ++row) {
				if(subSysStates.get(row)){
					// Transfer transition rewards
					for(auto& entry : this->getTransitionRewardMatrix().getRow(row)) {
						if(subSysStates.get(entry.getColumn())) {
							newTransRewardsBuilder.addNextValue(newRow, stateMapping[entry.getColumn()], entry.getValue());
						}
					}

					// Insert the reward (e.g. 0) for the transition taking care of the remaining outgoing probability.
					newTransRewardsBuilder.addNextValue(newRow, newStateCount - 1, storm::utility::constantZero<T>());

					newRow++;
				}
			}

			newTransitionRewards = newTransRewardsBuilder.build();
		}

		boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> newChoiceLabels;
		if(this->hasChoiceLabeling()) {

			// Get the choice label sets and move the needed values to the front.
			std::vector<boost::container::flat_set<uint_fast64_t>> newChoice(this->getChoiceLabeling());
			storm::utility::vector::selectVectorValues(newChoice, subSysStates, newChoice);

			// Throw away all values after the last state and set the choice label set for s_b as empty.
			newChoice.resize(newStateCount);
			newChoice[newStateCount - 1] = boost::container::flat_set<uint_fast64_t>();

			newChoiceLabels = newChoice;
		}

		// 5. Make Dtmc from its parts and return it
		return storm::models::Dtmc<T>(newMatBuilder.build(), newLabeling, newStateRewards, std::move(newTransitionRewards), newChoiceLabels);

	}

    virtual std::shared_ptr<AbstractModel<T>> applyScheduler(storm::storage::Scheduler const& scheduler) const override {
        storm::storage::SparseMatrix<T> newTransitionMatrix = storm::utility::matrix::applyScheduler(this->getTransitionMatrix(), scheduler);
        return std::shared_ptr<AbstractModel<T>>(new Dtmc(newTransitionMatrix, this->getStateLabeling(), this->hasStateRewards() ? this->getStateRewardVector() : boost::optional<std::vector<T>>(), this->hasTransitionRewards() ? this->getTransitionRewardMatrix() :  boost::optional<storm::storage::SparseMatrix<T>>(), this->hasChoiceLabeling() ? this->getChoiceLabeling() : boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>()));
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
				return false;
			}
			if (std::abs(sum - 1) > precision) {
				LOG4CPLUS_ERROR(logger, "Row " << row << " has sum " << sum << ".");
				return false;
			}
		}
		return true;
	}
};

} // namespace models

} // namespace storm

#endif /* STORM_MODELS_DTMC_H_ */
