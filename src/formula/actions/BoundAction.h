/*
 * BoundAction.h
 *
 *  Created on: Jun 22, 2014
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_FORMULA_ACTION_BOUNDACTION_H_
#define STORM_FORMULA_ACTION_BOUNDACTION_H_

#include "src/formula/actions/AbstractAction.h"
#include "src/formula/ComparisonType.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
namespace property {
namespace action {

/*!
 * The bound action deselects all states whose modelchecking values do not satisfy the given bound.
 *
 * Strict and non strict upper and lower bounds can be represented.
 * For modelchecking results of state formulas the value is taken to be 1 iff true and 0 otherwise.
 */
template <class T>
class BoundAction : public AbstractAction<T> {

	// Convenience typedef to make the code more readable.
	typedef typename AbstractAction<T>::Result Result;

public:

	/*!
	 * Constructs an empty BoundAction.
	 * The bound is set to >= 0. Thus, all states will be selected by the action.
	 */
	BoundAction() : comparisonOperator(storm::property::GREATER_EQUAL), bound(0) {
		//Intentionally left empty.
	}

	/*!
	 * Constructs a BoundAction using the given values for the comparison operator and the bound.
	 *
	 * @param comparisonOperator The operator used to make the comparison between the bound and the modelchecking values for each state.
	 * @param bound The bound to compare the modelchecking values against.
	 */
	BoundAction(storm::property::ComparisonType comparisonOperator, T bound) : comparisonOperator(comparisonOperator), bound(bound) {
		//Intentionally left empty.
	}

	/*!
	 * The virtual destructor.
	 * To ensure that the right destructor is called.
	 */
	virtual ~BoundAction() {
		//Intentionally left empty
	}

	/*!
	 * Evaluate the action for an Prctl formula.
	 *
	 * @param result The Result struct on which the action is to be evaluated.
	 * @param mc The Prctl modelchecker that computed the path- or stateResult contained in the Result struct.
	 * @returns A Result struct containing the output of the action.
	 */
	virtual Result evaluate(Result const & result, storm::modelchecker::prctl::AbstractModelChecker<T> const & mc) const override {
		return evaluate(result);
	}

	/*!
	 * Evaluate the action for an Csl formula.
	 *
	 * @param result The Result struct on which the action is to be evaluated.
	 * @param mc The Csl modelchecker that computed the path- or stateResult contained in the Result struct.
	 * @returns A Result struct containing the output of the action.
	 */
	virtual Result evaluate(Result const & result, storm::modelchecker::csl::AbstractModelChecker<T> const & mc) const override {
		return evaluate(result);
	}

	/*!
	 * Evaluate the action for an Ltl formula.
	 *
	 * @param result The Result struct on which the action is to be evaluated.
	 * @param mc The Ltl modelchecker that computed the path- or stateResult contained in the Result struct.
	 * @returns A Result struct containing the output of the action.
	 */
	virtual Result evaluate(Result const & result, storm::modelchecker::ltl::AbstractModelChecker<T> const & mc) const override {
		return evaluate(result);
	}

	/*!
	 * Returns a string representation of this action.
	 *
	 * @returns A string representing this action.
	 */
	virtual std::string toString() const override {
		std::string out = "bound(";
		switch (comparisonOperator) {
			case LESS:
				out += "<";
				break;
			case LESS_EQUAL:
				out += "<=";
				break;
			case GREATER:
				out += ">";
				break;
			case GREATER_EQUAL:
				out += ">=";
				break;
			default:
				LOG4CPLUS_ERROR(logger, "Unknown comparison operator of value " << comparisonOperator << ".");
				throw storm::exceptions::InvalidArgumentException() << "Unknown comparison operator of value " << comparisonOperator << ".";
				break;
		}
		out += ", ";
		out += std::to_string(bound);
		out += ")";
		return out;
	}

private:

	/*!
	 * Evaluate the action.
	 *
	 * As the BoundAction does not depend on the model or the formula for which the modelchecking result was computed,
	 * it does not depend on the modelchecker at all. This internal version of the evaluate method therefore only needs the
	 * modelchecking result as input.
	 *
	 * @param result The Result struct on which the action is to be evaluated.
	 * @returns A Result struct containing the output of the action.
	 */
	virtual Result evaluate(Result const & result) const {

		//Initialize the new selection vector.
		storm::storage::BitVector out(result.selection.size());

		if(result.pathResult.size() != 0) {

			if(result.stateResult.size() != 0) {
				LOG4CPLUS_WARN(logger, "Both pathResult and stateResult are set. The filter action is applied using only the pathResult.");
				std::cout << "Both pathResult and stateResult are set. The filter action is applied using only the pathResult." << std::endl;
			}

			//Fill the selection by comparing the values for all previously selected states with the given bound using the comparison operator.
			for(uint_fast64_t i = 0; i < result.pathResult.size(); i++) {
				if(result.selection[i]) {
					switch(comparisonOperator) {
						case storm::property::GREATER_EQUAL:
							out.set(i, result.pathResult[i] >= bound);
							break;
						case storm::property::GREATER:
							out.set(i, result.pathResult[i] > bound);
							break;
						case storm::property::LESS_EQUAL:
							out.set(i, result.pathResult[i] <= bound);
							break;
						case storm::property::LESS:
							out.set(i, result.pathResult[i] < bound);
							break;
						default:
							LOG4CPLUS_ERROR(logger, "Unknown comparison operator of value " << comparisonOperator << ".");
							throw storm::exceptions::InvalidArgumentException() << "Unknown comparison operator of value " << comparisonOperator << ".";
							break;
					}
				}
			}
		} else {

			//Fill the selction by comapring the values for all previously selected states with theegiven bound using the comparison operator.
			for(uint_fast64_t i = 0; i < result.stateMap.size(); i++) {
				if(result.selection[i]) {
					switch(comparisonOperator) {
						case storm::property::GREATER_EQUAL:
							out.set(i, result.stateResult[i] >= bound);
							break;
						case storm::property::GREATER:
							out.set(i, result.stateResult[i] > bound);
							break;
						case storm::property::LESS_EQUAL:
							out.set(i, result.stateResult[i] <= bound);
							break;
						case storm::property::LESS:
							out.set(i, result.stateResult[i] < bound);
							break;
						default:
							LOG4CPLUS_ERROR(logger, "Unknown comparison operator of value " << comparisonOperator << ".");
							throw storm::exceptions::InvalidArgumentException() << "Unknown comparison operator of value " << comparisonOperator << ".";
							break;
					}
				}
			}
		}

		return Result(out, result.stateMap, result.pathResult, result.stateResult);
	}

	// The operator used to make the comparison between the bound and the modelchecking values for each state at time of evaluation.
	storm::property::ComparisonType comparisonOperator;

	// The bound to compare the modelchecking values against during evaluation.
	T bound;

};


}
}
}


#endif /* STORM_FORMULA_ACTION_BOUNDACTION_H_ */
