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
#include "GraphTransitions.h"
#include "src/storage/SparseMatrix.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/utility/CommandLine.h"
#include "src/utility/Settings.h"
#include "src/models/AbstractModel.h"
#include "src/parser/NonDeterministicSparseTransitionParser.h"

namespace storm {

namespace models {

/*!
 * This class represents a Markov Decision Process (MDP) whose states are
 * labeled with atomic propositions.
 */
template <class T>
class Mdp : public storm::models::AbstractModel {

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
			std::shared_ptr<std::vector<uint_fast64_t>> rowMapping,
			std::shared_ptr<std::vector<T>> stateRewards = nullptr,
			std::shared_ptr<storm::storage::SparseMatrix<T>> transitionRewardMatrix = nullptr)
			: probabilityMatrix(probabilityMatrix), stateLabeling(stateLabeling), rowMapping(rowMapping),
			  stateRewards(stateRewards), transitionRewardMatrix(transitionRewardMatrix),
			  backwardTransitions(nullptr) {
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
	Mdp(const Mdp<T> &mdp) : probabilityMatrix(mdp.probabilityMatrix),
			stateLabeling(mdp.stateLabeling), rowMapping(mdp.rowMapping), stateRewards(mdp.stateRewards),
			transitionRewardMatrix(mdp.transitionRewardMatrix) {
		if (mdp.backwardTransitions != nullptr) {
			this->backwardTransitions = new storm::models::GraphTransitions<T>(*mdp.backwardTransitions);
		}
		if (!this->checkValidityOfProbabilityMatrix()) {
			LOG4CPLUS_ERROR(logger, "Probability matrix is invalid.");
			throw storm::exceptions::InvalidArgumentException() << "Probability matrix is invalid.";
		}
	}

	//! Destructor
	/*!
	 * Destructor. Frees the matrix and labeling associated with this MDP.
	 */
	~Mdp() {
		if (this->backwardTransitions != nullptr) {
			delete this->backwardTransitions;
		}
	}
	
	/*!
	 * Returns the state space size of the MDP.
	 * @return The size of the state space of the MDP.
	 */
	uint_fast64_t getNumberOfStates() const {
		return this->probabilityMatrix->getColumnCount();
	}

	/*!
	 * Returns the number of (non-zero) transitions of the MDP.
	 * @return The number of (non-zero) transitions of the MDP.
	 */
	uint_fast64_t getNumberOfTransitions() const {
		return this->probabilityMatrix->getNonZeroEntryCount();
	}

	/*!
	 * Returns a bit vector in which exactly those bits are set to true that
	 * correspond to a state labeled with the given atomic proposition.
	 * @param ap The atomic proposition for which to get the bit vector.
	 * @return A bit vector in which exactly those bits are set to true that
	 * correspond to a state labeled with the given atomic proposition.
	 */
	storm::storage::BitVector* getLabeledStates(std::string ap) const {
		return this->stateLabeling->getAtomicProposition(ap);
	}

	/*!
	 * Returns a pointer to the matrix representing the transition probability
	 * function.
	 * @return A pointer to the matrix representing the transition probability
	 * function.
	 */
	std::shared_ptr<storm::storage::SparseMatrix<T>> getTransitionProbabilityMatrix() const {
		return this->probabilityMatrix;
	}

	/*!
	 * Returns a pointer to the matrix representing the transition rewards.
	 * @return A pointer to the matrix representing the transition rewards.
	 */
	std::shared_ptr<storm::storage::SparseMatrix<T>> getTransitionRewardMatrix() const {
		return this->transitionRewardMatrix;
	}

	/*!
	 * Returns a pointer to the vector representing the state rewards.
	 * @return A pointer to the vector representing the state rewards.
	 */
	std::shared_ptr<std::vector<T>> getStateRewards() const {
		return this->stateRewards;
	}

	/*!
	 *
	 */
	std::set<std::string> const getPropositionsForState(uint_fast64_t const &state) const {
		return stateLabeling->getPropositionsForState(state);
	}

	/*!
	 * Retrieves a reference to the backwards transition relation.
	 * @return A reference to the backwards transition relation.
	 */
	storm::models::GraphTransitions<T>& getBackwardTransitions() {
		if (this->backwardTransitions == nullptr) {
			this->backwardTransitions = new storm::models::GraphTransitions<T>(this->probabilityMatrix, false);
		}
		return *this->backwardTransitions;
	}

	/*!
	 * Retrieves whether this MDP has a state reward model.
	 * @return True if this MDP has a state reward model.
	 */
	bool hasStateRewards() {
		return this->stateRewards != nullptr;
	}

	/*!
	 * Retrieves whether this MDP has a transition reward model.
	 * @return True if this MDP has a transition reward model.
	 */
	bool hasTransitionRewards() {
		return this->transitionRewardMatrix != nullptr;
	}

	/*!
	 * Retrieves whether the given atomic proposition is a valid atomic proposition in this model.
	 * @param atomicProposition The atomic proposition to be checked for validity.
	 * @return True if the given atomic proposition is valid in this model.
	 */
	bool hasAtomicProposition(std::string const& atomicProposition) {
		return this->stateLabeling->containsAtomicProposition(atomicProposition);
	}

	/*!
	 * Prints information about the model to the specified stream.
	 * @param out The stream the information is to be printed to.
	 */
	void printModelInformationToStream(std::ostream& out) const {
		storm::utility::printSeparationLine(out);
		out << std::endl;
		out << "Model type: \t\tMDP" << std::endl;
		out << "States: \t\t" << this->getNumberOfStates() << std::endl;
		out << "Transitions: \t\t" << this->getNumberOfTransitions() << std::endl;
		this->stateLabeling->printAtomicPropositionsInformationToStream(out);
		out << "Size in memory: \t"
			<< (this->probabilityMatrix->getSizeInMemory() +
				this->stateLabeling->getSizeInMemory() +
				sizeof(*this))/1024 << " kbytes" << std::endl;
		out << std::endl;
		storm::utility::printSeparationLine(out);
	}
	
	storm::models::ModelType getType() {
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
		for (uint_fast64_t row = 0; row < this->probabilityMatrix->getRowCount(); row++) {
			T sum = this->probabilityMatrix->getRowSum(row);
			if (sum == 0) continue;
			if (std::abs(sum - 1) > precision) return false;
		}
		return true;
	}

	/*! A matrix representing the transition probability function of the MDP. */
	std::shared_ptr<storm::storage::SparseMatrix<T>> probabilityMatrix;

	/*! The labeling of the states of the MDP. */
	std::shared_ptr<storm::models::AtomicPropositionsLabeling> stateLabeling;
	
	/*! The mapping from states to rows. */
	std::shared_ptr<std::vector<uint_fast64_t>> rowMapping;

	/*! The state-based rewards of the MDP. */
	std::shared_ptr<std::vector<T>> stateRewards;

	/*! The transition-based rewards of the MDP. */
	std::shared_ptr<storm::storage::SparseMatrix<T>> transitionRewardMatrix;

	/*!
	 * A data structure that stores the predecessors for all states. This is
	 * needed for backwards directed searches.
	 */
	storm::models::GraphTransitions<T>* backwardTransitions;
};

} // namespace models

} // namespace storm

#endif /* STORM_MODELS_MDP_H_ */
