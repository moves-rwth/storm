/*
 * Next.h
 *
 *  Created on: 26.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_FORMULA_PRCTL_EVENTUALLY_H_
#define STORM_FORMULA_PRCTL_EVENTUALLY_H_

#include "src/formula/abstract/Eventually.h"
#include "src/formula/Prctl/AbstractPathFormula.h"
#include "src/formula/Prctl/AbstractStateFormula.h"
#include "src/modelchecker/ForwardDeclarations.h"

namespace storm {
namespace property {
namespace prctl {

template <class T> class Eventually;

/*!
 *  @brief Interface class for model checkers that support Eventually.
 *
 *  All model checkers that support the formula class Eventually must inherit
 *  this pure virtual class.
 */
template <class T>
class IEventuallyModelChecker {
    public:
		/*!
         *  @brief Evaluates Eventually formula within a model checker.
         *
         *  @param obj Formula object with subformulas.
         *  @return Result of the formula for every node.
         */
        virtual std::vector<T>* checkEventually(const Eventually<T>& obj, bool qualitative) const = 0;
};

/*!
 * @brief
 * Class for an abstract (path) formula tree with an Eventually node as root.
 *
 * Has one Abstract state formula as sub formula/tree.
 *
 * @par Semantics
 * The formula holds iff eventually \e child holds.
 *
 * The subtree is seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to nullptr before deletion)
 *
 * @see AbstractPathFormula
 * @see AbstractPrctlFormula
 */
template <class T>
class Eventually : public storm::property::abstract::Eventually<T, AbstractStateFormula<T>>,
						 public AbstractPathFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	Eventually() {
		// Intentionally left empty
	}

	/*!
	 * Constructor
	 *
	 * @param child The child node
	 */
	Eventually(AbstractStateFormula<T>* child)
		: storm::property::abstract::Eventually<T, AbstractStateFormula<T>>(child) {

	}

	/*!
	 * Constructor.
	 *
	 * Also deletes the subtree.
	 * (this behaviour can be prevented by setting the subtrees to nullptr before deletion)
	 */
	virtual ~Eventually() {
		//intentionally left empty
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new Eventually-object that is identical the called object.
	 */
	virtual AbstractPathFormula<T>* clone() const {
		Eventually<T>* result = new Eventually<T>();
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
	 * @returns A vector indicating the probability that the formula holds for each state.
	 */
	virtual std::vector<T> *check(const storm::modelchecker::AbstractModelChecker<T>& modelChecker, bool qualitative) const {
		return modelChecker.template as<IEventuallyModelChecker>()->checkEventually(*this, qualitative);
	}
};

} //namespace prctl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_PRCTL_EVENTUALLY_H_ */
