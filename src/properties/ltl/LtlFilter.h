/*
 * LtlFilter.h
 *
 *  Created on: May 7, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_LTL_LTLFILTER_H_
#define STORM_FORMULA_LTL_LTLFILTER_H_

#include "src/properties/AbstractFilter.h"
#include "src/modelchecker/ltl/AbstractModelChecker.h"
#include "src/properties/ltl/AbstractLtlFormula.h"
#include "src/properties/actions/AbstractAction.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
namespace properties {
namespace ltl {

/*!
 * This is the Ltl specific filter.
 *
 * It maintains a Ltl formula which can be checked against a given model by either calling evaluate() or check().
 * Additionally it maintains a list of filter actions that are used to further manipulate the modelchecking results and prepare them for output.
 */
template <class T>
class LtlFilter : public storm::properties::AbstractFilter<T> {

	// Convenience typedef to make the code more readable.
	typedef typename storm::properties::action::AbstractAction<T>::Result Result;

public:

	/*!
	 * Creates an empty LtlFilter, maintaining no Ltl formula.
	 *
	 * Calling check or evaluate will return an empty result.
	 */
	LtlFilter() : AbstractFilter<T>(UNDEFINED), child(nullptr) {
		// Intentionally left empty.
	}

	/*!
	 * Creates an LtlFilter maintaining an Ltl formula but containing no actions.
	 *
	 * The modelchecking result will be returned or printed as is.
	 *
	 * @param child The Ltl formula to be maintained.
	 * @param opt An enum value indicating which kind of scheduler shall be used for path formulas on nondeterministic models.
	 */
	LtlFilter(std::shared_ptr<AbstractLtlFormula<T>> const & child, OptimizingOperator opt = UNDEFINED) : AbstractFilter<T>(opt), child(child) {
		// Intentionally left empty.
	}

	/*!
	 * Creates an LtlFilter maintaining a Ltl formula and containing a single action.
	 *
	 * The given action will be applied to the modelchecking result during evaluation.
	 * Further actions can be added later.
	 *
	 * @param child The Ltl formula to be maintained.
	 * @param action The single action to be executed during evaluation.
	 * @param opt An enum value indicating which kind of scheduler shall be used for path formulas on nondeterministic models.
	 */
	LtlFilter(std::shared_ptr<AbstractLtlFormula<T>> const & child, std::shared_ptr<action::AbstractAction<T>> const & action, OptimizingOperator opt = UNDEFINED) : AbstractFilter<T>(action, opt), child(child) {
		// Intentionally left empty.
	}

	/*!
	 * Creates an LtlFilter using the given parameters.
	 *
	 * The given actions will be applied to the modelchecking result in ascending index order during evaluation.
	 * Further actions can be added later.
	 *
	 * @param child The Ltl formula to be maintained.
	 * @param actions A vector conatining the actions that are to be executed during evaluation.
	 * @param opt An enum value indicating which kind of scheduler shall be used for path formulas on nondeterministic models.
	 */
	LtlFilter(std::shared_ptr<AbstractLtlFormula<T>> const & child, std::vector<std::shared_ptr<action::AbstractAction<T>>> const & actions, OptimizingOperator opt = UNDEFINED) : AbstractFilter<T>(actions, opt), child(child) {
		// Intentionally left empty.
	}

	/*!
	 * Empty virtual destructor.
	 */
	virtual ~LtlFilter() {
		// Intentionally left empty.
	}


	/*!
	 * Calls the modelchecker, retrieves the modelchecking result, applies the filter action one by one and prints out the result.
	 *
	 * @param modelchecker The modelchecker to be called.
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

	/*!
	 * Calls the modelchecker, retrieves the modelchecking result, applies the filter action one by one and returns the result.
	 *
	 * @param modelchecker The modelchecker to be called.
	 * @returns The result of the sequential application of the filter actions to the modelchecking result.
	 */
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

	/*!
	 * Returns a textual representation of the filter.
	 *
	 * That includes the actions as well as the maintained formula.
	 *
	 * @returns A string representing the filter.
	 */
	std::string toString() const override {
		std::string desc = "";

		if(this->actions.empty()){
			// There are no filter actions but only the raw state formula. So just print that.
			return child->toString();
		}

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

		desc += "(";
		desc += child->toString();
		desc += ")";

		return desc;
	}

	/*!
	 * Gets the child node.
	 *
	 * @returns The child node.
	 */
	std::shared_ptr<AbstractLtlFormula<T>> const & getChild() const {
		return child;
	}

	/*!
	 * Sets the subtree.
	 *
	 * @param child The new child.
	 */
	void setChild(std::shared_ptr<AbstractLtlFormula<T>> const & child) {
		this->child = child;
	}

	/*!
	 * Checks if the child is set, i.e. it does not point to null.
	 *
	 * @return True iff the child is set.
	 */
	bool isChildSet() const {
		return child.get() != nullptr;
	}

private:

	/*!
	 * Writes out the given result.
	 *
	 * @param result The result of the sequential application of the filter actions to a modelchecking result.
	 * @param modelchecker The modelchecker that was called to generate the modelchecking result. Needed for legacy support.
	 */
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

	// The Ltl formula maintained by this filter.
	std::shared_ptr<AbstractLtlFormula<T>> child;
};


} //namespace ltl
} //namespace properties
} //namespace storm

#endif /* STORM_FORMULA_LTL_LTLFILTER_H_ */
