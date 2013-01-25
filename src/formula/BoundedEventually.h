/*
 * BoundedUntil.h
 *
 *  Created on: 27.11.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_FORMULA_BOUNDEDEVENTUALLY_H_
#define STORM_FORMULA_BOUNDEDEVENTUALLY_H_

#include "AbstractPathFormula.h"
#include "AbstractStateFormula.h"
#include "boost/integer/integer_mask.hpp"
#include <string>
#include "src/modelChecker/AbstractModelChecker.h"

namespace storm {

namespace formula {

template <class T> class BoundedEventually;

template <class T>
class IBoundedEventuallyModelChecker {
    public:
        virtual std::vector<T>* checkBoundedEventually(const BoundedEventually<T>& obj) const = 0;
};

/*!
 * @brief
 * Class for a Abstract (path) formula tree with a BoundedEventually node as root.
 *
 * Has one Abstract state formulas as sub formula/tree.
 *
 * @par Semantics
 * The formula holds iff in at most \e bound steps, formula \e child holds.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractPathFormula
 * @see AbstractFormula
 */
template <class T>
class BoundedEventually : public AbstractPathFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	BoundedEventually() {
		this->child = nullptr;
		bound = 0;
	}

	/*!
	 * Constructor
	 *
	 * @param child The child formula subtree
	 * @param bound The maximal number of steps
	 */
	BoundedEventually(AbstractStateFormula<T>* child, uint_fast64_t bound) {
		this->child = child;
		this->bound = bound;
	}

	/*!
	 * Destructor.
	 *
	 * Also deletes the subtrees.
	 * (this behaviour can be prevented by setting the subtrees to NULL before deletion)
	 */
	virtual ~BoundedEventually() {
	  if (child != nullptr) {
		  delete child;
	  }
	}

	/*!
	 * @returns the child node
	 */
	const AbstractStateFormula<T>& getChild() const {
		return *child;
	}

	/*!
	 * Sets the subtree
	 * @param child the new child node
	 */
	void setChild(AbstractStateFormula<T>* child) {
		this->child = child;
	}

	/*!
	 * @returns the maximally allowed number of steps for the bounded until operator
	 */
	uint_fast64_t getBound() const {
		return bound;
	}

	/*!
	 * Sets the maximally allowed number of steps for the bounded until operator
	 *
	 * @param bound the new bound.
	 */
	void setBound(uint_fast64_t bound) {
		this->bound = bound;
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::string result = "F<=";
		result += std::to_string(bound);
		result += " ";
		result += child->toString();
		return result;
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new BoundedUntil-object that is identical the called object.
	 */
	virtual AbstractPathFormula<T>* clone() const {
		BoundedEventually<T>* result = new BoundedEventually<T>();
		result->setBound(bound);
		if (child != nullptr) {
			result->setChild(child->clone());
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
	 * @returns A vector indicating the probability that the formula holds for each state.
	 */
	virtual std::vector<T> *check(const storm::modelChecker::AbstractModelChecker<T>& modelChecker) const {
	  return this->template cast<IBoundedEventuallyModelChecker>(modelChecker)->checkBoundedEventually(*this);
	}

private:
	AbstractStateFormula<T>* child;
	uint_fast64_t bound;
};

} //namespace formula

} //namespace storm

#endif /* STORM_FORMULA_BOUNDEDUNTIL_H_ */
