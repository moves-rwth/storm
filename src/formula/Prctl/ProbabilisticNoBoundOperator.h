/*
 * ProbabilisticNoBoundOperator.h
 *
 *  Created on: 12.12.2012
 *      Author: thomas
 */

#ifndef STORM_FORMULA_PRCTL_PROBABILISTICNOBOUNDOPERATOR_H_
#define STORM_FORMULA_PRCTL_PROBABILISTICNOBOUNDOPERATOR_H_

#include "AbstractPathFormula.h"
#include "AbstractNoBoundOperator.h"
#include "src/formula/abstract/ProbabilisticNoBoundOperator.h"

namespace storm {
namespace property {
namespace prctl {

/*!
 * @brief
 * Class for an abstract formula tree with a P (probablistic) operator without declaration of probabilities
 * as root.
 *
 * Checking a formula with this operator as root returns the probabilities that the path formula holds
 * (for each state)
 *
 * Has one Abstract path formula as sub formula/tree.
 *
 * @note
 * 	This class is a hybrid of a state and path formula, and may only appear as the outermost operator.
 * 	Hence, it is seen as neither a state nor a path formula, but is directly derived from AbstractFormula.
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
 * @see AbstractStateFormula
 * @see AbstractPathFormula
 * @see ProbabilisticOperator
 * @see ProbabilisticIntervalOperator
 * @see AbstractPrctlFormula
 */
template <class T>
class ProbabilisticNoBoundOperator: public storm::property::abstract::ProbabilisticNoBoundOperator<T, AbstractPathFormula<T>>,
												public AbstractNoBoundOperator<T> {
public:
	/*!
	 * Empty constructor
	 */
	ProbabilisticNoBoundOperator() {
		// Intentionally left empty
	}

	/*!
	 * Constructor
	 *
	 * @param pathFormula The child node.
	 */
	ProbabilisticNoBoundOperator(AbstractPathFormula<T>* pathFormula)
		: storm::property::abstract::ProbabilisticNoBoundOperator<T, AbstractPathFormula<T>>(pathFormula) {
		// Intentionally left empty
	}

	/*!
	 * Constructor
	 *
	 * @param pathFormula The child node.
	 */
	ProbabilisticNoBoundOperator(AbstractPathFormula<T>* pathFormula, bool minimumOperator)
		: storm::property::abstract::ProbabilisticNoBoundOperator<T, AbstractPathFormula<T>>(pathFormula, minimumOperator) {
		// Intentionally left empty
	}

	/*!
	 * Destructor
	 */
	virtual ~ProbabilisticNoBoundOperator() {
		// Intentionally left empty
	}

	virtual AbstractNoBoundOperator<T>* clone() const {
		ProbabilisticNoBoundOperator<T>* result = new ProbabilisticNoBoundOperator<T>();
		if (this->pathFormulaIsSet()) {
			result->setPathFormula(this->getPathFormula().clone());
		}
		return result;
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
	 * @returns A vector indicating the probability that the formula holds for each state.
	 */
	virtual std::vector<T>* check(const storm::modelchecker::prctl::AbstractModelChecker<T>& modelChecker, bool qualitative=false) const {
		return this->getPathFormula().check(modelChecker, qualitative);
	}
};

} //namespace prctl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_PRCTL_PROBABILISTICNOBOUNDOPERATOR_H_ */
