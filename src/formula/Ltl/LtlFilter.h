/*
 * LtlFilter.h
 *
 *  Created on: May 7, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_LTL_LTLFILTER_H_
#define STORM_FORMULA_LTL_LTLFILTER_H_

#include "src/formula/AbstractFilter.h"
#include "src/modelchecker/ltl/AbstractModelChecker.h"

namespace storm {
namespace property {
namespace ltl {

template <class T>
class LtlFilter : public storm::property::AbstractFilter<T> {

public:

	LtlFilter() : child(nullptr) {
		// Intentionally left empty.
	}

	LtlFilter(AbstractLtlFormula* child) : child(child) {
		// Intentionally left empty.
	}

	LtlFilter(AbstractLtlFormula* child, action::Action<T>* action) : child(child) {
		this->actions.push_back(action);
	}

	LtlFilter(AbstractLtlFormula* child, std::vector<action::Action<T>*> actions) : AbstractFilter<T>(actions), child(child) {
		// Intentionally left empty.
	}

	virtual ~LtlFilter() {
		this->actions.clear();
		delete child;
	}


	/*!Description copied from the MC.
	 * Checks the given state formula on the model and prints the result (true/false) for all initial states, i.e.
	 * states that carry the atomic proposition "init".
	 *
	 * @param stateFormula The formula to be checked.
	 */
	void check(storm::modelchecker::ltl::AbstractModelChecker<T> const & modelchecker) const {

		// Write out the formula to be checked.
		std::cout << std::endl;
		LOG4CPLUS_INFO(logger, "Model checking formula\t" << this->toFormulaString());
		std::cout << "Model checking formula:\t" << this->toFormulaString() << std::endl;


		// Check the formula and apply the filter actions.
		storm::storage::BitVector result;

		try {
			result = evaluate(modelchecker, child);
		} catch (std::exception& e) {
			std::cout << "Error during computation: " << e.what() << "Skipping property." << std::endl;
			LOG4CPLUS_ERROR(logger, "Error during computation: " << e.what() << "Skipping property.");
			std::cout << std::endl << "-------------------------------------------" << std::endl;

			return;
		}

		// Now write out the result.

		if(this->actions.empty()) {

			// There is no filter action given. So provide legacy support:
			// Return the results for all states labeled with "init".
			LOG4CPLUS_INFO(logger, "Result for initial states:");
			std::cout << "Result for initial states:" << std::endl;
			for (auto initialState : modelchecker.getModel<storm::models::AbstractModel<T>>().getInitialStates()) {
				LOG4CPLUS_INFO(logger, "\t" << initialState << ": " << (result.get(initialState) ? "satisfied" : "not satisfied"));
				std::cout << "\t" << initialState << ": " << result.get(initialState) << std::endl;
			}
		}

		std::cout << std::endl << "-------------------------------------------" << std::endl;
	}

	bool validate() const {
		// Test whether the stored filter actions are consistent in relation to themselves and to the ingoing modelchecking result.

		//TODO: Actual validation.

		return true;
	}

	std::string toFormulaString() const {
		std::string desc = "filter(";
		return desc;
	}

	std::string toString() const {
		std::string desc = "Filter: ";
		desc += "\nActions:";
		for(auto action : this->actions) {
			desc += "\n\t" + action.toString();
		}
		desc += "\nFormula:\n\t" + child->toString();
		return desc;
	}

	void setChild(AbstractLtlFormula<T>* child) {
		this->child = child;
	}

	AbstractLtlFormula<T>* getChild() const {
		return child;
	}

private:

	storm::storage::BitVector evaluate(storm::modelchecker::ltl::AbstractModelChecker<T> const & modelchecker, AbstractLtlFormula<T>* formula) const {
		// First, get the model checking result.
		storm::storage::BitVector result = formula->check(modelchecker);

		// Now apply all filter actions and return the result.
		for(auto action : this->actions) {
			result = action->evaluate(result);
		}
		return result;
	}

	AbstractLtlFormula<T>* child;
};


} //namespace ltl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_LTL_LTLFILTER_H_ */
