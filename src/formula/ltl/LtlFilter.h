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
#include "src/formula/ltl/AbstractLtlFormula.h"
#include "src/formula/actions/AbstractAction.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
namespace property {
namespace action {
 template <typename T> class AbstractAction;
}
}
}

namespace storm {
namespace property {
namespace ltl {

template <class T>
class LtlFilter : public storm::property::AbstractFilter<T> {

	typedef typename storm::property::action::AbstractAction<T>::Result Result;

public:

	LtlFilter() : AbstractFilter<T>(UNDEFINED), child(nullptr) {
		// Intentionally left empty.
	}

	LtlFilter(std::shared_ptr<AbstractLtlFormula<T>> const & child, OptimizingOperator opt = UNDEFINED) : AbstractFilter<T>(opt), child(child) {
		// Intentionally left empty.
	}

	LtlFilter(std::shared_ptr<AbstractLtlFormula<T>> const & child, std::shared_ptr<action::AbstractAction<T>> const & action, OptimizingOperator opt = UNDEFINED) : AbstractFilter<T>(action, opt), child(child) {
		// Intentionally left empty.
	}

	LtlFilter(std::shared_ptr<AbstractLtlFormula<T>> const & child, std::vector<std::shared_ptr<action::AbstractAction<T>>> const & actions, OptimizingOperator opt = UNDEFINED) : AbstractFilter<T>(actions, opt), child(child) {
		// Intentionally left empty.
	}

	virtual ~LtlFilter() {
		// Intentionally left empty.
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
		LOG4CPLUS_INFO(logger, "Model checking formula\t" << this->toString());
		std::cout << "Model checking formula:\t" << this->toString() << std::endl;

		Result result;

		try {
			result = evaluate(modelchecker);
		} catch (std::exception& e) {
			std::cout << "Error during computation: " << e.what() << "Skipping property." << std::endl;
			LOG4CPLUS_ERROR(logger, "Error during computation: " << e.what() << "Skipping property.");
			std::cout << std::endl << "-------------------------------------------" << std::endl;

			return;
		}

		writeOut(result, modelchecker);

	}

	Result evaluate(storm::modelchecker::ltl::AbstractModelChecker<T> const & modelchecker) const {
		// First, get the model checking result.
		Result result;

		if(this->opt != UNDEFINED) {
			// If it is specified that min/max probabilities/rewards should be computed, call the appropriate method of the model checker.
			LOG4CPLUS_ERROR(logger, "Calculation of minimizing and maximizing schedulers for LTL-formula model checking is not yet implemented.");
			throw storm::exceptions::NotImplementedException() << "Calculation of minimizing and maximizing schedulers for LTL-formula model checking is not yet implemented.";
		} else {
			result.pathResult = child->check(modelchecker);
		}

		// Now apply all filter actions and return the result.

		// Init the state selection and state map vectors.
		result.selection = storm::storage::BitVector(result.stateResult.size(), true);
		result.stateMap = std::vector<uint_fast64_t>(result.selection.size());
		for(uint_fast64_t i = 0; i < result.selection.size(); i++) {
			result.stateMap[i] = i;
		}

		// Now apply all filter actions and return the result.
		for(auto action : this->actions) {
			result = action->evaluate(result, modelchecker);
		}

		return result;
	}

	std::string toString() const override {
		std::string desc = "";

		if(this->actions.empty()){
			// There are no filter actions but only the raw state formula. So just print that.
			return child->toString();
		}

		desc = "filter[";

		switch(this->opt) {
			case MINIMIZE:
				desc += " min, ";
				break;
			case MAXIMIZE:
				desc += " max, ";
				break;
			default:
				break;
		}

		for(auto action : this->actions) {
			desc += action->toString();
			desc += "; ";
		}

		// Remove the last "; ".
		desc.pop_back();
		desc.pop_back();

		desc += "]";

		desc += "(";
		desc += child->toString();
		desc += ")";

		return desc;
	}

	virtual std::string toPrettyString() const override {
		std::string desc = "Filter: ";
		desc += "\nActions:";
		for(auto action : this->actions) {
			desc += "\n\t" + action->toString();
		}
		desc += "\nFormula:\n\t" + child->toString();
		return desc;
	}

	void setChild(std::shared_ptr<AbstractLtlFormula<T>> const & child) {
		this->child = child;
	}

	std::shared_ptr<AbstractLtlFormula<T>> const & getChild() const {
		return child;
	}

private:

	void writeOut(Result const & result, storm::modelchecker::ltl::AbstractModelChecker<T> const & modelchecker) const {

		// Test for the kind of result. Values or states.
		if(!result.pathResult.empty()) {

			// Write out the selected value results in the order given by the stateMap.
			if(this->actions.empty()) {

				// There is no filter action given. So provide legacy support:
				// Return the results for all states labeled with "init".
				LOG4CPLUS_INFO(logger, "Result for initial states:");
				std::cout << "Result for initial states:" << std::endl;
				for (auto initialState : modelchecker.template getModel<storm::models::AbstractModel<T>>().getInitialStates()) {
					LOG4CPLUS_INFO(logger, "\t" << initialState << ": " << result.pathResult[initialState]);
					std::cout << "\t" << initialState << ": " << result.pathResult[initialState] << std::endl;
				}
			} else {
				LOG4CPLUS_INFO(logger, "Result for " << result.selection.getNumberOfSetBits() << " selected states:");
				std::cout << "Result for " << result.selection.getNumberOfSetBits() << " selected states:" << std::endl;

				for(uint_fast64_t i = 0; i < result.stateMap.size(); i++) {
					if(result.selection.get(result.stateMap[i])) {
						LOG4CPLUS_INFO(logger, "\t" << result.stateMap[i] << ": " << result.pathResult[result.stateMap[i]]);
						std::cout << "\t" << result.stateMap[i] << ": " << result.pathResult[result.stateMap[i]] << std::endl;
					}
				}
			}

		} else {
			LOG4CPLUS_WARN(logger, "No results could be computed.");
			std::cout << "No results could be computed." << std::endl;
		}

		std::cout << std::endl << "-------------------------------------------" << std::endl;
	}

	std::shared_ptr<AbstractLtlFormula<T>> child;
};


} //namespace ltl
} //namespace property
} //namespace storm

#endif /* STORM_FORMULA_LTL_LTLFILTER_H_ */
