/*
 * AbstractPathFormula.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_PRCTL_ABSTRACTPATHFORMULA_H_
#define STORM_FORMULA_PRCTL_ABSTRACTPATHFORMULA_H_

namespace storm { namespace property { namespace prctl {
template <class T> class AbstractPathFormula;
}}}

#include "src/formula/abstract/AbstractFormula.h"
#include "src/modelchecker/prctl/ForwardDeclarations.h"

#include <vector>
#include <iostream>
#include <typeinfo>

namespace storm {
namespace property {
namespace prctl {

/*!
 * @brief
 * Abstract base class for Abstract path formulas.
 *
 * @attention This class is abstract.
 * @note Formula classes do not have copy constructors. The parameters of the constructors are usually the subtrees, so
 * 	   the syntax conflicts with copy constructors for unary operators. To produce an identical object, use the method
 * 	   clone().
 *
 * @note This class is intentionally not derived from AbstractPrctlFormula, as path formulas are not complete PRCTL formulas.
 */
template <class T>
class AbstractPathFormula : public virtual storm::property::abstract::AbstractFormula<T> {

public:
	/*!
	 * empty destructor
	 */
	virtual ~AbstractPathFormula() {
		// Intentionally left empty
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @note This function is not implemented in this class.
	 * @returns a new AND-object that is identical the called object.
	 */
	virtual AbstractPathFormula<T>* clone() const = 0;

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
	virtual std::vector<T> check(const storm::modelchecker::prctl::AbstractModelChecker<T>& modelChecker, bool qualitative) const = 0;
};

} //namespace prctl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_PRCTL_ABSTRACTPATHFORMULA_H_ */
