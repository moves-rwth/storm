/*
 * PathNoBoundOperator.h
 *
 *  Created on: 27.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_FORMULA_ABSTRACT_NOBOUNDOPERATOR_H_
#define STORM_FORMULA_ABSTRACT_NOBOUNDOPERATOR_H_

#include "src/formula/abstract/AbstractFormula.h"
#include "src/formula/abstract/AbstractFormula.h"
#include "src/formula/AbstractFormulaChecker.h"
#include "src/formula/abstract/OptimizingOperator.h"

#include "src/modelchecker/ForwardDeclarations.h"

namespace storm {

namespace formula {

namespace abstract {
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
 * @see AbstractFormula
 * @see AbstractFormula
 * @see ProbabilisticOperator
 * @see ProbabilisticIntervalOperator
 * @see AbstractFormula
 */
template <class T, class FormulaType>
class PathNoBoundOperator: public storm::formula::AbstractFormula<T>, public OptimizingOperator {
public:
	/*!
	 * Empty constructor
	 */
	PathNoBoundOperator() : optimalityOperator(false), minimumOperator(false) {
		this->pathFormula = nullptr;
	}

	/*!
	 * Constructor
	 *
	 * @param pathFormula The child node.
	 */
	PathNoBoundOperator(FormulaType* pathFormula) : optimalityOperator(false), minimumOperator(false) {
		this->pathFormula = pathFormula;
	}

	/*!
	 * Constructor
	 *
	 * @param pathFormula The child node.
	 * @param minimumOperator A flag indicating whether this operator is a minimizing or a
	 * maximizing operator.
	 */
	PathNoBoundOperator(FormulaType* pathFormula, bool minimumOperator)
		: optimalityOperator(true), minimumOperator(minimumOperator) {
		this->pathFormula = pathFormula;
	}

	/*!
	 * Destructor
	 */
	virtual ~PathNoBoundOperator() {
		if (pathFormula != NULL) {
			delete pathFormula;
		}
	}

	/*!
	 * @returns the child node (representation of a Abstract path formula)
	 */
	const FormulaType& getPathFormula () const {
		return *pathFormula;
	}

	/*!
	 * Sets the child node
	 *
	 * @param pathFormula the path formula that becomes the new child node
	 */
	void setPathFormula(FormulaType* pathFormula) {
		this->pathFormula = pathFormula;
	}

	/*!
	 *
	 * @return True if the path formula is set, i.e. it does not point to nullptr; false otherwise
	 */
	bool pathFormulaIsSet() const {
		return pathFormula != nullptr;
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::string result;
		if (this->isOptimalityOperator()) {
			if (this->isMinimumOperator()) {
				result += "min";
			} else {
				result += "max";
			}
		}
		result += " = ? [";
		result += this->getPathFormula().toString();
		result += "]";
		return result;
	}
	
	/*!
     *  @brief Checks if the subtree conforms to some logic.
     * 
     *  @param checker Formula checker object.
     *  @return true iff the subtree conforms to some logic.
     */
	virtual bool conforms(const AbstractFormulaChecker<T>& checker) const {
		return checker.conforms(this->pathFormula);
	}

	/*!
	 * Retrieves whether the operator is to be interpreted as an optimizing (i.e. min/max) operator.
	 * @returns True if the operator is an optimizing operator.
	 */
	bool isOptimalityOperator() const {
		return optimalityOperator;
	}

	/*!
	 * Retrieves whether the operator is a minimizing operator given that it is an optimality
	 * operator.
	 * @returns True if the operator is an optimizing operator and it is a minimizing operator and
	 * false otherwise, i.e. if it is either not an optimizing operator or not a minimizing operator.
	 */
	bool isMinimumOperator() const {
		return optimalityOperator && minimumOperator;
	}

private:
	FormulaType* pathFormula;

	// A flag that indicates whether this operator is meant as an optimizing (i.e. min/max) operator
	// over a nondeterministic model.
	bool optimalityOperator;

	// In the case this operator is an optimizing operator, this flag indicates whether it is
	// looking for the minimum or the maximum value.
	bool minimumOperator;
};

} //namespace abstract

} //namespace formula

} //namespace storm

#endif /* STORM_FORMULA_ABSTRACT_NOBOUNDOPERATOR_H_ */
