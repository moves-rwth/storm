/*
 * DtmcPrctlModelChecker.h
 *
 *  Created on: 22.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_MODELCHECKER_ABSTRACTMODELCHECKER_H_
#define STORM_MODELCHECKER_ABSTRACTMODELCHECKER_H_

namespace storm { namespace modelChecker {
template <class Type> class AbstractModelChecker;
}}

#include "src/exceptions/InvalidPropertyException.h"
#include "src/formula/Formulas.h"
#include "src/storage/BitVector.h"
#include "src/models/AbstractModel.h"

#include <iostream>

namespace storm {
namespace modelChecker {

/*!
 * @brief
 * Interface for model checker classes.
 *
 * This class provides basic functions that are the same for all subclasses, but mainly only declares
 * abstract methods that are to be implemented in concrete instances.
 *
 * @attention This class is abstract.
 */
template<class Type>
class AbstractModelChecker :
	public virtual storm::formula::IApModelChecker<Type>,
	public virtual storm::formula::IAndModelChecker<Type>,
	public virtual storm::formula::IOrModelChecker<Type>,
	public virtual storm::formula::INotModelChecker<Type>,
	public virtual storm::formula::IUntilModelChecker<Type>,
	public virtual storm::formula::IEventuallyModelChecker<Type>,
	public virtual storm::formula::IGloballyModelChecker<Type>,
	public virtual storm::formula::INextModelChecker<Type>,
	public virtual storm::formula::IBoundedUntilModelChecker<Type>,
	public virtual storm::formula::IBoundedEventuallyModelChecker<Type>,
	public virtual storm::formula::INoBoundOperatorModelChecker<Type>,
	public virtual storm::formula::IProbabilisticBoundOperatorModelChecker<Type>,
	public virtual storm::formula::IRewardBoundOperatorModelChecker<Type>,
	public virtual storm::formula::IReachabilityRewardModelChecker<Type>,
	public virtual storm::formula::ICumulativeRewardModelChecker<Type>,
	public virtual storm::formula::IInstantaneousRewardModelChecker<Type> {
	
public:
	explicit AbstractModelChecker(storm::models::AbstractModel<Type>& model)
		: model(model) {
		// Nothing to do here...
	}
	
	explicit AbstractModelChecker(AbstractModelChecker<Type>* modelChecker)
		: model(modelChecker->model) {
	}
	
	virtual ~AbstractModelChecker() {
		//intentionally left empty
	}

	template <template <class T> class Target>
	const Target<Type>* as() const {
		try {
			const Target<Type>* target = dynamic_cast<const Target<Type>*>(this);
			return target;
		} catch (std::bad_cast& bc) {
			std::cerr << "Bad cast: tried to cast " << typeid(*this).name() << " to " << typeid(Target<Type>).name() << std::endl;
		}
		return nullptr;
	}

	/*!
	 * Checks the given state formula on the DTMC and prints the result (true/false) for all initial
	 * states.
	 * @param stateFormula The formula to be checked.
	 */
	void check(const storm::formula::AbstractStateFormula<Type>& stateFormula) const {
		std::cout << std::endl;
		LOG4CPLUS_INFO(logger, "Model checking formula\t" << stateFormula.toString());
		std::cout << "Model checking formula:\t" << stateFormula.toString() << std::endl;
		storm::storage::BitVector* result = nullptr;
		try {
			result = stateFormula.check(*this);
			LOG4CPLUS_INFO(logger, "Result for initial states:");
			std::cout << "Result for initial states:" << std::endl;
			for (auto initialState : *model.getLabeledStates("init")) {
				LOG4CPLUS_INFO(logger, "\t" << initialState << ": " << (result->get(initialState) ? "satisfied" : "not satisfied"));
				std::cout << "\t" << initialState << ": " << result->get(initialState) << std::endl;
			}
			delete result;
		} catch (std::exception& e) {
			std::cout << "Error during computation: " << e.what() << "Skipping property." << std::endl;
			if (result != nullptr) {
				delete result;
			}
		}
		std::cout << std::endl;
		storm::utility::printSeparationLine(std::cout);
	}

	/*!
	 * Checks the given operator (with no bound) on the DTMC and prints the result
	 * (probability/rewards) for all initial states.
	 * @param noBoundFormula The formula to be checked.
	 */
	void check(const storm::formula::NoBoundOperator<Type>& noBoundFormula) const {
		std::cout << std::endl;
		LOG4CPLUS_INFO(logger, "Model checking formula\t" << noBoundFormula.toString());
		std::cout << "Model checking formula:\t" << noBoundFormula.toString() << std::endl;
		std::vector<Type>* result = nullptr;
		try {
			result = noBoundFormula.check(*this);
			LOG4CPLUS_INFO(logger, "Result for initial states:");
			std::cout << "Result for initial states:" << std::endl;
			for (auto initialState : *model.getLabeledStates("init")) {
				LOG4CPLUS_INFO(logger, "\t" << initialState << ": " << (*result)[initialState]);
				std::cout << "\t" << initialState << ": " << (*result)[initialState] << std::endl;
			}
			delete result;
		} catch (std::exception& e) {
			std::cout << "Error during computation: " << e.what() << " Skipping property." << std::endl;
			if (result != nullptr) {
				delete result;
			}
		}
		std::cout << std::endl;
		storm::utility::printSeparationLine(std::cout);
	}

	/*!
	 * The check method for a formula with an AP node as root in its formula tree
	 *
	 * @param formula The Ap state formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	storm::storage::BitVector* checkAp(const storm::formula::Ap<Type>& formula) const {
		if (formula.getAp().compare("true") == 0) {
			return new storm::storage::BitVector(model.getNumberOfStates(), true);
		} else if (formula.getAp().compare("false") == 0) {
			return new storm::storage::BitVector(model.getNumberOfStates());
		}

		if (!model.hasAtomicProposition(formula.getAp())) {
			LOG4CPLUS_ERROR(logger, "Atomic proposition '" << formula.getAp() << "' is invalid.");
			throw storm::exceptions::InvalidPropertyException() << "Atomic proposition '" << formula.getAp() << "' is invalid.";
		}

		return new storm::storage::BitVector(*model.getLabeledStates(formula.getAp()));
	}

	/*!
	 * The check method for a state formula with an And node as root in its formula tree
	 *
	 * @param formula The And formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	storm::storage::BitVector* checkAnd(const storm::formula::And<Type>& formula) const {
		storm::storage::BitVector* result = formula.getLeft().check(*this);
		storm::storage::BitVector* right = formula.getRight().check(*this);
		(*result) &= (*right);
		delete right;
		return result;
	}

	/*!
	 * The check method for a formula with a Not node as root in its formula tree
	 *
	 * @param formula The Not state formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	storm::storage::BitVector* checkNot(const storm::formula::Not<Type>& formula) const {
		storm::storage::BitVector* result = formula.getChild().check(*this);
		result->complement();
		return result;
	}

	/*!
	 * The check method for a state formula with an Or node as root in its formula tree
	 *
	 * @param formula The Or state formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	virtual storm::storage::BitVector* checkOr(const storm::formula::Or<Type>& formula) const {
		storm::storage::BitVector* result = formula.getLeft().check(*this);
		storm::storage::BitVector* right = formula.getRight().check(*this);
		(*result) |= (*right);
		delete right;
		return result;
	}

	/*!
	 * The check method for a state formula with a bound operator node as root in
	 * its formula tree
	 *
	 * @param formula The state formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	storm::storage::BitVector* checkProbabilisticBoundOperator(const storm::formula::ProbabilisticBoundOperator<Type>& formula) const {
		// First, we need to compute the probability for satisfying the path formula for each state.
		std::vector<Type>* quantitativeResult = formula.getPathFormula().check(*this, false);

		// Create resulting bit vector, which will hold the yes/no-answer for every state.
		storm::storage::BitVector* result = new storm::storage::BitVector(quantitativeResult->size());

		// Now, we can compute which states meet the bound specified in this operator and set the
		// corresponding bits to true in the resulting vector.
		for (uint_fast64_t i = 0; i < quantitativeResult->size(); ++i) {
			if (formula.meetsBound((*quantitativeResult)[i])) {
				result->set(i, true);
			}
		}

		// Delete the probabilities computed for the states and return result.
		delete quantitativeResult;
		return result;
	}

	/*!
	 * The check method for a state formula with a bound operator node as root in
	 * its formula tree
	 *
	 * @param formula The state formula to check
	 * @returns The set of states satisfying the formula, represented by a bit vector
	 */
	storm::storage::BitVector* checkRewardBoundOperator(const storm::formula::RewardBoundOperator<Type>& formula) const {
		// First, we need to compute the probability for satisfying the path formula for each state.
		std::vector<Type>* quantitativeResult = formula.getPathFormula().check(*this, false);

		// Create resulting bit vector, which will hold the yes/no-answer for every state.
		storm::storage::BitVector* result = new storm::storage::BitVector(quantitativeResult->size());

		// Now, we can compute which states meet the bound specified in this operator and set the
		// corresponding bits to true in the resulting vector.
		for (uint_fast64_t i = 0; i < quantitativeResult->size(); ++i) {
			if (formula.meetsBound((*quantitativeResult)[i])) {
				result->set(i, true);
			}
		}

		// Delete the probabilities computed for the states and return result.
		delete quantitativeResult;
		return result;
	}
	
	void setModel(storm::models::AbstractModel<Type>& model) {
		this->model = model;
	}
	
	template <class Model>
	Model& getModel() const {
		return *dynamic_cast<Model*>(&this->model);
	}

private:
	storm::models::AbstractModel<Type>& model;

};

} //namespace modelChecker

} //namespace storm

#endif /* STORM_MODELCHECKER_DTMCPRCTLMODELCHECKER_H_ */
