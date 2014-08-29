/*
 * Abstractformula.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_ABSTRACTFORMULA_H_
#define STORM_FORMULA_ABSTRACTFORMULA_H_

#include <string>
#include <memory>

namespace storm {
namespace property {
template <class T> class AbstractFormula;
} //namespace property
} //namespace storm

namespace storm {
namespace property {

// do properties

/*!
 * This is the abstract base class for every formula class in every logic.
 *
 * There are currently three implemented logics Ltl, Csl and Pctl.
 * The implementation of these logics can be found in the namespaces storm::property::<logic>
 * where <logic> is one of ltl, pctl and csl.
 *
 * @note While formula classes do have copy constructors using a copy constructor
 *       will yield a formula objects whose formula subtree consists of the same objects
 *       as the original formula. The ownership of the formula tree will be shared between
 *       the original and the copy.
 */
template <class T>
class AbstractFormula {

public:

	/*!
	 * The virtual destructor.
	 */
	virtual ~AbstractFormula() {
		// Intentionally left empty.
	}

	/*!
	 *	Return string representation of this formula.
	 *
	 *	@note Every subclass must implement this method.
	 *
	 *	@returns a string representation of the formula
	 */
	virtual std::string toString() const = 0;

	/*!
	 * Returns whether the formula is a propositional logic formula.
	 * That is, this formula and all its subformulas consist only of And, Or, Not and AP.
	 *
	 * @return True iff this is a propositional logic formula.
	 */
	virtual bool isPropositional() const {
		return false;
	}
};

} // namespace property
} // namespace storm

#endif /* STORM_FORMULA_ABSTRACTFORMULA_H_ */
