/*
 * ProbabilisticBoundOperator.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_CSL_PROBABILISTICBOUNDOPERATOR_H_
#define STORM_FORMULA_CSL_PROBABILISTICBOUNDOPERATOR_H_

#include "src/formula/Csl/AbstractStateFormula.h"
#include "src/formula/Csl/AbstractPathFormula.h"
#include "src/formula/ComparisonType.h"
#include "utility/constants.h"

namespace storm {
namespace property {
namespace csl {

template <class T> class ProbabilisticBoundOperator;

/*!
 *  @brief Interface class for model checkers that support ProbabilisticBoundOperator.
 *
 *  All model checkers that support the formula class PathBoundOperator must inherit
 *  this pure virtual class.
 */
template <class T>
class IProbabilisticBoundOperatorModelChecker {
    public:
        virtual storm::storage::BitVector checkProbabilisticBoundOperator(const ProbabilisticBoundOperator<T>& obj) const = 0;
};

/*!
 * @brief
 * Class for an abstract formula tree with a P (probablistic) operator node over a probability interval
 * as root.
 *
 * Has one Abstract path formula as sub formula/tree.
 *
 * @par Semantics
 * 	  The formula holds iff the probability that the path formula holds is inside the bounds
 * 	  specified in this operator
 *
 * The subtree is seen as part of the object and deleted with it
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 *
 * @see AbstractStateFormula
 * @see AbstractPathFormula
 * @see ProbabilisticOperator
 * @see ProbabilisticNoBoundsOperator
 * @see AbstractCslFormula
 */
template<class T>
class ProbabilisticBoundOperator : public AbstractStateFormula<T> {

public:

	/*!
	 * Empty constructor
	 */
	ProbabilisticBoundOperator() : comparisonOperator(LESS), bound(0), pathFormula(nullptr) {
		// Intentionally left empty.
	}

	/*!
	 * Constructor for non-optimizing operator.
	 *
	 * @param comparisonOperator The relation for the bound.
	 * @param bound The bound for the probability
	 * @param pathFormula The child node
	 */
	ProbabilisticBoundOperator(storm::property::ComparisonType comparisonOperator, T bound, AbstractPathFormula<T>* pathFormula)
		: comparisonOperator(comparisonOperator), bound(bound), pathFormula(pathFormula) {
		// Intentionally left empty.
	}

	/*!
	 * Destructor
	 *
	 * The subtree is deleted with the object
	 * (this behavior can be prevented by setting them to NULL before deletion)
	 */
	virtual ~ProbabilisticBoundOperator() {
	 if (pathFormula != nullptr) {
		 delete pathFormula;
	 }
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new AND-object that is identical the called object.
	 */
	virtual AbstractStateFormula<T>* clone() const override {
		ProbabilisticBoundOperator<T>* result = new ProbabilisticBoundOperator<T>();
		result->setComparisonOperator(this->getComparisonOperator());
		result->setBound(this->getBound());
		result->setPathFormula(this->getPathFormula().clone());
		return result;
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
	virtual storm::storage::BitVector check(const storm::modelchecker::csl::AbstractModelChecker<T>& modelChecker) const override {
		return modelChecker.template as<IProbabilisticBoundOperatorModelChecker>()->checkProbabilisticBoundOperator(*this);
	}

	/*!
	 *  @brief Checks if the subtree conforms to some logic.
	 *
	 *  @param checker Formula checker object.
	 *  @return true iff the subtree conforms to some logic.
	 */
	virtual bool validate(const AbstractFormulaChecker<T>& checker) const override {
		return checker.validate(this->pathFormula);
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const override {
		std::string result = "P ";
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

	/*!
	 * @returns the child node (representation of a formula)
	 */
	const AbstractPathFormula<T>& getPathFormula () const {
		return *pathFormula;
	}

	/*!
	 * Sets the child node
	 *
	 * @param pathFormula the path formula that becomes the new child node
	 */
	void setPathFormula(AbstractPathFormula<T>* pathFormula) {
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

	bool meetsBound(T value) const {
		switch (comparisonOperator) {
		case LESS: return value < bound; break;
		case LESS_EQUAL: return value <= bound; break;
		case GREATER: return value > bound; break;
		case GREATER_EQUAL: return value >= bound; break;
		default: return false;
		}
	}

private:
	storm::property::ComparisonType comparisonOperator;
	T bound;
	AbstractPathFormula<T>* pathFormula;
};

} //namespace csl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_CSL_PROBABILISTICBOUNDOPERATOR_H_ */
