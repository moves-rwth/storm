/*
 * UnaryBooleanFunctionExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef UNARYBOOLEANFUNCTIONEXPRESSION_H_
#define UNARYBOOLEANFUNCTIONEXPRESSION_H_

#include "src/ir/expressions/UnaryExpression.h"

namespace storm {

namespace ir {

namespace expressions {

class UnaryBooleanFunctionExpression : public UnaryExpression {
public:
	enum FunctionType {NOT};

	UnaryBooleanFunctionExpression(std::shared_ptr<BaseExpression> child, FunctionType functionType) : UnaryExpression(bool_, child), functionType(functionType) {

	}

	virtual ~UnaryBooleanFunctionExpression() {

	}

	FunctionType getFunctionType() const {
		return functionType;
	}

	virtual bool getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		bool resultChild = this->getChild()->getValueAsBool(variableValues);
		switch(functionType) {
		case NOT: return !resultChild; break;
		default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
				<< "Unknown boolean unary operator: '" << functionType << "'.";
		}
	}

	virtual void accept(ExpressionVisitor* visitor) {
		visitor->visit(this);
	}

	virtual std::string toString() const {
		std::string result = "";
		switch (functionType) {
		case NOT: result += "!"; break;
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

#endif /* UNARYBOOLEANFUNCTIONEXPRESSION_H_ */
