/*
 * PctlStateFormula.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_PCTLSTATEFORMULA_H_
#define STORM_FORMULA_PCTLSTATEFORMULA_H_

#include "modelChecker/ForwardDeclarations.h"
#include "PctlFormula.h"
#include "storage/BitVector.h"

namespace storm {

namespace formula {

/*!
 * @brief
 * Abstract base class for PCTL state formulas.
 *
 * @attention This class is abstract.
 * @note Formula classes do not have copy constructors. The parameters of the constructors are usually the subtrees, so
 * 	   the syntax conflicts with copy constructors for unary operators. To produce an identical object, use the method
 * 	   clone().
 */
template <class T>
class PctlStateFormula : public PctlFormula<T> {

public:
	/*!
	 * empty destructor
	 */
	virtual ~PctlStateFormula() { }

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @note This function is not implemented in this class.
	 * @returns a new AND-object that is identical the called object.
	 */
	virtual PctlStateFormula<T>* clone() const = 0;

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
	virtual storm::storage::BitVector *check(const storm::modelChecker::DtmcPrctlModelChecker<T>& modelChecker) const = 0;
};

} //namespace formula

} //namespace storm

#endif /* STORM_FORMULA_PCTLSTATEFORMULA_H_ */
