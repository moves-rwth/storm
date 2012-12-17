/*
 * ProbabilisticNoBoundsOperator.h
 *
 *  Created on: 12.12.2012
 *      Author: thomas
 */

#ifndef MRMC_FORMULA_PROBABILISTICNOBOUNDSOPERATOR_H_
#define MRMC_FORMULA_PROBABILISTICNOBOUNDSOPERATOR_H_

#include "PctlFormula.h"
#include "PctlPathFormula.h"

namespace mrmc {
namespace formula {

/*!
 * @brief
 * Class for a PCTL formula tree with a P (probablistic) operator without declaration of probabilities
 * as root.
 *
 * Checking a formula with this operator as root returns the probabilities that the path formula holds
 * (for each state)
 *
 * Has one PCTL path formula as sub formula/tree.
 *
 * @note
 * 	This class is a hybrid of a state and path formula, and may only appear as the outermost operator.
 * 	Hence, it is seen as neither a state nor a path formula, but is directly derived from PctlFormula.
 *
 * The subtree is seen as part of the object and deleted with it
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 *
 * @see PctlStateFormula
 * @see PctlPathFormula
 * @see ProbabilisticOperator
 * @see ProbabilisticIntervalOperator
 * @see PctlFormula
 */
template <class T>
class ProbabilisticNoBoundsOperator: public mrmc::formula::PctlFormula<T> {
public:
	/*!
	 * Empty constructor
	 */
	ProbabilisticNoBoundsOperator() {
		// TODO Auto-generated constructor stub
		this->pathFormula = NULL;
	}

	/*!
	 * Constructor
	 *
	 * @param pathFormula The child node.
	 */
	ProbabilisticNoBoundsOperator(PctlPathFormula<T> &pathFormula) {
		this->pathFormula = &pathFormula;
	}

	/*!
	 * Destructor
	 */
	virtual ~ProbabilisticNoBoundsOperator() {
		// TODO Auto-generated destructor stub
	}

	/*!
	 * @returns the child node (representation of a PCTL path formula)
	 */
	const PctlPathFormula<T>& getPathFormula () const {
		return *pathFormula;
	}

	/*!
	 * Sets the child node
	 *
	 * @param pathFormula the path formula that becomes the new child node
	 */
	void setPathFormula(PctlPathFormula<T>* pathFormula) {
		this->pathFormula = pathFormula;
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::string result = " P=? (";
		result += pathFormula->toString();
		result += ")";
		return result;
	}

private:
	PctlPathFormula<T>* pathFormula;
};

} /* namespace formula */
} /* namespace mrmc */

#endif /* MRMC_FORMULA_PROBABILISTICNOBOUNDSOPERATOR_H_ */
