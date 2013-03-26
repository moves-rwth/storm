/*
 * UnaryFunctionExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef UNARYFUNCTIONEXPRESSION_H_
#define UNARYFUNCTIONEXPRESSION_H_

#include "src/ir/expressions/UnaryExpression.h"

namespace storm {

namespace ir {

namespace expressions {

class UnaryNumericalFunctionExpression : public UnaryExpression {
public:
	enum FunctionType {MINUS};

	UnaryNumericalFunctionExpression(ReturnType type, std::shared_ptr<BaseExpression> child, FunctionType functionType) : UnaryExpression(type, child), functionType(functionType) {

	}

	virtual ~UnaryNumericalFunctionExpression() {

	}

	virtual std::shared_ptr<BaseExpression> clone(const std::map<std::string, std::string>& renaming, const std::map<std::string, uint_fast64_t>& bools, const std::map<std::string, uint_fast64_t>& ints) {
		return std::shared_ptr<BaseExpression>(new UnaryNumericalFunctionExpression(this->getType(), this->getChild()->clone(renaming, bools, ints), this->functionType));
	}

	virtual int_fast64_t getValueAsInt(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		if (this->getType() != int_) {
			BaseExpression::getValueAsInt(variableValues);
		}

		int_fast64_t resultChild = this->getChild()->getValueAsInt(variableValues);
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

		double resultChild = this->getChild()->getValueAsDouble(variableValues);
		switch(functionType) {
		case MINUS: return -resultChild; break;
		default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
				<< "Unknown numerical unary operator: '" << functionType << "'.";
		}
	}

	FunctionType getFunctionType() const {
		return functionType;
	}

	virtual void accept(ExpressionVisitor* visitor) {
		visitor->visit(this);
	}

	virtual std::string toString() const {
		std::string result = "";
		switch (functionType) {
		case MINUS: result += "-"; break;
		}
		result += this->getChild()->toString();

		return result;
	}

private:
	FunctionType functionType;
};

}

}

}

#endif /* UNARYFUNCTIONEXPRESSION_H_ */
