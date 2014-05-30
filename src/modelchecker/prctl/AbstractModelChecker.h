/*
 * AbstractModelChecker.h
 *
 *  Created on: 22.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_MODELCHECKER_ABSTRACTMODELCHECKER_H_
#define STORM_MODELCHECKER_ABSTRACTMODELCHECKER_H_

// Forward declaration of abstract model checker class needed by the formula classes.
namespace storm {
namespace modelchecker {
namespace prctl {
	template <class Type> class AbstractModelChecker;
}
}
}

#include <stack>
#include "src/exceptions/InvalidPropertyException.h"
#include "src/formula/Prctl.h"
#include "src/storage/BitVector.h"
#include "src/models/AbstractModel.h"
#include "src/settings/Settings.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

#include <iostream>

extern log4cplus::Logger logger;

namespace storm {
namespace modelchecker {
namespace prctl {

/*!
 * @brief
 * (Abstract) interface for all model checker classes.
 *
 * This class provides basic functions that are common to all model checkers (i.e. subclasses). It mainly declares
 * abstract methods that are implemented in the concrete subclasses, but also covers checking procedures that are common
 * to all model checkers for state-based models.
 */
template<class Type>
class AbstractModelChecker :
	// A list of interfaces the model checker supports. Typically, for each of the interfaces, a check method needs to
	// be implemented that performs the corresponding check.
	public virtual storm::property::prctl::IApModelChecker<Type>,
	public virtual storm::property::prctl::IAndModelChecker<Type>,
	public virtual storm::property::prctl::IOrModelChecker<Type>,
	public virtual storm::property::prctl::INotModelChecker<Type>,
	public virtual storm::property::prctl::IUntilModelChecker<Type>,
	public virtual storm::property::prctl::IEventuallyModelChecker<Type>,
	public virtual storm::property::prctl::IGloballyModelChecker<Type>,
	public virtual storm::property::prctl::INextModelChecker<Type>,
	public virtual storm::property::prctl::IBoundedUntilModelChecker<Type>,
	public virtual storm::property::prctl::IBoundedEventuallyModelChecker<Type>,
	public virtual storm::property::prctl::IProbabilisticBoundOperatorModelChecker<Type>,
	public virtual storm::property::prctl::IRewardBoundOperatorModelChecker<Type>,
	public virtual storm::property::prctl::IReachabilityRewardModelChecker<Type>,
	public virtual storm::property::prctl::ICumulativeRewardModelChecker<Type>,
	public virtual storm::property::prctl::IInstantaneousRewardModelChecker<Type> {
	
public:
	/*!
	 * Constructs an AbstractModelChecker with the given model.
	 */
	explicit AbstractModelChecker(storm::models::AbstractModel<Type> const& model) : minimumOperatorStack(), model(model) {
		// Intentionally left empty.
	}
	/*!
	 * Copy constructs an AbstractModelChecker from the given model checker. In particular, this means that the newly
	 * constructed model checker will have the model of the given model checker as its associated model.
	 */
	explicit AbstractModelChecker(AbstractModelChecker<Type> const& modelchecker) : minimumOperatorStack(), model(modelchecker.model) {
		// Intentionally left empty.
	}
	
	/*!
	 * Virtual destructor. Needs to be virtual, because this class has virtual methods.
	 */
	virtual ~AbstractModelChecker() {
		// Intentionally left empty.
	}

	/*!
	 * Returns a pointer to the model checker object that is of the requested type as given by the template parameters.
     *
	 * @return A pointer to the model checker object that is of the requested type as given by the template parameters.
	 * If the model checker is not of the requested type, type casting will fail and result in an exception.
	 */
	template <template <class T> class Target>
	const Target<Type>* as() const {
		try {
			const Target<Type>* target = dynamic_cast<const Target<Type>*>(this);
			return target;
		} catch (std::bad_cast& bc) {
			LOG4CPLUS_ERROR(logger, "Bad cast: tried to cast " << typeid(*this).name() << " to " << typeid(Target<Type>).name() << ".");
			throw bc;
		}
		return nullptr;
	}

	/*!
	 * Retrieves the model associated with this model checker as a constant reference to an object of the type given
	 * by the template parameter.
	 *
	 * @return A constant reference of the specified type to the model associated with this model checker. If the model
	 * is not of the requested type, type casting will fail and result in an exception.
	 */
	template <class Model>
	Model const& getModel() const {
		try {
			Model const& target = dynamic_cast<Model const&>(this->model);
			return target;
		} catch (std::bad_cast& bc) {
			LOG4CPLUS_ERROR(logger, "Bad cast: tried to cast " << typeid(this->model).name() << " to " << typeid(Model).name() << ".");
			throw bc;
		}
	}

	/*!
	 * Checks the given formula consisting of a single atomic proposition.
	 *
	 * @param formula The formula to be checked.
	 * @return The set of states satisfying the formula represented by a bit vector.
	 */
	storm::storage::BitVector checkAp(storm::property::prctl::Ap<Type> const& formula) const {
		if (formula.getAp() == "true") {
			return storm::storage::BitVector(model.getNumberOfStates(), true);
		} else if (formula.getAp() == "false") {
			return storm::storage::BitVector(model.getNumberOfStates());
		}

		if (!model.hasAtomicProposition(formula.getAp())) {
			LOG4CPLUS_ERROR(logger, "Atomic proposition '" << formula.getAp() << "' is invalid.");
			throw storm::exceptions::InvalidPropertyException() << "Atomic proposition '" << formula.getAp() << "' is invalid.";
		}

		return storm::storage::BitVector(model.getLabeledStates(formula.getAp()));
	}

	/*!
	 * Checks the given formula that is a logical "and" of two formulae.
	 *
	 * @param formula The formula to be checked.
	 * @return The set of states satisfying the formula represented by a bit vector.
	 */
	storm::storage::BitVector checkAnd(storm::property::prctl::And<Type> const& formula) const {
		storm::storage::BitVector result = formula.getLeft().check(*this);
		storm::storage::BitVector right = formula.getRight().check(*this);
		result &= right;
		return result;
	}

	/*!
	 * Checks the given formula that is a logical "or" of two formulae.
	 *
	 * @param formula The formula to check.
	 * @return The set of states satisfying the formula represented by a bit vector.
	 */
	storm::storage::BitVector checkOr(storm::property::prctl::Or<Type> const& formula) const {
		storm::storage::BitVector result = formula.getLeft().check(*this);
		storm::storage::BitVector right = formula.getRight().check(*this);
		result |= right;
		return result;
	}

	/*!
	 * Checks the given formula that is a logical "not" of a sub-formula.
	 *
	 * @param formula The formula to check.
	 * @return The set of states satisfying the formula represented by a bit vector.
	 */
	storm::storage::BitVector checkNot(const storm::property::prctl::Not<Type>& formula) const {
		storm::storage::BitVector result = formula.getChild().check(*this);
		result.complement();
		return result;
	}


	/*!
	 * Checks the given formula that is a P operator over a path formula featuring a value bound.
	 *
	 * @param formula The formula to check.
	 * @return The set of states satisfying the formula represented by a bit vector.
	 */
	virtual storm::storage::BitVector checkProbabilisticBoundOperator(storm::property::prctl::ProbabilisticBoundOperator<Type> const& formula) const {
		// First, we need to compute the probability for satisfying the path formula for each state.
		std::vector<Type> quantitativeResult = formula.getPathFormula().check(*this, false);

		// Create resulting bit vector that will hold the yes/no-answer for every state.
		storm::storage::BitVector result(quantitativeResult.size());

		// Now, we can compute which states meet the bound specified in this operator and set the
		// corresponding bits to true in the resulting vector.
		for (uint_fast64_t i = 0; i < quantitativeResult.size(); ++i) {
			if (formula.meetsBound(quantitativeResult[i])) {
				result.set(i, true);
			}
		}

		return result;
	}

	/*!
	 * Checks the given formula that is an R operator over a reward formula featuring a value bound.
	 *
	 * @param formula The formula to check.
	 * @return The set of states satisfying the formula represented by a bit vector.
	 */
	virtual storm::storage::BitVector checkRewardBoundOperator(const storm::property::prctl::RewardBoundOperator<Type>& formula) const {
		// First, we need to compute the probability for satisfying the path formula for each state.
		std::vector<Type> quantitativeResult = formula.getPathFormula().check(*this, false);

		// Create resulting bit vector that will hold the yes/no-answer for every state.
		storm::storage::BitVector result(quantitativeResult.size());

		// Now, we can compute which states meet the bound specified in this operator and set the
		// corresponding bits to true in the resulting vector.
		for (uint_fast64_t i = 0; i < quantitativeResult.size(); ++i) {
			if (formula.meetsBound(quantitativeResult[i])) {
				result.set(i, true);
			}
		}

		return result;
	}

	/*!
	 * Checks the given formula and determines whether minimum or maximum probabilities are to be computed for the formula.
	 *
	 * @param formula The formula to check.
	 * @param optOperator True iff minimum probabilities are to be computed.
	 * @returns The probabilities to satisfy the formula, represented by a vector.
	 */
	virtual std::vector<Type> checkOptimizingOperator(storm::property::prctl::AbstractPathFormula<Type> const & formula, bool optOperator) const {
		minimumOperatorStack.push(optOperator);
		std::vector<Type> result = formula.check(*this, false);
		minimumOperatorStack.pop();
		return result;
	}

	/*!
	 * Checks the given formula and determines whether minimum or maximum rewards are to be computed for the formula.
	 *
	 * @param formula The formula to check.
	 * @param optOperator True iff minimum rewards are to be computed.
	 * @returns The the rewards accumulated by the formula, represented by a vector.
	 */
	virtual std::vector<Type> checkOptimizingOperator(storm::property::prctl::AbstractRewardPathFormula<Type> const & formula, bool optOperator) const {
		minimumOperatorStack.push(optOperator);
		std::vector<Type> result = formula.check(*this, false);
		minimumOperatorStack.pop();
		return result;
	}

	/*!
	 * Checks the given formula and determines whether minimum or maximum probabilities or rewards are to be computed for the formula.
	 *
	 * @param formula The formula to check.
	 * @param optOperator True iff minimum probabilities/rewards are to be computed.
	 * @returns The set of states satisfying the formula represented by a bit vector.
	 */
	virtual storm::storage::BitVector checkOptimizingOperator(storm::property::prctl::AbstractStateFormula<Type> const & formula, bool optOperator) const {
		minimumOperatorStack.push(optOperator);
		storm::storage::BitVector result = formula.check(*this);
		minimumOperatorStack.pop();
		return result;
	}

protected:

	/*!
	 * A stack used for storing whether we are currently computing min or max probabilities or rewards, respectively.
	 * The topmost element is true if and only if we are currently computing minimum probabilities or rewards.
	 */
	mutable std::stack<bool> minimumOperatorStack;

private:

	/*!
	 * A constant reference to the model associated with this model checker.
	 *
	 * @note that we do not own this object, but merely have a constant reference to  it. That means that using the
	 * model checker object is unsafe after the object has been destroyed.
	 */
	storm::models::AbstractModel<Type> const& model;
};

} // namespace prctl
} // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_PRCTL_DTMCPRCTLMODELCHECKER_H_ */
