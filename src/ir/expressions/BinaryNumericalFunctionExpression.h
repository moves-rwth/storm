/*
 * BinaryFunctionExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef BINARYFUNCTIONEXPRESSION_H_
#define BINARYFUNCTIONEXPRESSION_H_

#include "src/ir/expressions/BaseExpression.h"

namespace storm {

namespace ir {

namespace expressions {

class BinaryNumericalFunctionExpression : public BaseExpression {
public:
	enum FunctionType {PLUS, MINUS, TIMES, DIVIDE};

	BinaryNumericalFunctionExpression(ReturnType type, std::shared_ptr<BaseExpression> left, std::shared_ptr<BaseExpression> right, FunctionType functionType) : BaseExpression(type), left(left), right(right), functionType(functionType) {

	}

	virtual ~BinaryNumericalFunctionExpression() {

	}

	virtual int_fast64_t getValueAsInt(std::vector<bool> const& booleanVariableValues, std::vector<int_fast64_t> const& integerVariableValues) const {
		if (this->getType() != int_) {
			BaseExpression::getValueAsInt(booleanVariableValues, integerVariableValues);
		}

		int_fast64_t resultLeft = left->getValueAsInt(booleanVariableValues, integerVariableValues);
		int_fast64_t resultRight = right->getValueAsInt(booleanVariableValues, integerVariableValues);
		switch(functionType) {
		case PLUS: return resultLeft + resultRight; break;
		case MINUS: return resultLeft - resultRight; break;
		case TIMES: return resultLeft * resultRight; break;
		case DIVIDE: return resultLeft / resultRight; break;
		default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
				<< "Unknown numeric binary operator: '" << functionType << "'.";
		}
	}

	virtual double getValueAsDouble(std::vector<bool> const& booleanVariableValues, std::vector<int_fast64_t> const& integerVariableValues) const {
		if (this->getType() != double_) {
			BaseExpression::getValueAsDouble(booleanVariableValues, integerVariableValues);
		}

		double resultLeft = left->getValueAsDouble(booleanVariableValues, integerVariableValues);
		double resultRight = right->getValueAsDouble(booleanVariableValues, integerVariableValues);
		switch(functionType) {
		case PLUS: return resultLeft + resultRight; break;
		case MINUS: return resultLeft - resultRight; break;
		case TIMES: return resultLeft * resultRight; break;
		case DIVIDE: return resultLeft / resultRight; break;
		default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
				<< "Unknown numeric binary operator: '" << functionType << "'.";
		}
	}

	virtual std::string toString() const {
		std::string result = left->toString();
		switch (functionType) {
		case PLUS: result += " + "; break;
		case MINUS: result += " - "; break;
		case TIMES: result += " * "; break;
		case DIVIDE: result += " / "; break;
		}
		result += right->toString();

		return result;
	}
private:
	std::shared_ptr<BaseExpression> left;
	std::shared_ptr<BaseExpression> right;
	FunctionType functionType;
};

}

}

}

#endif /* BINARYFUNCTIONEXPRESSION_H_ */
