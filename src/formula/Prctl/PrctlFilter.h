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
#include "src/modelchecker/prctl/AbstractModelChecker.h"
#include "src/formula/Actions/MinMaxAction.h"

namespace storm {
namespace property {
namespace prctl {

template <class T>
class PrctlFilter : public storm::property::AbstractFilter<T> {

public:

	PrctlFilter() : child(nullptr) {
		// Intentionally left empty.
	}

	PrctlFilter(AbstractPrctlFormula<T>* child) : child(child) {
		// Intentionally left empty.
	}

	PrctlFilter(AbstractPrctlFormula<T>* child, bool minimize) : child(child) {
		this->actions.push_back(new storm::property::action::MinMaxAction<T>(minimize));
	}

	PrctlFilter(AbstractPrctlFormula<T>* child, action::AbstractAction<T>* action) : child(child) {
		this->actions.push_back(action);
	}

	PrctlFilter(AbstractPrctlFormula<T>* child, std::vector<action::AbstractAction<T>*> actions) : AbstractFilter<T>(actions), child(child) {
		// Intentionally left empty.
	}

	virtual ~PrctlFilter() {
		this->actions.clear();
		delete child;
	}

	void check(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker) const {

		// Write out the formula to be checked.
		std::cout << std::endl;
		LOG4CPLUS_INFO(logger, "Model checking formula\t" << this->toFormulaString());
		std::cout << "Model checking formula:\t" << this->toFormulaString() << std::endl;

		// Do a dynamic cast to test for the actual formula type and call the correct evaluation function.
		if(dynamic_cast<AbstractStateFormula<T>*>(child) != nullptr) {

			// Check the formula and apply the filter actions.
			storm::storage::BitVector result;

			try {
				result = evaluate(modelchecker, dynamic_cast<AbstractStateFormula<T>*>(child));
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
				for (auto initialState : modelchecker.template getModel<storm::models::AbstractModel<T>>().getInitialStates()) {
					LOG4CPLUS_INFO(logger, "\t" << initialState << ": " << (result.get(initialState) ? "satisfied" : "not satisfied"));
					std::cout << "\t" << initialState << ": " << result.get(initialState) << std::endl;
				}
			}

			std::cout << std::endl << "-------------------------------------------" << std::endl;

		}
		else if (dynamic_cast<AbstractPathFormula<T>*>(child) != nullptr) {

			// Check the formula and apply the filter actions.
			std::vector<T> result;

			try {
				result = evaluate(modelchecker, dynamic_cast<AbstractPathFormula<T>*>(child));
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
				for (auto initialState : modelchecker.template getModel<storm::models::AbstractModel<T>>().getInitialStates()) {
					LOG4CPLUS_INFO(logger, "\t" << initialState << ": " << result[initialState]);
					std::cout << "\t" << initialState << ": " << result[initialState] << std::endl;
				}
			}

			std::cout << std::endl << "-------------------------------------------" << std::endl;

		}
		else if (dynamic_cast<AbstractRewardPathFormula<T>*>(child) != nullptr) {

			// Check the formula and apply the filter actions.
			std::vector<T> result;

			try {
				result = evaluate(modelchecker, dynamic_cast<AbstractRewardPathFormula<T>*>(child));
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
				for (auto initialState : modelchecker.template getModel<storm::models::AbstractModel<T>>().getInitialStates()) {
					LOG4CPLUS_INFO(logger, "\t" << initialState << ": " << result[initialState]);
					std::cout << "\t" << initialState << ": " << result[initialState] << std::endl;
				}
			}

			std::cout << std::endl << "-------------------------------------------" << std::endl;

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
		for(auto action : this->actions) {
			desc += "\n\t" + action->toString();
		}
		desc += "\nFormula:\n\t" + child->toString();
		return desc;
	}

	void setChild(AbstractPrctlFormula<T>* child) {
		this->child = child;
	}

	AbstractPrctlFormula<T>* getChild() const {
		return child;
	}

private:

	storm::storage::BitVector evaluate(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker, AbstractStateFormula<T>* formula) const {
		// First, get the model checking result.
		storm::storage::BitVector result;

		if(this->getActionCount() != 0 &&  dynamic_cast<storm::property::action::MinMaxAction<T>*>(this->getAction(0)) != nullptr) {
			// If there is an action specifying that min/max probabilities should be computed, call the appropriate method of the model checker.
			result = modelchecker.checkMinMaxOperator(*formula, static_cast<storm::property::action::MinMaxAction<T>*>(this->getAction(0))->getMinimize());
		} else {
			result = formula->check(modelchecker);
		}


		// Now apply all filter actions and return the result.
		for(auto action : this->actions) {
			result = action->evaluate(result);
		}
		return result;
	}

	std::vector<T> evaluate(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker, AbstractPathFormula<T>* formula) const {
		// First, get the model checking result.
		std::vector<T> result;

		if(this->getActionCount() != 0 &&  dynamic_cast<storm::property::action::MinMaxAction<T>*>(this->getAction(0)) != nullptr) {
			// If there is an action specifying that min/max probabilities should be computed, call the appropriate method of the model checker.
			result = modelchecker.checkMinMaxOperator(*formula, dynamic_cast<storm::property::action::MinMaxAction<T>*>(this->getAction(0))->getMinimize());
		} else {
			result = formula->check(modelchecker, false);
		}

		// Now apply all filter actions and return the result.
		for(auto action : this->actions) {
			result = action->evaluate(result);
		}
		return result;
	}

	std::vector<T> evaluate(storm::modelchecker::prctl::AbstractModelChecker<T> const & modelchecker, AbstractRewardPathFormula<T>* formula) const {
		// First, get the model checking result.
		std::vector<T> result;

		if(this->getActionCount() != 0 &&  dynamic_cast<storm::property::action::MinMaxAction<T>*>(this->getAction(0)) != nullptr) {
			// If there is an action specifying that min/max probabilities should be computed, call the appropriate method of the model checker.
			result = modelchecker.checkMinMaxOperator(*formula, dynamic_cast<storm::property::action::MinMaxAction<T>*>(this->getAction(0))->getMinimize());
		} else {
			result = formula->check(modelchecker, false);
		}

		// Now apply all filter actions and return the result.
		for(auto action : this->actions) {
			result = action->evaluate(result);
		}
		return result;
	}

	AbstractPrctlFormula<T>* child;
};

} //namespace prctl
} //namespace property
} //namespace storm



#endif /* STORM_FORMULA_PRCTL_PRCTLFILTER_H_ */
