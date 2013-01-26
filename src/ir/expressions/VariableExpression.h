/*
 * VariableExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef VARIABLEEXPRESSION_H_
#define VARIABLEEXPRESSION_H_

#include "src/ir/expressions/BaseExpression.h"

#include <memory>
#include <iostream>

namespace storm {

namespace ir {

namespace expressions {

class VariableExpression : public BaseExpression {
public:
	VariableExpression(ReturnType type, uint_fast64_t index, std::string variableName,
			std::shared_ptr<BaseExpression> lowerBound = std::shared_ptr<storm::ir::expressions::BaseExpression>(nullptr),
			std::shared_ptr<BaseExpression> upperBound = std::shared_ptr<storm::ir::expressions::BaseExpression>(nullptr))
			: BaseExpression(type), index(index), variableName(variableName),
			  lowerBound(lowerBound), upperBound(upperBound) {

	}

	virtual ~VariableExpression() {

	}

	virtual std::string toString() const {
		return variableName;
	}

	virtual int_fast64_t getValueAsInt(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		if (this->getType() != int_) {
			BaseExpression::getValueAsInt(variableValues);
		}

		if (variableValues != nullptr) {
			return variableValues->second[index];
		} else {
			throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression"
					<< " involving variables without variable values.";
		}
	}

	virtual bool getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		if (this->getType() != bool_) {
			BaseExpression::getValueAsBool(variableValues);
		}

		if (variableValues != nullptr) {
			return variableValues->first[index];
		} else {
			throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression"
					<< " involving variables without variable values.";
		}
	}

	virtual double getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		if (this->getType() != double_) {
			BaseExpression::getValueAsDouble(variableValues);
		}

		throw storm::exceptions::NotImplementedException() << "Cannot evaluate expression with "
				<< " variable '" << variableName << "' of type double.";
	}

	virtual ADD* toAdd() const {
		storm::utility::CuddUtility* cuddUtility = storm::utility::cuddUtilityInstance();

		return nullptr;

		if (this->getType() == bool_) {
			ADD* result = cuddUtility->getConstant(0);
			//cuddUtility->addValueForEncodingOfConstant(result, 1, )
		} else {
			int64_t low = lowerBound->getValueAsInt(nullptr);
			int64_t high = upperBound->getValueAsInt(nullptr);
		}

		return new ADD();
	}

private:
	uint_fast64_t index;
	std::string variableName;

	std::shared_ptr<BaseExpression> lowerBound;
	std::shared_ptr<BaseExpression> upperBound;
};

}

}

}

#endif /* VARIABLEEXPRESSION_H_ */
