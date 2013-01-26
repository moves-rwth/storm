/*
 * UnaryFunctionExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef UNARYFUNCTIONEXPRESSION_H_
#define UNARYFUNCTIONEXPRESSION_H_

#include "src/ir/expressions/BaseExpression.h"

namespace storm {

namespace ir {

namespace expressions {

class UnaryNumericalFunctionExpression : public BaseExpression {
public:
	enum FunctionType {MINUS};

	UnaryNumericalFunctionExpression(ReturnType type, std::shared_ptr<BaseExpression> child, FunctionType functionType) : BaseExpression(type), child(child), functionType(functionType) {

	}

	virtual ~UnaryNumericalFunctionExpression() {

	}

	virtual int_fast64_t getValueAsInt(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		if (this->getType() != int_) {
			BaseExpression::getValueAsInt(variableValues);
		}

		int_fast64_t resultChild = child->getValueAsInt(variableValues);
		switch(functionType) {
		case MINUS: return -resultChild; break;
		default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
				<< "Unknown numerical unary operator: '" << functionType << "'.";
		}
	}

	virtual double getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		if (this->getType() != double_) {
			BaseExpression::getValueAsDouble(variableValues);
		}

		double resultChild = child->getValueAsDouble(variableValues);
		switch(functionType) {
		case MINUS: return -resultChild; break;
		default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
				<< "Unknown numerical unary operator: '" << functionType << "'.";
		}
	}

	virtual ADD* toAdd() const {
		ADD* childResult = child->toAdd();
		storm::utility::CuddUtility* cuddUtility = storm::utility::cuddUtilityInstance();
		ADD* result = cuddUtility->getConstant(0);
		return new ADD(result->Minus(*childResult));
	}

	virtual std::string toString() const {
		std::string result = "";
		switch (functionType) {
		case MINUS: result += "-"; break;
		}
		result += child->toString();

		return result;
	}

private:
	std::shared_ptr<BaseExpression> child;
	FunctionType functionType;
};

}

}

}

#endif /* UNARYFUNCTIONEXPRESSION_H_ */
