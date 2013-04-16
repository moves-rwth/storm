/*
 * ProbabilisticNoBoundOperator.h
 *
 *  Created on: 12.12.2012
 *      Author: thomas
 */

#ifndef STORM_FORMULA_PRCTL_PROBABILISTICNOBOUNDOPERATOR_H_
#define STORM_FORMULA_PRCTL_PROBABILISTICNOBOUNDOPERATOR_H_

#include "AbstractPathFormula.h"
#include "src/formula/abstract/ProbabilisticNoBoundOperator.h"

namespace storm {
namespace formula {
namespace prctl {

/*!
 * @brief
 * Class for a Abstract formula tree with a P (probablistic) operator without declaration of probabilities
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
 * @see AbstractFormula
 */
template <class T>
class ProbabilisticNoBoundOperator: public storm::formula::abstract::ProbabilisticNoBoundOperator<T, AbstractPathFormula<T>>,
												public AbstractStateFormula<T> {
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
		: storm::formula::abstract::ProbabilisticNoBoundOperator<T, AbstractPathFormula<T>>(pathFormula) {
		// Intentionally left empty
	}

	/*!
	 * Constructor
	 *
	 * @param pathFormula The child node.
	 */
	ProbabilisticNoBoundOperator(AbstractPathFormula<T>* pathFormula, bool minimumOperator)
		: storm::formula::abstract::ProbabilisticNoBoundOperator<T, AbstractPathFormula<T>>(pathFormula, minimumOperator) {
		// Intentionally left empty
	}

	/*!
	 * Destructor
	 */
	virtual ~ProbabilisticNoBoundOperator() {
		// Intentionally left empty
	}
};

} //namespace formula
} //namespace formula
} //namespace storm

#endif /* STORM_FORMULA_PRCTL_PROBABILISTICNOBOUNDOPERATOR_H_ */
