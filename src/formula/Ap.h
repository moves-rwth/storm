/*
 * Ap.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef MRMC_FORMULA_AP_H_
#define MRMC_FORMULA_AP_H_

#include "PctlStateFormula.h"

namespace mrmc {

namespace formula {

/*!
 * @brief
 * Class for a PCTL formula tree with atomic proposition as root.
 *
 * This class represents the leaves in the formula tree.
 *
 * @see PctlStateFormula
 * @see PctlFormula
 */
template <class T>
class Ap : public PctlStateFormula<T> {

public:
	/*!
	 * Constructor
	 *
	 * Creates a new atomic proposition leaf, with the label Ap
	 *
	 * @param ap The string representing the atomic proposition
	 */
	Ap(std::string ap) {
		this->ap = ap;
	}

	/*!
	 * Destructor.
	 * At this time, empty...
	 */
	virtual ~Ap() { }

	/*!
	 * @returns the name of the atomic proposition
	 */
	const std::string& getAp() const {
		return ap;
	}

	/*!
	 * @returns a string representation of the leaf.
	 *
	 */
	virtual std::string toString() const {
		return getAp();
	}

	/*!
	 * Clones the called object.
	 *
	 * @returns a new Ap-object that is identical the called object.
	 */
	virtual PctlStateFormula<T>* clone() const {
	  return new Ap(ap);
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
	virtual mrmc::storage::BitVector *check(const mrmc::modelChecker::DtmcPrctlModelChecker<T>& modelChecker) const {
	  return modelChecker.checkAp(*this);
	}

private:
	std::string ap;
};

} //namespace formula

} //namespace mrmc

#endif /* MRMC_FORMULA_AP_H_ */
