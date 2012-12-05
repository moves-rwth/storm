/*
 * PCTLStateFormula.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef PCTLSTATEFORMULA_H_
#define PCTLSTATEFORMULA_H_

#include "PCTLformula.h"
#include "storage/BitVector.h"
#include "modelChecker/DtmcPrctlModelChecker.h"

namespace mrmc {

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
class PCTLStateFormula : public PCTLFormula<T> {

public:
	/*!
	 * empty destructor
	 */
	virtual ~PCTLStateFormula() { }

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @note This function is not implemented in this class.
	 * @returns a new AND-object that is identical the called object.
	 */
	virtual PCTLStateFormula<T>* clone() = 0;

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
	virtual mrmc::storage::BitVector *check(mrmc::modelChecker::DtmcPrctlModelChecker<T>& modelChecker) = 0;
};

} //namespace formula

} //namespace mrmc


#endif /* PCTLSTATEFORMULA_H_ */
