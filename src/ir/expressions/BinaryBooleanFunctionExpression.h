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

	virtual std::shared_ptr<BaseExpression> clone(const std::map<std::string, std::string>& renaming, const std::map<std::string, uint_fast64_t>& bools, const std::map<std::string, uint_fast64_t>& ints) {
		return std::shared_ptr<BaseExpression>(new BinaryBooleanFunctionExpression(this->getLeft()->clone(renaming, bools, ints), this->getRight()->clone(renaming, bools, ints), this->functionType));
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
	
	virtual std::string dump(std::string prefix) const {
		std::stringstream result;
		result << prefix << "BinaryBooleanFunctionExpression" << std::endl;
		result << this->getLeft()->dump(prefix + "\t");
		switch (functionType) {
		case AND: result << prefix << "&" << std::endl; break;
		case OR: result << prefix << "|" << std::endl; break;
		}
		result << this->getRight()->dump(prefix + "\t");
		return result.str();
	}

private:
	FunctionType functionType;
};

} // namespace expressions

} // namespace ir

} // namespace storm

#endif /* STORM_IR_EXPRESSIONS_BINARYBOOLEANFUNCTIONEXPRESSION_H_ */
