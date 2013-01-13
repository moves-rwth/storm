/*
 * VariableExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef VARIABLEEXPRESSION_H_
#define VARIABLEEXPRESSION_H_

#include "src/ir/expressions/BaseExpression.h"

#include <iostream>

namespace storm {

namespace ir {

namespace expressions {

class VariableExpression : public BaseExpression {
public:
	VariableExpression(ReturnType type, uint_fast64_t index, std::string variableName) : BaseExpression(type), index(index), variableName(variableName) {

	}

	virtual ~VariableExpression() {

	}

	virtual std::string toString() const {
		return variableName;
	}

	virtual int_fast64_t getValueAsInt(std::vector<bool> const& booleanVariableValues, std::vector<int_fast64_t> const& integerVariableValues) const {
		if (this->getType() != int_) {
			BaseExpression::getValueAsInt(booleanVariableValues, integerVariableValues);
		}

		return integerVariableValues[index];
	}

	virtual bool getValueAsBool(std::vector<bool> const& booleanVariableValues, std::vector<int_fast64_t> const& integerVariableValues) const {
		if (this->getType() != bool_) {
			BaseExpression::getValueAsBool(booleanVariableValues, integerVariableValues);
		}

		return booleanVariableValues[index];
	}

	virtual double getValueAsDouble(std::vector<bool> const& booleanVariableValues, std::vector<int_fast64_t> const& integerVariableValues) const {
		if (this->getType() != double_) {
			BaseExpression::getValueAsDouble(booleanVariableValues, integerVariableValues);
		}

		throw storm::exceptions::NotImplementedException() << "Cannot evaluate expression with "
				<< " variable '" << variableName << "' of type double.";
	}

private:
	uint_fast64_t index;
	std::string variableName;
};

}

}

}

#endif /* VARIABLEEXPRESSION_H_ */
