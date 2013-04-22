/*
 * ProbabilisticNoBoundOperator.h
 *
 *  Created on: 12.12.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_ABSTRACT_PROBABILISTICNOBOUNDOPERATOR_H_
#define STORM_FORMULA_ABSTRACT_PROBABILISTICNOBOUNDOPERATOR_H_

#include "AbstractFormula.h"
#include "src/formula/abstract/AbstractFormula.h"
#include "PathNoBoundOperator.h"

namespace storm {
namespace formula {
namespace abstract {

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
 * @tparam FormulaType The type of the subformula.
 * 		  The instantiation of FormulaType should be a subclass of AbstractFormula, as the functions
 * 		  "toString" and "conforms" of the subformulas are needed.
 *
 * @see AbstractFormula
 * @see PathNoBoundOperator
 * @see ProbabilisticBoundOperator
 */
template <class T, class FormulaType>
class ProbabilisticNoBoundOperator: public PathNoBoundOperator<T, FormulaType> {
public:
	/*!
	 * Empty constructor
	 */
	ProbabilisticNoBoundOperator() : PathNoBoundOperator<T, FormulaType>(nullptr) {
		// Intentionally left empty
	}

	/*!
	 * Constructor
	 *
	 * @param pathFormula The child node.
	 */
	ProbabilisticNoBoundOperator(FormulaType* pathFormula) : PathNoBoundOperator<T, FormulaType>(pathFormula) {
		// Intentionally left empty
	}

	/*!
	 * Destructor
	 */
	virtual ~ProbabilisticNoBoundOperator() {
		// Intentionally left empty
	}

	/*!
	 * Constructor
	 *
	 * @param pathFormula The child node.
	 */
	ProbabilisticNoBoundOperator(FormulaType* pathFormula, bool minimumOperator) : PathNoBoundOperator<T, FormulaType>(pathFormula, minimumOperator) {
		// Intentionally left empty
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::string result = "P";
		result += PathNoBoundOperator<T, FormulaType>::toString();
		return result;
	}
};

} //namespace abstract
} //namespace formula
} //namespace storm

#endif /* STORM_FORMULA_ABSTRACT_PROBABILISTICNOBOUNDOPERATOR_H_ */
