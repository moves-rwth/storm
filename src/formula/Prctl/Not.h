/*
 * Not.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_PRCTL_NOT_H_
#define STORM_FORMULA_PRCTL_NOT_H_

#include "AbstractStateFormula.h"
#include "src/formula/AbstractFormulaChecker.h"
#include "src/modelchecker/prctl/ForwardDeclarations.h"

namespace storm {
namespace property {
namespace prctl {

template <class T> class Not;

/*!
 *  @brief Interface class for model checkers that support Not.
 *   
 *  All model checkers that support the formula class Not must inherit
 *  this pure virtual class.
 */
template <class T>
class INotModelChecker {
    public:
		/*!
         *  @brief Evaluates Not formula within a model checker.
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
        virtual storm::storage::BitVector checkNot(const Not<T>& obj) const = 0;
};

/*!
 * @brief
 * Class for an abstract formula tree with NOT node as root.
 *
 * Has one Abstract state formula as sub formula/tree.
 *
 * The subtree is seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractStateFormula
 * @see AbstractPrctlFormula
 */
template <class T>
class Not : public AbstractStateFormula<T> {

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
	Not(AbstractStateFormula<T>* child) {
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
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new AND-object that is identical the called object.
	 */
	virtual AbstractStateFormula<T>* clone() const override {
		Not<T>* result = new Not<T>();
		if (this->childIsSet()) {
			result->setChild(this->getChild().clone());
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
	 * @returns A bit vector indicating all states that satisfy the formula represented by the called object.
	 */
	virtual storm::storage::BitVector check(const storm::modelchecker::prctl::AbstractModelChecker<T>& modelChecker) const override {
		return modelChecker.template as<INotModelChecker>()->checkNot(*this);  
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const override {
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
	virtual bool validate(const AbstractFormulaChecker<T>& checker) const override {
		return checker.validate(this->child);
	}

	/*!
	 * @returns The child node
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
	 *
	 * @return True if the child node is set, i.e. it does not point to nullptr; false otherwise
	 */
	bool childIsSet() const {
		return child != nullptr;
	}

private:
	AbstractStateFormula<T>* child;
};

} //namespace prctl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_PRCTL_NOT_H_ */
