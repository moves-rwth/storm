/*
 * BoundedUntil.h
 *
 *  Created on: 27.11.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_FORMULA_ABSTRACT_BOUNDEDEVENTUALLY_H_
#define STORM_FORMULA_ABSTRACT_BOUNDEDEVENTUALLY_H_

#include "src/formula/abstract/AbstractFormula.h"
#include "src/formula/AbstractFormulaChecker.h"
#include "boost/integer/integer_mask.hpp"
#include <string>
#include "src/modelchecker/ForwardDeclarations.h"

namespace storm {

namespace property {

namespace abstract {

/*!
 * @brief
 * Class for an abstract (path) formula tree with a BoundedEventually node as root.
 *
 * Has one formula as sub formula/tree.
 *
 * @par Semantics
 * The formula holds iff in at most \e bound steps, formula \e child holds.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @tparam FormulaType The type of the subformula.
 * 		  The instantiation of FormulaType should be a subclass of AbstractFormula, as the functions
 * 		  "toString" and "validate" of the subformulas are needed.
 *
 * @see AbstractFormula
 */
template <class T, class FormulaType>
class BoundedEventually : public virtual AbstractFormula<T> {

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
	BoundedEventually(FormulaType* child, uint_fast64_t bound) {
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
	const FormulaType& getChild() const {
		return *child;
	}

	/*!
	 * Sets the subtree
	 * @param child the new child node
	 */
	void setChild(FormulaType* child) {
		this->child = child;
	}

	/*!
	 *
	 * @return True if the child is set, i.e. it does not point to nullptr; false otherwise
	 */
	bool childIsSet() const {
		return child != nullptr;
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
     *  @brief Checks if the subtree conforms to some logic.
     * 
     *  @param checker Formula checker object.
     *  @return true iff the subtree conforms to some logic.
     */
	virtual bool validate(const AbstractFormulaChecker<T>& checker) const {
		return checker.validate(this->child);
	}


private:
	FormulaType* child;
	uint_fast64_t bound;
};

} //namespace abstract

} //namespace property

} //namespace storm

#endif /* STORM_FORMULA_ABSTRACT_BOUNDEDEVENTUALLY_H_ */
