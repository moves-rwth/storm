/*
 * Not.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_CSL_NOT_H_
#define STORM_FORMULA_CSL_NOT_H_

#include "AbstractStateFormula.h"
#include "src/formula/abstract/Not.h"
#include "src/formula/AbstractFormulaChecker.h"
#include "src/modelchecker/ForwardDeclarations.h"

namespace storm {
namespace formula {
namespace csl {

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
        virtual storm::storage::BitVector* checkNot(const Not<T>& obj) const = 0;
};

/*!
 * @brief
 * Class for a Abstract formula tree with NOT node as root.
 *
 * Has one Abstract state formula as sub formula/tree.
 *
 * The subtree is seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see AbstractStateFormula
 * @see AbstractFormula
 */
template <class T>
class Not : public storm::formula::abstract::Not<T, AbstractStateFormula<T>>,
			   public AbstractStateFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	Not() {
		//intentionally left empty
	}

	/*!
	 * Constructor
	 * @param child The child node
	 */
	Not(AbstractStateFormula<T>* child) :
		storm::formula::abstract::Not<T, AbstractStateFormula<T>>(child){
		//intentionally left empty
	}

	/*!
	 * Destructor
	 *
	 * Also deletes the subtree
	 * (this behavior can be prevented by setting them to NULL before deletion)
	 */
	virtual ~Not() {
	  //intentionally left empty
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new AND-object that is identical the called object.
	 */
	virtual AbstractStateFormula<T>* clone() const {
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
	virtual storm::storage::BitVector *check(const storm::modelchecker::AbstractModelChecker<T>& modelChecker) const {
		return modelChecker.template as<INotModelChecker>()->checkNot(*this);  
	}
};

} //namespace csl
} //namespace formula
} //namespace storm

#endif /* STORM_FORMULA_CSL_NOT_H_ */
