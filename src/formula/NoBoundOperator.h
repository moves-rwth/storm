/*
 * NoBoundOperator.h
 *
 *  Created on: 27.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_FORMULA_NOBOUNDOPERATOR_H_
#define STORM_FORMULA_NOBOUNDOPERATOR_H_

#include "PctlFormula.h"
#include "PctlPathFormula.h"

#include "modelChecker/ForwardDeclarations.h"

namespace storm {

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
 * @note
 * 	This class does not contain a check() method like the other formula classes.
 * 	The check method should only be called by the model checker to infer the correct check function for sub
 * 	formulas. As this operator can only appear at the root, the method is not useful here.
 * 	Use the checkProbabilisticNoBoundOperator method from the DtmcPrctlModelChecker class instead.
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
class NoBoundOperator: public storm::formula::PctlFormula<T> {
public:
	/*!
	 * Empty constructor
	 */
	NoBoundOperator() {
		this->pathFormula = NULL;
	}

	/*!
	 * Constructor
	 *
	 * @param pathFormula The child node.
	 */
	NoBoundOperator(PctlPathFormula<T>* pathFormula) {
		this->pathFormula = pathFormula;
	}

	/*!
	 * Destructor
	 */
	virtual ~NoBoundOperator() {
		if (pathFormula != NULL) {
			delete pathFormula;
		}
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
	 * Calls the model checker to check this formula.
	 * Needed to infer the correct type of formula class.
	 *
	 * @note This function should only be called in a generic check function of a model checker class. For other uses,
	 *       the methods of the model checker should be used.
	 *
	 * @note This function is not implemented in this class.
	 *
	 * @returns A vector indicating all states that satisfy the formula represented by the called object.
	 */
	virtual std::vector<T>* check(const storm::modelChecker::DtmcPrctlModelChecker<T>& modelChecker) const {
		return modelChecker.checkNoBoundOperator(*this);
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const = 0;

private:
	PctlPathFormula<T>* pathFormula;
};

} /* namespace formula */

} /* namespace storm */

#endif /* STORM_FORMULA_NOBOUNDOPERATOR_H_ */
