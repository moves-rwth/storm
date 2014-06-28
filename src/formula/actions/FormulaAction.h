/*
 * PrctlFormulaAction.h
 *
 *  Created on: Jun 6, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_ACTION_FORMULAACTION_H_
#define STORM_FORMULA_ACTION_FORMULAACTION_H_

#include "src/formula/actions/AbstractAction.h"
#include "src/formula/prctl/AbstractStateFormula.h"
#include "src/formula/csl/AbstractStateFormula.h"


#include <string>

namespace storm {
namespace property {
namespace action {

template <class T>
class FormulaAction : public AbstractAction<T> {

	typedef typename AbstractAction<T>::Result Result;

public:

	FormulaAction(storm::property::prctl::AbstractStateFormula<T>* prctlFormula) : prctlFormula(prctlFormula), cslFormula(nullptr) {
		//Intentionally left empty.
	}

	FormulaAction(storm::property::csl::AbstractStateFormula<T>* cslFormula) : prctlFormula(nullptr), cslFormula(cslFormula) {
		//Intentionally left empty.
	}

	/*!
	 * Virtual destructor
	 * To ensure that the right destructor is called
	 */
	virtual ~FormulaAction() {
		if(prctlFormula != nullptr) {
			delete prctlFormula;
		}
		if(cslFormula != nullptr) {
			delete cslFormula;
		}
	}

	/*!
	 *
	 */
	virtual Result evaluate(Result const & result, storm::modelchecker::prctl::AbstractModelChecker<T> const & mc) const override {

		storm::storage::BitVector selection(result.selection);

		//Compute the modelchecking results of the actions state formula and deselect all states that do not satisfy it.
		if(prctlFormula != nullptr) {
			selection = selection & prctlFormula->check(mc);
		}

		return Result(selection, result.stateMap, result.pathResult, result.stateResult);
	}

	/*!
	 *
	 */
	virtual Result evaluate(Result const & result, storm::modelchecker::csl::AbstractModelChecker<T> const & mc) const override {

		storm::storage::BitVector selection(result.selection);

		//Compute the modelchecking results of the actions state formula and deselect all states that do not satisfy it.
		if(cslFormula != nullptr) {
			selection = selection & cslFormula->check(mc);
		}

		return Result(selection, result.stateMap, result.pathResult, result.stateResult);
	}

	/*!
	 *
	 */
	virtual std::string toString() const override {
		std::string out = "states(";
		if(prctlFormula != nullptr) {
			out += prctlFormula->toString();
		} else if(cslFormula != nullptr) {
			out += cslFormula->toString();
		}
		out += ")";
		return out;
	}

private:
	storm::property::prctl::AbstractStateFormula<T>* prctlFormula;
	storm::property::csl::AbstractStateFormula<T>* cslFormula;

};

} //namespace action
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_ACTION_FORMULAACTION_H_ */
