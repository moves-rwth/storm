/*
 * AP.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef AP_H_
#define AP_H_

#include "PCTLStateFormula.h"

namespace mrmc {

namespace formula {

/*!
 * @brief
 * Class for a PCTL formula tree with atomic proposition as root.
 *
 * This class represents the leaves in the formula tree.
 *
 * @see PCTLStateFormula
 * @see PCTLFormula
 */
template <class T>
class AP : public PCTLStateFormula<T> {

public:
	/*!
	 * Constructor
	 *
	 * Creates a new atomic proposition leaf, with the label AP
	 *
	 * @param ap The string representing the atomic proposition
	 */
	AP(std::string ap) {
		this->ap = ap;
	}

	/*!
	 * Constructor
	 *
	 * Creates a new atomic proposition leaf, with the label AP
	 *
	 * @param ap The string representing the atomic proposition
	 */
	AP(char* ap) {
	 //TODO: Does that really work?
		this->ap = ap;
	}

	/*!
	 * Destructor.
	 * At this time, empty...
	 */
	virtual ~AP() { }

	/*!
	 * @returns the name of the atomic proposition
	 */
	std::string getAP() {
		return ap;
	}

	/*!
	 * @returns a string representation of the leaf.
	 *
	 */
	virtual std::string toString() {
		return getAP();
	}

	/*!
	 * Clones the called object.
	 *
	 * @returns a new AP-object that is identical the called object.
	 */
	virtual PCTLStateFormula<T>* clone() {
	  return new AP(ap);
	}

	/*!
	 * Calls the model checker to check this formula.
	 * Needed to infer the correct type of formula class.
	 *
	 * @note This function should only be called in a generic check function of a model checker class. For other uses,
	 *       the methods of the model checker should be used.
	 *
	 * @returns A bit vector indicating all states that satisfy the formula represented by the called object.
	 */
	virtual mrmc::storage::BitVector *check(mrmc::modelChecker::DtmcPrctlModelChecker<T>* modelChecker) {
	  return modelChecker->checkAP(this);
	}

private:
	std::string ap;
};

} //namespace formula

} //namespace mrmc

#endif /* AP_H_ */
