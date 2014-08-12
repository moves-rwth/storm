/*
 * PrctlFilter.h
 *
 *  Created on: Apr 26, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_PRCTL_PRCTLFILTER_H_
#define STORM_FORMULA_PRCTL_PRCTLFILTER_H_

#include "src/formula/AbstractFilter.h"
#include "src/formula/prctl/AbstractPrctlFormula.h"
#include "src/formula/prctl/AbstractPathFormula.h"
#include "src/formula/prctl/AbstractStateFormula.h"
#include "src/modelchecker/prctl/AbstractModelChecker.h"
#include "src/formula/actions/AbstractAction.h"

// TODO: Test if this can be can be ommitted.
namespace storm {
namespace property {
namespace action {
 template <typename T> class AbstractAction;
}
}
}

#include <algorithm>
#include <memory>

namespace storm {
namespace property {
namespace prctl {

template <class T>
class PrctlFilter : public storm::property::AbstractFilter<T> {

	typedef typename storm::property::action::AbstractAction<T>::Result Result;

public:

	PrctlFilter() : AbstractFilter<T>(UNDEFINED), child(nullptr) {
		// Intentionally left empty.
	}

	PrctlFilter(std::shared_ptr<AbstractPrctlFormula<T>> const & child, OptimizingOperator opt = UNDEFINED) : AbstractFilter<T>(opt), child(child) {
		// Intentionally left empty.
	}

	PrctlFilter(std::shared_ptr<AbstractPrctlFormula<T>> const & child, std::shared_ptr<action::AbstractAction<T>> const & action, OptimizingOperator opt = UNDEFINED) : AbstractFilter<T>(action, opt), child(child) {
		// Intentionally left empty.
	}

	PrctlFilter(std::shared_ptr<AbstractPrctlFormula<T>> const & child, std::vector<std::shared_ptr<action::AbstractAction<T>>> const & actions, OptimizingOperator opt = UNDEFINED) : AbstractFilter<T>(actions, opt), child(child) {
		// Intentionally left empty.
	}

	virtual ~PrctlFilter() {
		// Intentionally left empty.
	}

	void check(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker) const {

		// Write out the formula to be checked.
		std::cout << std::endl;
		LOG4CPLUS_INFO(logger, "Model checking formula\t" << this->toString());
		std::cout << "Model checking formula:\t" << this->toString() << std::endl;

		writeOut(evaluate(modelchecker), modelchecker);
	}

	Result evaluate(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker) const {

		Result result;

		try {
			if(dynamic_cast<AbstractStateFormula<T> *>(child.get()) != nullptr) {
				result = evaluate(modelchecker, std::dynamic_pointer_cast<AbstractStateFormula<T>>(child));
			} else if (dynamic_cast<AbstractPathFormula<T> *>(child.get()) != nullptr) {
				result = evaluate(modelchecker, std::dynamic_pointer_cast<AbstractPathFormula<T>>(child));
			} else if (dynamic_cast<AbstractRewardPathFormula<T> *>(child.get()) != nullptr) {
				result = evaluate(modelchecker, std::dynamic_pointer_cast<AbstractRewardPathFormula<T>>(child));
			}
		} catch (std::exception& e) {
			std::cout << "Error during computation: " << e.what() << "Skipping property." << std::endl;
			LOG4CPLUS_ERROR(logger, "Error during computation: " << e.what() << "Skipping property.");
		}

		return result;
	}

	virtual std::string toString() const override {
		std::string desc = "";

		if(!std::dynamic_pointer_cast<AbstractStateFormula<T>>(child)) {

			// The formula is not a state formula so we either have a probability query or a reward query.
			if(this->actions.empty()){

				// There is exactly one action in the list, the minmax action. Again, we do legacy support-

				if(std::dynamic_pointer_cast<AbstractPathFormula<T>>(child)) {
					// It is a probability query.
					desc += "P ";

				} else {
					// It is a reward query.
					desc += "R ";
				}

				switch(this->opt) {
					case MINIMIZE:
						desc += "min ";
						break;
					case MAXIMIZE:
						desc += "max ";
						break;
					default:
						break;
				}

				desc += "= ? ";

			} else {
				desc = "filter[";

				switch(this->opt) {
					case MINIMIZE:
						desc += "min; ";
						break;
					case MAXIMIZE:
						desc += "max; ";
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
			}

		} else {

			if(this->actions.empty()) {

				// There are no filter actions but only the raw state formula. So just print that.
				return child->toString();
			}

			desc = "filter[";

			for(auto action : this->actions) {
				desc += action->toString();
				desc += "; ";
			}

			// Remove the last "; ".
			desc.pop_back();
			desc.pop_back();

			desc += "]";
		}

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

	void setChild(std::shared_ptr<AbstractPrctlFormula<T>> const & child) {
		this->child = child;
	}

	std::shared_ptr<AbstractPrctlFormula<T>> const & getChild() const {
		return child;
	}

private:

	Result evaluate(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker, std::shared_ptr<AbstractStateFormula<T>> const & formula) const {
		// First, get the model checking result.
		Result result;

		if(this->opt != UNDEFINED) {
			// If it is specified that min/max probabilities/rewards should be computed, call the appropriate method of the model checker.
			result.stateResult = modelchecker.checkOptimizingOperator(*formula, this->opt == storm::property::MINIMIZE ? true : false);
		} else {
			result.stateResult = formula->check(modelchecker);
		}

		// Now apply all filter actions and return the result.
		return evaluateActions(result, modelchecker);
	}

	Result evaluate(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker, std::shared_ptr<AbstractPathFormula<T>> const & formula) const {
		// First, get the model checking result.
		Result result;

		if(this->opt != UNDEFINED) {
			// If it is specified that min/max probabilities/rewards should be computed, call the appropriate method of the model checker.
			result.pathResult = modelchecker.checkOptimizingOperator(*formula, this->opt == storm::property::MINIMIZE ? true : false);
		} else {
			result.pathResult = formula->check(modelchecker, false);
		}

		// Now apply all filter actions and return the result.
		return evaluateActions(result, modelchecker);
	}

	Result evaluate(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker, std::shared_ptr<AbstractRewardPathFormula<T>> const & formula) const {
		// First, get the model checking result.
		Result result;

		if(this->opt != UNDEFINED) {
			// If it is specified that min/max probabilities/rewards should be computed, call the appropriate method of the model checker.
			result.pathResult = modelchecker.checkOptimizingOperator(*formula, this->opt == storm::property::MINIMIZE ? true : false);
		} else {
			result.pathResult = formula->check(modelchecker, false);
		}

		// Now apply all filter actions and return the result.
		return evaluateActions(result, modelchecker);
	}

	Result evaluateActions(Result const & input, storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker) const {

		// Init the state selection and state map vectors.
		Result result = input;
		uint_fast64_t size = result.stateResult.size() == 0 ? result.pathResult.size() : result.stateResult.size();
		result.selection = storm::storage::BitVector(size, true);
		result.stateMap = std::vector<uint_fast64_t>(size);
		for(uint_fast64_t i = 0; i < size; i++) {
			result.stateMap[i] = i;
		}

		// Now apply all filter actions and return the result.
		for(auto action : this->actions) {
			result = action->evaluate(result, modelchecker);
		}
		return result;
	}


	void writeOut(Result const & result, storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker) const {

		// Test if there is anything to write out.
		// The selection size should only be 0 if an error occurred during the evaluation (since a model with 0 states is invalid).
		if(result.selection.size() == 0) {
			std::cout << std::endl << "-------------------------------------------" << std::endl;
			return;
		}

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

			// Write out the selected state results in the order given by the stateMap.
			if(this->actions.empty()) {

				// There is no filter action given. So provide legacy support:
				// Return the results for all states labeled with "init".
				LOG4CPLUS_INFO(logger, "Result for initial states:");
				std::cout << "Result for initial states:" << std::endl;
				for (auto initialState : modelchecker.template getModel<storm::models::AbstractModel<T>>().getInitialStates()) {
					LOG4CPLUS_INFO(logger, "\t" << initialState << ": " << (result.stateResult[initialState] ? "satisfied" : "not satisfied"));
					std::cout << "\t" << initialState << ": " << result.stateResult[initialState] << std::endl;
				}
			} else {
				LOG4CPLUS_INFO(logger, "Result for " << result.selection.getNumberOfSetBits() << " selected states:");
				std::cout << "Result for " << result.selection.getNumberOfSetBits() << " selected states:" << std::endl;

				for(uint_fast64_t i = 0; i < result.stateMap.size(); i++) {
					if(result.selection.get(result.stateMap[i])) {
						LOG4CPLUS_INFO(logger, "\t" << result.stateMap[i] << ": " << (result.stateResult[result.stateMap[i]] ? "satisfied" : "not satisfied"));
						std::cout << "\t" << result.stateMap[i] << ": " << (result.stateResult[result.stateMap[i]] ? "satisfied" : "not satisfied") << std::endl;
					}
				}
			}
		}

		std::cout << std::endl << "-------------------------------------------" << std::endl;
	}

	std::shared_ptr<AbstractPrctlFormula<T>> child;
};

} //namespace prctl
} //namespace property
} //namespace storm



#endif /* STORM_FORMULA_PRCTL_PRCTLFILTER_H_ */
