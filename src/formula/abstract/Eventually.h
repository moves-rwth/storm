/*
 * Next.h
 *
 *  Created on: 26.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_FORMULA_ABSTRACT_EVENTUALLY_H_
#define STORM_FORMULA_ABSTRACT_EVENTUALLY_H_

#include "src/formula/abstract/AbstractFormula.h"
#include "src/modelchecker/ForwardDeclarations.h"

namespace storm {

namespace property {

namespace abstract {

/*!
 * @brief
 * Class for an abstract (path) formula tree with an Eventually node as root.
 *
 * Has one formula as sub formula/tree.
 *
 * @par Semantics
 * The formula holds iff eventually \e child holds.
 *
 * The subtree is seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to nullptr before deletion)
 *
 * @tparam FormulaType The type of the subformula.
 * 		  The instantiation of FormulaType should be a subclass of AbstractFormula, as the functions
 * 		  "toString" and "conforms" of the subformulas are needed.
 *
 * @see AbstractFormula
 */
template <class T, class FormulaType>
class Eventually : public virtual AbstractFormula<T> {

	// Throw a compiler error when FormulaType is not a subclass of AbstractFormula.
	static_assert(std::is_base_of<AbstractFormula<T>, FormulaType>::value,
				  "Instantiaton of FormulaType for storm::property::abstract::Eventually<T,FormulaType> has to be a subtype of storm::property::abstract::AbstractFormula<T>");

public:
	/*!
	 * Empty constructor
	 */
	Eventually() {
		this->child = nullptr;
	}

	/*!
	 * Constructor
	 *
	 * @param child The child node
	 */
	Eventually(FormulaType* child) {
		this->child = child;
	}

	/*!
	 * Constructor.
	 *
	 * Also deletes the subtree.
	 * (this behaviour can be prevented by setting the subtrees to nullptr before deletion)
	 */
	virtual ~Eventually() {
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
	 * @return True if the child node is set, i.e. it does not point to nullptr; false otherwise
	 */
	bool childIsSet() const {
		return child != nullptr;
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::string result = "F ";
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

#endif /* STORM_FORMULA_ABSTRACT_EVENTUALLY_H_ */
