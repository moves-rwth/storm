/*
 * AbstractLtlFormula.h
 *
 *  Created on: 15.04.2013
 *      Author: thomas
 */

#ifndef STORM_LTL_ABSTRACTLTLFORMULA_H_
#define STORM_LTL_ABSTRACTLTLFORMULA_H_

#include <vector>
#include "src/modelchecker/ltl/ForwardDeclarations.h"
#include "src/properties/AbstractFormula.h"

namespace storm {
namespace properties {
namespace ltl {

/*!
 * This is the abstract base class for all Ltl formulas.
 *
 * @note While formula classes do have copy constructors using a copy constructor
 *       will yield a formula objects whose formula subtree consists of the same objects
 *       as the original formula. The ownership of the formula tree will be shared between
 *       the original and the copy.
 */
template <class T>
class AbstractLtlFormula : public virtual storm::properties::AbstractFormula<T> {
public:

	/*!
	 * The virtual destructor.
	 */
	virtual ~AbstractLtlFormula() {
		// Intentionally left empty
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns A deep copy of the called object.
	 */
	virtual std::shared_ptr<AbstractLtlFormula<T>> clone() const = 0;

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
	virtual std::vector<T> check(const storm::modelchecker::ltl::AbstractModelChecker<T>& modelChecker) const = 0;
};

} /* namespace ltl */
} /* namespace properties */
} /* namespace storm */
#endif /* STORM_LTL_ABSTRACTLTLFORMULA_H_ */
