/*
 * Abstractformula.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_ABSTRACTFORMULA_H_
#define STORM_FORMULA_ABSTRACTFORMULA_H_

#include <string>

namespace storm {
namespace property {
template <class T> class AbstractFormula;
} //namespace property
} //namespace storm

#include "src/formula/AbstractFormulaChecker.h"

namespace storm {
namespace property {

//abstract
/*!
 *	@brief Abstract base class for logic Abstract formulas in general.
 *
 * The namespace storm::property::abstract contains versions of the formula classes which are logic abstract, and contain
 * the implementation which is not directly dependent on the logics.
 * The classes for the subtrees are referenced by the template parameter FormulaType, which is typically instantiated in
 * the derived classes of concrete logics.
 *
 * The instantiation of FormulaType should be a subclass of AbstractFormula, as the functions "toString" and "validate"
 * of the subformulas are needed.
 *
 * @note
 * 		Even though the namespace is called "abstract", its classes may be completely implemented; abstract here denotes
 * 		the abstraction from a concrete logic.
 *
 *	@attention This class is abstract.
 *	@note Formula classes do not have copy constructors. The parameters of the constructors are usually the subtrees, so
 * 	   the syntax conflicts with copy constructors for unary operators. To produce an identical object, the classes
 * 	   AbstractFormula and AbstractFormula offer the method clone().
 *
 *	This is the base class for every formula class in every logic.
 */
template <class T>
class AbstractFormula {

public:
	/*!
	 * Virtual destructor.
	 */
	virtual ~AbstractFormula() {
		// Intentionally left empty.
	}

	/*!
	 *	@brief Return string representation of this formula.
	 *
	 *	@note every subclass must implement this method.
	 *
	 *	@returns a string representation of the formula
	 */
	virtual std::string toString() const = 0;

	/*!
	 *	@brief Checks if all subtrees are valid in some logic.
	 *
	 *	@note Every subclass must implement this method.
	 *
	 *	This method is given a checker object that knows which formula
	 *	classes are allowed within the logic the checker represents. Every
	 *	subclass is supposed to call checker.validate() for all child
	 *	formulas and return true if and only if all those calls returned
	 *	true.
	 *
	 *	@param checker Checker object.
	 *	@return true iff all subtrees are valid.
	 */
	virtual bool validate(const AbstractFormulaChecker<T>& checker) const = 0;
};

} // namespace property
} // namespace storm

#endif /* STORM_FORMULA_ABSTRACTFORMULA_H_ */
