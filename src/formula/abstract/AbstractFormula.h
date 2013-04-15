/*
 * Abstractformula.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_ABSTRACT_ABSTRACTFORMULA_H_
#define STORM_FORMULA_ABSTRACT_ABSTRACTFORMULA_H_

#include <string>

namespace storm {
namespace formula {
namespace abstract {
template <class T> class AbstractFormula;
}
}
}

#include "src/modelchecker/ForwardDeclarations.h"
#include "src/formula/AbstractFormulaChecker.h"

namespace storm {
namespace formula {
namespace abstract {

//abstract
/*!
 *	@brief Abstract base class for Abstract formulas in general.
 *
 *	@attention This class is abstract.
 *	@note Formula classes do not have copy constructors. The parameters of the constructors are usually the subtrees, so
 * 	   the syntax conflicts with copy constructors for unary operators. To produce an identical object, the classes
 * 	   AbstractPathFormula and AbstractStateFormula offer the method clone().
 *
 *	This is the base class for every formula class in every logic.
 */
template <class T>
class AbstractFormula {

public:
	/*!
	 * Virtual destructor.
	 */
	virtual ~AbstractFormula() { }
	
	/*!
	 *	@brief Return string representation of this formula.
	 *
	 *	@note very subclass must implement this method.
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
	 *	subclass is supposed to call checker.conforms() for all child
	 *	formulas and return true if and only if all those calls returned
	 *	true.
	 *
	 *	@param checker Checker object.
	 *	@return true iff all subtrees are valid.
	 */
	virtual bool conforms(const AbstractFormulaChecker<T>& checker) const = 0;
};

} // namespace abstract
} // namespace formula
} // namespace storm

#endif /* STORM_FORMULA_ABSTRACT_ABSTRACTFORMULA_H_ */
