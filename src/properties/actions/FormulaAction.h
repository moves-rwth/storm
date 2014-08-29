/*
 * PrctlFormulaAction.h
 *
 *  Created on: Jun 6, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_ACTION_FORMULAACTION_H_
#define STORM_FORMULA_ACTION_FORMULAACTION_H_

#include "src/properties/actions/AbstractAction.h"
#include "src/properties/prctl/AbstractStateFormula.h"
#include "src/properties/csl/AbstractStateFormula.h"


#include <string>

namespace storm {
namespace properties {
namespace action {

/*!
 * The Formula action deselects all states that do not satisfy the given state formula.
 *
 * @note Since there is no modelchecker implemented for Ltl formulas (except the abstract base class)
 *       the formula action currently only supports Prctl and Csl state formulas.
 */
template <class T>
class FormulaAction : public AbstractAction<T> {

	// Convenience typedef to make the code more readable.
	typedef typename AbstractAction<T>::Result Result;

public:


	/*!
	 * Constructs an empty FormulaAction.
	 *
	 * The evaluation will do nothing and return the Result as it was put in.
	 */
	FormulaAction() : prctlFormula(nullptr), cslFormula(nullptr) {
		//Intentionally left empty.
	}

	/*!
	 * Constructs a FormulaAction using the given Prctl formula.
	 *
	 * @param prctlFormula The Prctl state formula used to filter the selection during evaluation.
	 */
	FormulaAction(std::shared_ptr<storm::properties::prctl::AbstractStateFormula<T>> const & prctlFormula) : prctlFormula(prctlFormula), cslFormula(nullptr) {
		//Intentionally left empty.
	}

	/*!
	 * Constructs a FormulaAction using the given Csl formula.
	 *
	 * @param cslFormula The Csl state formula used to filter the selection during evaluation.
	 */
	FormulaAction(std::shared_ptr<storm::properties::csl::AbstractStateFormula<T>> const & cslFormula) : prctlFormula(nullptr), cslFormula(cslFormula) {
		//Intentionally left empty.
	}

	/*!
	 * The virtual destructor.
	 * To ensure that the right destructor is called.
	 */
	virtual ~FormulaAction() {
		// Intentionally left empty.
	}

	/*!
	 * Evaluate the action for an Prctl formula.
	 *
	 * @param result The Result struct on which the action is to be evaluated.
	 * @param mc The Prctl modelchecker that computed the path- or stateResult contained in the Result struct.
	 * @returns A Result struct containing the output of the action.
	 */
	virtual Result evaluate(Result const & result, storm::modelchecker::prctl::AbstractModelChecker<T> const & mc) const override {

		storm::storage::BitVector selection(result.selection);

		//Compute the modelchecking results of the actions state formula and deselect all states that do not satisfy it.
		if(prctlFormula.get() != nullptr) {
			selection = selection & prctlFormula->check(mc);
		}

		return Result(selection, result.stateMap, result.pathResult, result.stateResult);
	}

	/*!
	 * Evaluate the action for an Csl formula.
	 *
	 * @param result The Result struct on which the action is to be evaluated.
	 * @param mc The Csl modelchecker that computed the path- or stateResult contained in the Result struct.
	 * @returns A Result struct containing the output of the action.
	 */
	virtual Result evaluate(Result const & result, storm::modelchecker::csl::AbstractModelChecker<T> const & mc) const override {

		storm::storage::BitVector selection(result.selection);

		//Compute the modelchecking results of the actions state formula and deselect all states that do not satisfy it.
		if(cslFormula.get() != nullptr) {
			selection = selection & cslFormula->check(mc);
		}

		return Result(selection, result.stateMap, result.pathResult, result.stateResult);
	}

	/*!
	 * Returns a string representation of this action.
	 *
	 * @returns A string representing this action.
	 */
	virtual std::string toString() const override {
		std::string out = "formula(";
		if(prctlFormula.get() != nullptr) {
			out += prctlFormula->toString();
		} else if(cslFormula.get() != nullptr) {
			out += cslFormula->toString();
		}
		out += ")";
		return out;
	}

private:

	// The Prctl state formula used during evaluation.
	std::shared_ptr<storm::properties::prctl::AbstractStateFormula<T>> prctlFormula;

	// The Csl state formula used during evaluation.
	std::shared_ptr<storm::properties::csl::AbstractStateFormula<T>> cslFormula;

};

} //namespace action
} //namespace properties
} //namespace storm

#endif /* STORM_FORMULA_ACTION_FORMULAACTION_H_ */
