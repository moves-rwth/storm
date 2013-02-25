/*
 * BinaryBooleanFunctionExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef STORM_IR_EXPRESSIONS_BINARYBOOLEANFUNCTIONEXPRESSION_H_
#define STORM_IR_EXPRESSIONS_BINARYBOOLEANFUNCTIONEXPRESSION_H_

#include "src/ir/expressions/BinaryExpression.h"

#include <memory>
#include <sstream>

namespace storm {

namespace ir {

namespace expressions {

class BinaryBooleanFunctionExpression : public BinaryExpression {
public:
	enum FunctionType {AND, OR};

	BinaryBooleanFunctionExpression(std::shared_ptr<BaseExpression> left, std::shared_ptr<BaseExpression> right, FunctionType functionType) : BinaryExpression(bool_, left, right), functionType(functionType) {

	}

	virtual ~BinaryBooleanFunctionExpression() {

	}

	virtual bool getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		bool resultLeft = this->getLeft()->getValueAsBool(variableValues);
		bool resultRight = this->getRight()->getValueAsBool(variableValues);
		switch(functionType) {
		case AND: return resultLeft & resultRight; break;
		case OR: return resultLeft | resultRight; break;
		default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
				<< "Unknown boolean binary operator: '" << functionType << "'.";
		}
	}

	FunctionType getFunctionType() const {
		return functionType;
	}

	virtual void accept(ExpressionVisitor* visitor) {
		visitor->visit(this);
	}

	virtual std::string toString() const {
		std::stringstream result;
		result << this->getLeft()->toString();
		switch (functionType) {
		case AND: result << " & "; break;
		case OR: result << " | "; break;
		}
		result << this->getRight()->toString();

		return result.str();
	}

private:
	FunctionType functionType;
};

} // namespace expressions

} // namespace ir

} // namespace storm

#endif /* STORM_IR_EXPRESSIONS_BINARYBOOLEANFUNCTIONEXPRESSION_H_ */
