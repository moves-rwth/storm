/*
 * Abstractformula.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_AbstractFORMULA_H_
#define STORM_FORMULA_AbstractFORMULA_H_

#include <string>

namespace storm { namespace formula {
template <class T> class AbstractFormula;
}}

#include "src/modelChecker/AbstractModelChecker.h"

namespace storm {
namespace formula {


//abstract
/*!
 * @brief
 * Abstract base class for Abstract formulas in general.
 *
 * @attention This class is abstract.
 * @note Formula classes do not have copy constructors. The parameters of the constructors are usually the subtrees, so
 * 	   the syntax conflicts with copy constructors for unary operators. To produce an identical object, the classes
 * 	   AbstractPathFormula and AbstractStateFormula offer the method clone().
 */
template <class T>
class AbstractFormula {

public:
	/*!
	 * virtual destructor
	 */
	virtual ~AbstractFormula() { }
	
	/*!
	 * @note This function is not implemented in this class.
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const = 0;
	
	template <template <class Type> class MC>
	const MC<T>* cast(const storm::modelChecker::AbstractModelChecker<T>& modelChecker) const {
		try {
			const MC<T>& mc = dynamic_cast<const MC<T>&>(modelChecker);
			return &mc;
		} catch (std::bad_cast& bc) {
			std::cerr << "Bad cast: tried to cast " << typeid(modelChecker).name() << " to " << typeid(MC<T>).name() << std::endl;
			
		}
		return nullptr;
	}
};

} //namespace formula

} //namespace storm

#endif /* STORM_FORMULA_AbstractFORMULA_H_ */
