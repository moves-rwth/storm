/*
 * AbstractLtlFormula.h
 *
 *  Created on: 15.04.2013
 *      Author: thomas
 */

#ifndef ABSTRACTLTLFORMULA_H_
#define ABSTRACTLTLFORMULA_H_

namespace storm {
namespace formula {

class AbstractLtlFormula {
public:
	/**
	 * Empty destructor
	 */
	virtual ~AbstractLtlFormula() {
		// Intentionally left empty
	}

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
	virtual std::vector<T>* check(const storm::modelchecker::AbstractModelChecker<T>& modelChecker, bool qualitative) const = 0;
};

} /* namespace formula */
} /* namespace storm */
#endif /* ABSTRACTLTLFORMULA_H_ */
