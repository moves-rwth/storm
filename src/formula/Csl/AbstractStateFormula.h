/*
 * AbstractStateFormula.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_CSL_ABSTRACTSTATEFORMULA_H_
#define STORM_FORMULA_CSL_ABSTRACTSTATEFORMULA_H_

namespace storm {
namespace property {
namespace csl {
template<class T> class AbstractStateFormula;
}
}
}

#include "AbstractCslFormula.h"
#include "src/storage/BitVector.h"
#include "src/modelchecker/csl/ForwardDeclarations.h"

namespace storm {
namespace property {
namespace csl {

/*!
 * @brief
 * Abstract base class for Abstract state formulas.
 *
 * @attention This class is abstract.
 * @note Formula classes do not have copy constructors. The parameters of the constructors are usually the subtrees, so
 * 	   the syntax conflicts with copy constructors for unary operators. To produce an identical object, use the method
 * 	   clone().
 */
template <class T>
class AbstractStateFormula : public AbstractCslFormula<T> {

public:
	/*!
	 * empty destructor
	 */
	virtual ~AbstractStateFormula() {
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
	virtual AbstractStateFormula<T>* clone() const = 0;

	/*!
	 * Calls the model checker to check this formula.
	 * Needed to infer the correct type of formula class.
	 *
	 * @note This function should only be called in a generic check function of a model checker class. For other uses,
	 *       the methods of the model checker should be used.
	 *
	 * @note This function is not implemented in this class.
	 *
	 * @returns A bit vector indicating all states that satisfy the formula represented by the called object.
	 */
	virtual storm::storage::BitVector* check(const storm::modelchecker::csl::AbstractModelChecker<T>& modelChecker) const = 0;
};

} //namespace csl
} //namespace property
} //namespace storm


#endif /* STORM_FORMULA_CSL_AbstractSTATEFORMULA_H_ */
