/*
 * Not.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_ABSTRACT_NOT_H_
#define STORM_FORMULA_ABSTRACT_NOT_H_

#include "src/formula/abstract/AbstractFormula.h"
#include "src/formula/AbstractFormulaChecker.h"
#include "src/modelchecker/ForwardDeclarations.h"

namespace storm {

namespace formula {

namespace abstract {

/*!
 * @brief
 * Class for a Abstract formula tree with NOT node as root.
 *
 * Has one Abstract state formula as sub formula/tree.
 *
 * The subtree is seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractFormula
 * @see AbstractFormula
 */
template <class T>
class Not : public AbstractFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	Not() {
		this->child = NULL;
	}

	/*!
	 * Constructor
	 * @param child The child node
	 */
	Not(AbstractFormula<T>* child) {
		this->child = child;
	}

	/*!
	 * Destructor
	 *
	 * Also deletes the subtree
	 * (this behavior can be prevented by setting them to NULL before deletion)
	 */
	virtual ~Not() {
	  if (child != NULL) {
		  delete child;
	  }
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::string result = "!";
		result += child->toString();
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
	 * @returns The child node
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

#endif /* STORM_FORMULA_ABSTRACT_NOT_H_ */
