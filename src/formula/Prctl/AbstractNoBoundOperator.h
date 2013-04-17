/*
 * AbstractNoBoundOperator.h
 *
 *  Created on: 16.04.2013
 *      Author: thomas
 */

#ifndef ABSTRACTNOBOUNDOPERATOR_H_
#define ABSTRACTNOBOUNDOPERATOR_H_

#include "AbstractPrctlFormula.h"
#include "src/formula/abstract/IOptimizingOperator.h"

namespace storm {
namespace formula {
namespace prctl {

template <class T>
class AbstractNoBoundOperator;

/*!
 *  @brief Interface class for model checkers that support PathNoBoundOperator.
 *
 *  All model checkers that support the formula class NoBoundOperator must inherit
 *  this pure virtual class.
 */
template <class T>
class INoBoundOperatorModelChecker {
public:
	/*!
     *  @brief Evaluates NoBoundOperator formula within a model checker.
     *
     *  @param obj Formula object with subformulas.
     *  @return Result of the formula for every node.
     */
    virtual std::vector<T>* checkNoBoundOperator(const AbstractNoBoundOperator<T>& obj) const = 0;


};

template <class T>
class AbstractNoBoundOperator: public AbstractPrctlFormula<T>,
										 public virtual storm::formula::abstract::IOptimizingOperator {
public:
	AbstractNoBoundOperator() {
		// TODO Auto-generated constructor stub

	}
	virtual ~AbstractNoBoundOperator() {
		// TODO Auto-generated destructor stub
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @note This function is not implemented in this class.
	 * @returns a new AND-object that is identical the called object.
	 */
	virtual AbstractNoBoundOperator<T>* clone() const = 0;

	/*!
	 * Calls the model checker to check this formula.
	 * Needed to infer the correct type of formula class.
	 *
	 * @note This function should only be called in a generic check function of a model checker class. For other uses,
	 *       the methods of the model checker should be used.
	 *
	 * @note This function is not implemented in this class.
	 *
	 * @returns A vector indicating the probability that the formula holds for each state.
	 */
	virtual std::vector<T>* check(const storm::modelchecker::AbstractModelChecker<T>& modelChecker, bool qualitative=false) const = 0;
};

} /* namespace prctl */
} /* namespace formula */
} /* namespace storm */
#endif /* ABSTRACTNOBOUNDOPERATOR_H_ */
