/*
 * PathBoundOperator.h
 *
 *  Created on: 27.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_FORMULA_ABSTRACT_PATHBOUNDOPERATOR_H_
#define STORM_FORMULA_ABSTRACT_PATHBOUNDOPERATOR_H_

#include "src/formula/abstract/AbstractFormula.h"
#include "src/formula/abstract/AbstractFormula.h"
#include "src/formula/AbstractFormulaChecker.h"
#include "src/formula/ComparisonType.h"

#include "src/formula/abstract/OptimizingOperator.h"

#include "src/modelchecker/ForwardDeclarations.h"
#include "src/utility/ConstTemplates.h"

namespace storm {

namespace property {

namespace abstract {

/*!
 * @brief
 * Class for an abstract formula tree with a P (probablistic) operator node over a probability interval
 * as root.
 *
 * Has one formula as sub formula/tree.
 *
 * @par Semantics
 * 	  The formula holds iff the probability that the path formula holds is inside the bounds
 * 	  specified in this operator
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
 */
template<class T, class FormulaType>
class PathBoundOperator : public virtual AbstractFormula<T>, public OptimizingOperator {

public:
	/*!
	 * Constructor for non-optimizing operator.
	 *
	 * @param comparisonOperator The relation for the bound.
	 * @param bound The bound for the probability
	 * @param pathFormula The child node
	 */
	PathBoundOperator(storm::property::ComparisonType comparisonOperator, T bound, FormulaType* pathFormula)
		: comparisonOperator(comparisonOperator), bound(bound), pathFormula(pathFormula) {
		// Intentionally left empty
	}
	
	/*!
	 * Constructor for optimizing operator.
	 *
	 * @param comparisonOperator The relation for the bound.
	 * @param bound The bound for the probability
	 * @param pathFormula The child node
	 * @param minimumOperator Indicator, if operator should be minimum or maximum operator.
	 */
	PathBoundOperator(storm::property::ComparisonType comparisonOperator, T bound, FormulaType* pathFormula, bool minimumOperator)
		: comparisonOperator(comparisonOperator), bound(bound), pathFormula(pathFormula), OptimizingOperator(minimumOperator) {
		// Intentionally left empty
	}

	/*!
	 * Destructor
	 *
	 * The subtree is deleted with the object
	 * (this behavior can be prevented by setting them to NULL before deletion)
	 */
	virtual ~PathBoundOperator() {
	 if (pathFormula != nullptr) {
		 delete pathFormula;
	 }
	}

	/*!
	 * @returns the child node (representation of a formula)
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
	 * @returns the comparison relation
	 */
	const storm::property::ComparisonType getComparisonOperator() const {
		return comparisonOperator;
	}

	void setComparisonOperator(storm::property::ComparisonType comparisonOperator) {
		this->comparisonOperator = comparisonOperator;
	}

	/*!
	 * @returns the bound for the measure
	 */
	const T& getBound() const {
		return bound;
	}

	/*!
	 * Sets the interval in which the probability that the path formula holds may lie in.
	 *
	 * @param bound The bound for the measure
	 */
	void setBound(T bound) {
		this->bound = bound;
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::string result = "";
		switch (comparisonOperator) {
		case LESS: result += "<"; break;
		case LESS_EQUAL: result += "<="; break;
		case GREATER: result += ">"; break;
		case GREATER_EQUAL: result += ">="; break;
		}
		result += " ";
		result += std::to_string(bound);
		result += " [";
		result += pathFormula->toString();
		result += "]";
		return result;
	}

	bool meetsBound(T value) const {
		switch (comparisonOperator) {
		case LESS: return value < bound; break;
		case LESS_EQUAL: return value <= bound; break;
		case GREATER: return value > bound; break;
		case GREATER_EQUAL: return value >= bound; break;
		default: return false;
		}
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
	storm::property::ComparisonType comparisonOperator;
	T bound;
	FormulaType* pathFormula;
};

} //namespace abstract

} //namespace property

} //namespace storm

#endif /* STORM_FORMULA_ABSTRACT_PATHBOUNDOPERATOR_H_ */
