/*
 * BinaryBooleanFunctionExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef STORM_IR_EXPRESSIONS_BINARYBOOLEANFUNCTIONEXPRESSION_H_
#define STORM_IR_EXPRESSIONS_BINARYBOOLEANFUNCTIONEXPRESSION_H_

#include "src/ir/expressions/BaseExpression.h"

#include "src/utility/CuddUtility.h"

#include <memory>
#include <sstream>

namespace storm {

namespace ir {

namespace expressions {

class BinaryBooleanFunctionExpression : public BaseExpression {
public:
	enum FunctionType {AND, OR};

	BinaryBooleanFunctionExpression(std::shared_ptr<BaseExpression> left, std::shared_ptr<BaseExpression> right, FunctionType functionType) : BaseExpression(bool_), left(left), right(right), functionType(functionType) {

	}

	virtual ~BinaryBooleanFunctionExpression() {

	}

	virtual bool getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		bool resultLeft = left->getValueAsBool(variableValues);
		bool resultRight = right->getValueAsBool(variableValues);
		switch(functionType) {
		case AND: return resultLeft & resultRight; break;
		case OR: return resultLeft | resultRight; break;
		default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
				<< "Unknown boolean binary operator: '" << functionType << "'.";
		}
	}

	virtual ADD* toAdd() const {
		ADD* leftAdd = left->toAdd();
		ADD* rightAdd = right->toAdd();

		switch(functionType) {
		case AND: return new ADD(leftAdd->Times(*rightAdd)); break;
		case OR: return new ADD(leftAdd->Plus(*rightAdd)); break;
		default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
				<< "Unknown boolean binary operator: '" << functionType << "'.";
		}
	}

	virtual std::string toString() const {
		std::stringstream result;
		result << left->toString();
		switch (functionType) {
		case AND: result << " & "; break;
		case OR: result << " | "; break;
		}
		result << right->toString();

		return result.str();
	}

private:
	std::shared_ptr<BaseExpression> left;
	std::shared_ptr<BaseExpression> right;
	FunctionType functionType;
};

} // namespace expressions

} // namespace ir

} // namespace storm

#endif /* STORM_IR_EXPRESSIONS_BINARYBOOLEANFUNCTIONEXPRESSION_H_ */
