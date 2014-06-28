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

template <class T>
class BoundAction : public AbstractAction<T> {

	typedef typename AbstractAction<T>::Result Result;

public:

	BoundAction() : comparisonOperator(storm::property::GREATER_EQUAL), bound(0) {
		//Intentionally left empty.
	}

	BoundAction(storm::property::ComparisonType comparisonOperator, T bound) : comparisonOperator(comparisonOperator), bound(bound) {
		//Intentionally left empty.
	}

	/*!
	 * Virtual destructor
	 * To ensure that the right destructor is called
	 */
	virtual ~BoundAction() {
		//Intentionally left empty
	}

	/*!
	 *
	 */
	virtual Result evaluate(Result const & result, storm::modelchecker::prctl::AbstractModelChecker<T> const & mc) const override {
		return evaluate(result);
	}

	/*!
	 *
	 */
	virtual Result evaluate(Result const & result, storm::modelchecker::csl::AbstractModelChecker<T> const & mc) const override {
		return evaluate(result);
	}

	/*!
	 *
	 */
	virtual Result evaluate(Result const & result, storm::modelchecker::ltl::AbstractModelChecker<T> const & mc) const override {
		return evaluate(result);
	}

	/*!
	 *
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
	 *
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

	storm::property::ComparisonType comparisonOperator;
	T bound;

};


}
}
}


#endif /* STORM_FORMULA_ACTION_BOUNDACTION_H_ */
