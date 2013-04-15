/*
 * Next.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_ABSTRACT_NEXT_H_
#define STORM_FORMULA_ABSTRACT_NEXT_H_

#include "src/formula/abstract/AbstractFormula.h"
#include "src/formula/AbstractFormulaChecker.h"

namespace storm {

namespace formula {

namespace abstract {

/*!
 * @brief
 * Class for a Abstract (path) formula tree with a Next node as root.
 *
 * Has two Abstract state formulas as sub formulas/trees.
 *
 * @par Semantics
 * The formula holds iff in the next step, \e child holds
 *
 * The subtree is seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractFormula
 * @see AbstractFormula
 */
template <class T>
class Next : public AbstractFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	Next() {
		this->child = NULL;
	}

	/*!
	 * Constructor
	 *
	 * @param child The child node
	 */
	Next(AbstractFormula<T>* child) {
		this->child = child;
	}

	/*!
	 * Constructor.
	 *
	 * Also deletes the subtree.
	 * (this behaviour can be prevented by setting the subtrees to NULL before deletion)
	 */
	virtual ~Next() {
	  if (child != NULL) {
		  delete child;
	  }
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::string result = "(";
		result += " X ";
		result += child->toString();
		result += ")";
		return result;
	}
	
	/*!
     *  @brief Checks if the subtree conforms to some logic.
     * 
     *  @param checker Formula checker object.
     *  @return true iff the subtree conforms to some logic.
     */
	virtual bool conforms(const AbstractFormulaChecker<T>& checker) const {
        return checker.conforms(this->child);
    }

protected:
	/*!
	 * @returns the child node
	 */
	const AbstractFormula<T>& getChild() const {
		return *child;
	}

	/*!
	 * Sets the subtree
	 * @param child the new child node
	 */
	void setChild(AbstractFormula<T>* child) {
		this->child = child;
	}

private:
	AbstractFormula<T>* child;
};

} //namespace abstract

} //namespace formula

} //namespace storm

#endif /* STORM_FORMULA_ABSTRACT_NEXT_H_ */
