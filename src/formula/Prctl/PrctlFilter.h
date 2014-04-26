/*
 * PrctlFilter.h
 *
 *  Created on: Apr 26, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_PRCTL_PRCTLFILTER_H_
#define STORM_FORMULA_PRCTL_PRCTLFILTER_H_

#include "src/formula/AbstractFilter.h"
#include "src/formula/Prctl/AbstractPrctlFormula.h"
#include "src/formula/Prctl/AbstractPathFormula.h"
#include "src/formula/Prctl/AbstractStateFormula.h"

namespace storm {
namespace property {
namespace prctl {

template <class T>
class PrctlFilter : storm::property::AbstractFilter<T> {

public:

	PrctlFilter() : child(nullptr) {
		// Intentionally left empty.
	}

	PrctlFilter(AbstractFormula* child) : child(child) {
		// Intentionally left empty.
	}

	PrctlFilter(AbstractFormula* child, action::Action<T>* action) : child(child) {
		actions.push_back(action);
	}

	PrctlFilter(AbstractFormula* child, std::vector<action::Action<T>*> actions) : child(child), actions(actions) {
		// Intentionally left empty.
	}

	virtual ~PrctlFilter() {
		actions.clear();
		delete child;
	}

	void check(AbstractModelChecker& modelchecker) const {

		// Do a dynamic cast to test for the actual formula type and call the correct evaluation function.
		if(dynamic_cast<AbstractStateFormula<T>*>(child) != nullptr) {
			// Check the formula and apply the filter actions.
			storm::storage::BitVector result = evaluate(modelchecker, static_cast<AbstractStateFormula<T>*>(child));

			// Now write out the result.

		}
		else if (dynamic_cast<AbstractPathFormula<T>*>(child) != nullptr) {
			// Check the formula and apply the filter actions.
			std::vector<T> result = evaluate(modelchecker, static_cast<AbstractPathFormula<T>*>(child));

			// Now write out the result.
		}
		else {
			// This branch should be unreachable. If you ended up here, something strange has happened.
			//TODO: Error here.
		}
	}

	bool validate() const {
		// Test whether the stored filter actions are consistent in relation to themselves and to the ingoing modelchecking result.

		// Do a dynamic cast to test for the actual formula type.
		if(dynamic_cast<AbstractStateFormula<T>*>(child) != nullptr) {
			//TODO: Actual validation.
		}
		else if (dynamic_cast<AbstractPathFormula<T>*>(child) != nullptr) {
			//TODO: Actual validation.
		}
		else {
			// This branch should be unreachable. If you ended up here, something strange has happened.
			//TODO: Error here.
		}

		return true;
	}

	std::string toFormulaString() const {
		std::string desc = "filter(";
		return desc;
	}

	std::string toString() const {
		std::string desc = "Filter: ";
		desc += "\nActions:";
		for(auto action : actions) {
			desc += "\n\t" + action.toString();
		}
		desc += "\nFormula:\n\t" + child->toString();
		return desc;
	}

	void setChild(AbstractFormula* child) {
		this->child = child;
	}

	AbstractFormula* getChild() const {
		return child;
	}

private:

	BitVector<T> evaluate(AbstractModelChecker& modelchecker, AbstractStateFormula<T>* formula) const {
		// First, get the model checking result.
		BitVector result = formula->check(modelchecker);

		// Now apply all filter actions and return the result.
		for(auto action : actions) {
			result = action->evaluate(result);
		}
		return result;
	}

	std::vector<T> evaluate(AbstractModelChecker& modelchecker, AbstractPathFormula<T>* formula) const {
		// First, get the model checking result.
		std::vector<T> result = formula->check(modelchecker);

		// Now apply all filter actions and return the result.
		for(auto action : actions) {
			result = action->evaluate(result);
		}
		return result;
	}

	AbstractPrctlFormula* child;
};

} //namespace prctl
} //namespace property
} //namespace storm



#endif /* STORM_FORMULA_PRCTL_PRCTLFILTER_H_ */
