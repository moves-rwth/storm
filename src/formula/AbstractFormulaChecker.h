#ifndef STORM_FORMULA_ABSTRACTFORMULACHECKER_H_
#define STORM_FORMULA_ABSTRACTFORMULACHECKER_H_

#include "src/formula/AbstractFormula.h"

namespace storm {
namespace formula {

/*!
 *	@brief	Base class for all formula checkers.
 *
 *	A formula checker is used to check if a given formula is valid in some
 *	logic. Hence, this pure virtual base class should be subclassed for
 *	every logic we support.
 *
 *	Every subclass must implement conforms(). It gets a pointer to an
 *	AbstractFormula object and should return if the subtree represented by
 *	this formula is valid in the logic.
 *
 *	Usually, this will be implemented like this:
 *	@code
 *	if (
 *			dynamic_cast<const And<T>*>(formula) ||
 *			dynamic_cast<const Not<T>*>(formula) ||
 *			dynamic_cast<const Or<T>*>(formula)
 *		) {
 *		return formula->conforms(*this);
 *	} else return false;
 *	@endcode
 *
 *	Every formula class implements a conforms() method itself which calls
 *	conforms() on the given checker for every child in the formula tree.
 *
 *	If the formula structure is not an actual tree, but an directed acyclic
 *	graph, the shared subtrees will be checked twice. If we have directed
 *	cycles, we will have infinite recursions.
 */
template <class T>
class AbstractFormulaChecker {
	public:
		/*!
		 *	@brief Checks if the given formula is valid in some logic.
		 *
		 *	Every subclass must implement this method and check, if the
		 *	formula object is valid in the logic of the subclass.
		 *
		 *	@param formula A pointer to some formula object.
		 *	@return true iff the formula is valid.
		 */
		virtual bool conforms(const AbstractFormula<T>* formula) const = 0;
};

} // namespace formula
} // namespace storm

#endif