/*
 * Ctmdp.h
 *
 *  Created on: 14.01.2013
 *      Author: Philipp Berger
 */

#ifndef STORM_MODELS_CTMDP_H_
#define STORM_MODELS_CTMDP_H_

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
#include "src/models/AbstractNonDeterministicModel.h"
#include "src/parser/NonDeterministicSparseTransitionParser.h"

namespace storm {

namespace models {

/*!
 * This class represents a Markov Decision Process (CTMDP) whose states are
 * labeled with atomic propositions.
 */
template <class T>
class Ctmdp : public storm::models::AbstractNonDeterministicModel<T> {

public:
	//! Constructor
	/*!
	 * Constructs a CTMDP object from the given transition probability matrix and
	 * the given labeling of the states.
	 * @param probabilityMatrix The transition probability relation of the
	 * CTMDP given by a matrix.
	 * @param stateLabeling The labeling that assigns a set of atomic
	 * propositions to each state.
	 */
	Ctmdp(std::shared_ptr<storm::storage::SparseMatrix<T>> probabilityMatrix,
			std::shared_ptr<storm::models::AtomicPropositionsLabeling> stateLabeling,
			std::shared_ptr<std::vector<uint_fast64_t>> choiceIndices,
			std::shared_ptr<std::vector<T>> stateRewardVector = nullptr,
			std::shared_ptr<storm::storage::SparseMatrix<T>> transitionRewardMatrix = nullptr)
			: AbstractNonDeterministicModel<T>(probabilityMatrix, stateLabeling, choiceIndices, stateRewardVector, transitionRewardMatrix) {
		if (!this->checkValidityOfProbabilityMatrix()) {
			LOG4CPLUS_ERROR(logger, "Probability matrix is invalid.");
			throw storm::exceptions::InvalidArgumentException() << "Probability matrix is invalid.";
		}
	}

	//! Copy Constructor
	/*!
	 * Copy Constructor. Performs a deep copy of the given CTMDP.
	 * @param ctmdp A reference to the CTMDP that is to be copied.
	 */
	Ctmdp(const Ctmdp<T> &ctmdp) : AbstractNonDeterministicModel<T>(ctmdp) {
		if (!this->checkValidityOfProbabilityMatrix()) {
			LOG4CPLUS_ERROR(logger, "Probability matrix is invalid.");
			throw storm::exceptions::InvalidArgumentException() << "Probability matrix is invalid.";
		}
	}

	//! Destructor
	/*!
	 * Destructor.
	 */
	~Ctmdp() {
		// Intentionally left empty.
	}
	
	/*!
	 * Prints information about the model to the specified stream.
	 * @param out The stream the information is to be printed to.
	 */
	void printModelInformationToStream(std::ostream& out) const {
		storm::utility::printSeparationLine(out);
		out << std::endl;
		out << "Model type: \t\tCTMDP" << std::endl;
		out << "States: \t\t" << this->getNumberOfStates() << std::endl;
		out << "Transitions: \t\t" << this->getNumberOfTransitions() << std::endl;
		this->getStateLabeling()->printAtomicPropositionsInformationToStream(out);
		out << "Size in memory: \t"
			<< (this->getTransitionMatrix()->getSizeInMemory() +
				this->getStateLabeling()->getSizeInMemory() +
				sizeof(*this))/1024 << " kbytes" << std::endl;
		out << std::endl;
		storm::utility::printSeparationLine(out);
	}
	
	storm::models::ModelType getType() {
		return CTMDP;
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

#endif /* STORM_MODELS_CTMDP_H_ */
