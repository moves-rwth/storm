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

namespace property {

namespace abstract {

/*!
 * @brief
 * Class for an abstract formula tree with NOT node as root.
 *
 * Has one formula as sub formula/tree.
 *
 * The subtree is seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @tparam FormulaType The type of the subformula.
 * 		  The instantiation of FormulaType should be a subclass of AbstractFormula, as the functions
 * 		  "toString" and "conforms" of the subformulas are needed.
 *
 * @see AbstractFormula
 */
template <class T, class FormulaType>
class Not : public virtual AbstractFormula<T> {

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
	Not(FormulaType* child) {
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
	 * @returns The child node
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
	 * @return True if the child node is set, i.e. it does not point to nullptr; false otherwise
	 */
	bool childIsSet() const {
		return child != nullptr;
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
	virtual bool validate(const AbstractFormulaChecker<T>& checker) const {
		return checker.validate(this->child);
	}

private:
	FormulaType* child;
};

} //namespace abstract

} //namespace property

} //namespace storm

#endif /* STORM_FORMULA_ABSTRACT_NOT_H_ */
