/*
 * UnaryBooleanFunctionExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef UNARYBOOLEANFUNCTIONEXPRESSION_H_
#define UNARYBOOLEANFUNCTIONEXPRESSION_H_

#include "src/ir/expressions/BaseExpression.h"

namespace storm {

namespace ir {

namespace expressions {

class UnaryBooleanFunctionExpression : public BaseExpression {
public:
	enum FunctionType {NOT};

	UnaryBooleanFunctionExpression(std::shared_ptr<BaseExpression> child, FunctionType functionType) : BaseExpression(bool_), child(child), functionType(functionType) {

	}

	virtual ~UnaryBooleanFunctionExpression() {

	}

	virtual bool getValueAsBool(std::vector<bool> const& booleanVariableValues, std::vector<int_fast64_t> const& integerVariableValues) const {
		bool resultChild = child->getValueAsBool(booleanVariableValues, integerVariableValues);
		switch(functionType) {
		case NOT: return !resultChild; break;
		default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
				<< "Unknown boolean unary operator: '" << functionType << "'.";
		}
	}

	virtual std::string toString() const {
		std::string result = "";
		switch (functionType) {
		case NOT: result += "!"; break;
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

#endif /* UNARYBOOLEANFUNCTIONEXPRESSION_H_ */
