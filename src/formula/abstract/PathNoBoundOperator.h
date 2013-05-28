/*
 * PathNoBoundOperator.h
 *
 *  Created on: 27.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_FORMULA_ABSTRACT_NOBOUNDOPERATOR_H_
#define STORM_FORMULA_ABSTRACT_NOBOUNDOPERATOR_H_

#include "src/formula/abstract/AbstractFormula.h"
#include "src/formula/AbstractFormulaChecker.h"
#include "src/formula/abstract/OptimizingOperator.h"

#include "src/modelchecker/ForwardDeclarations.h"

namespace storm {

namespace property {

namespace abstract {
/*!
 * @brief
 * Class for an abstract formula tree with a P (probablistic) operator without declaration of probabilities
 * as root.
 *
 * Checking a formula with this operator as root returns the probabilities that the path formula holds
 * (for each state)
 *
 * Has one formula as sub formula/tree.
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
 * @see PathBoundOperator
 */
template <class T, class FormulaType>
class PathNoBoundOperator: public virtual AbstractFormula<T>, public OptimizingOperator {

	// Throw a compiler error if FormulaType is not a subclass of AbstractFormula.
	static_assert(std::is_base_of<AbstractFormula<T>, FormulaType>::value,
				  "Instantiaton of FormulaType for storm::property::abstract::PathNoBoundOperator<T,FormulaType> has to be a subtype of storm::property::abstract::AbstractFormula<T>");


public:
	/*!
	 * Empty constructor
	 */
	PathNoBoundOperator() :
		OptimizingOperator(false) {
		this->pathFormula = nullptr;
	}

	/*!
	 * Constructor
	 *
	 * @param pathFormula The child node.
	 */
	PathNoBoundOperator(FormulaType* pathFormula) {
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
		: OptimizingOperator(minimumOperator) {
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
	 * @returns the child node (representation of an abstract path formula)
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
	virtual bool validate(const AbstractFormulaChecker<T>& checker) const {
		return checker.validate(this->pathFormula);
	}

private:
	FormulaType* pathFormula;
};

} //namespace abstract

} //namespace property

} //namespace storm

#endif /* STORM_FORMULA_ABSTRACT_NOBOUNDOPERATOR_H_ */
