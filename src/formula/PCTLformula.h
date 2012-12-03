/*
 * PCTLformula.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef PCTLFORMULA_H_
#define PCTLFORMULA_H_

#include <string>

namespace mrmc {

namespace formula {


//abstract
/*!
 * @brief
 * Abstract base class for PCTL formulas in general.
 *
 * @attention This class is abstract.
 * @note Formula classes do not have copy constructors. The parameters of the constructors are usually the subtrees, so
 * 	   the syntax conflicts with copy constructors for unary operators. To produce an identical object, the classes
 * 	   PCTLPathFormula and PCTLStateFormula offer the method clone().
 */
template <class T>
class PCTLFormula {
   public:
		/*!
		 * virtual destructor
		 */
	   virtual ~PCTLFormula() { }

	   /*!
	    * @note This function is not implemented in this class.
	    * @returns a string representation of the formula
	    */
      virtual std::string toString() = 0;
};

} //namespace formula

} //namespace mrmc

#endif /* PCTLFORMULA_H_ */
