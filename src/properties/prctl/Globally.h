/*
 * Globally.h
 *
 *  Created on: 26.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_FORMULA_PRCTL_GLOBALLY_H_
#define STORM_FORMULA_PRCTL_GLOBALLY_H_

#include "src/properties/prctl/AbstractPathFormula.h"
#include "src/properties/prctl/AbstractStateFormula.h"
#include "src/modelchecker/prctl/ForwardDeclarations.h"

namespace storm {
namespace properties {
namespace prctl {

// Forward declaration for the interface class.
template <class T> class Globally;

/*!
 * Interface class for model checkers that support Globally.
 *   
 * All model checkers that support the formula class Globally must inherit
 * this pure virtual class.
 */
template <class T>
class IGloballyModelChecker {
    public:

		/*!
		 * Empty virtual destructor.
		 */
		virtual ~IGloballyModelChecker() {
			// Intentionally left empty
		}

		/*!
		 * Evaluates a Globally formula within a model checker.
		 *
		 * @param obj Formula object with subformulas.
		 * @param qualitative A flag indicating whether the formula only needs to be evaluated qualitatively, i.e. if the
         *                    results are only compared against the bounds 0 and 1.
		 * @return The modelchecking result of the formula for every state.
		 */
        virtual std::vector<T> checkGlobally(const Globally<T>& obj, bool qualitative) const = 0;
};

/*!
 * Class for an Prctl (path) formula tree with a Globally node as root.
 *
 * Has one state formula as sub formula/tree.
 *
 * @par Semantics
 * The formula holds iff globally formula \e child holds.
 *
 * The object has shared ownership of its subtree. If this object is deleted and no other object has a shared
 * ownership of the subtree it will be deleted as well.
 *
 * @see AbstractPathFormula
 * @see AbstractPrctlFormula
 */
template <class T>
class Globally : public AbstractPathFormula<T> {

public:

	/*!
	 * Creates a Globally node without a subnode.
	 * The resulting object will not represent a complete formula!
	 */
	Globally() : child(nullptr){
		// Intentionally left empty.
	}

	/*!
	 * Creates a Globally node using the given parameter.
	 *
	 * @param child The child formula subtree.
	 */
	Globally(std::shared_ptr<AbstractStateFormula<T>> const & child) : child(child){
		// Intentionally left empty.
	}

	/*!
	 * Empty virtual destructor.
	 */
	virtual ~Globally() {
		// Intentionally left empty.
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subnodes of the new object are clones of the original ones.
	 *
	 * @returns A new Globally object that is a deep copy of the called object.
	 */
	virtual std::shared_ptr<AbstractPathFormula<T>> clone() const override {
		std::shared_ptr<Globally<T>> result(new Globally<T>());
		if (this->isChildSet()) {
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
	virtual std::vector<T> check(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelChecker, bool qualitative) const override {
		return modelChecker.template as<IGloballyModelChecker>()->checkGlobally(*this, qualitative);
	}

	/*!
	 * Returns a textual representation of the formula tree with this node as root.
	 *
	 * @returns A string representing the formula tree.
	 */
	virtual std::string toString() const override {
		std::string result = "G ";
		result += child->toString();
		return result;
	}

	/*!
	 * Gets the child node.
	 *
	 * @returns The child node.
	 */
	std::shared_ptr<AbstractStateFormula<T>> const & getChild() const {
		return child;
	}

	/*!
	 * Sets the subtree.
	 *
	 * @param child The new child.
	 */
	void setChild(std::shared_ptr<AbstractStateFormula<T>> const & child) {
		this->child = child;
	}

	/*!
	 * Checks if the child is set, i.e. it does not point to null.
	 *
	 * @return True iff the child is set.
	 */
	bool isChildSet() const {
		return child.get() != nullptr;
	}

private:

	// The child node.
	std::shared_ptr<AbstractStateFormula<T>> child;
};

} //namespace prctl
} //namespace properties
} //namespace storm

#endif /* STORM_FORMULA_PRCTL_GLOBALLY_H_ */
