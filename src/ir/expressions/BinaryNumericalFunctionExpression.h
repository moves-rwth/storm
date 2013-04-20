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

class BinaryNumericalFunctionExpression : public BinaryExpression {
public:
	enum FunctionType {PLUS, MINUS, TIMES, DIVIDE};

	BinaryNumericalFunctionExpression(ReturnType type, std::shared_ptr<BaseExpression> left, std::shared_ptr<BaseExpression> right, FunctionType functionType) : BinaryExpression(type, left, right), functionType(functionType) {

	}

	virtual std::shared_ptr<BaseExpression> clone(const std::map<std::string, std::string>& renaming, const std::map<std::string, uint_fast64_t>& bools, const std::map<std::string, uint_fast64_t>& ints) {
		return std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(this->getType(), this->getLeft()->clone(renaming, bools, ints), this->getRight()->clone(renaming, bools, ints), this->functionType));
	}

	virtual ~BinaryNumericalFunctionExpression() {

	}

	FunctionType getFunctionType() const {
		return functionType;
	}

	virtual int_fast64_t getValueAsInt(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		if (this->getType() != int_) {
			BaseExpression::getValueAsInt(variableValues);
		}

		int_fast64_t resultLeft = this->getLeft()->getValueAsInt(variableValues);
		int_fast64_t resultRight = this->getRight()->getValueAsInt(variableValues);
		switch(functionType) {
		case PLUS: return resultLeft + resultRight; break;
		case MINUS: return resultLeft - resultRight; break;
		case TIMES: return resultLeft * resultRight; break;
		case DIVIDE: return resultLeft / resultRight; break;
		default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
				<< "Unknown numeric binary operator: '" << functionType << "'.";
		}
	}

	virtual double getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		if (this->getType() != double_) {
			BaseExpression::getValueAsDouble(variableValues);
		}

		double resultLeft = this->getLeft()->getValueAsDouble(variableValues);
		double resultRight = this->getRight()->getValueAsDouble(variableValues);
		switch(functionType) {
		case PLUS: return resultLeft + resultRight; break;
		case MINUS: return resultLeft - resultRight; break;
		case TIMES: return resultLeft * resultRight; break;
		case DIVIDE: return resultLeft / resultRight; break;
		default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
				<< "Unknown numeric binary operator: '" << functionType << "'.";
		}
	}

	virtual void accept(ExpressionVisitor* visitor) {
		visitor->visit(this);
	}

	virtual std::string toString() const {
		std::string result = this->getLeft()->toString();
		switch (functionType) {
		case PLUS: result += " + "; break;
		case MINUS: result += " - "; break;
		case TIMES: result += " * "; break;
		case DIVIDE: result += " / "; break;
		}
		result += this->getRight()->toString();

		return result;
	}
	
	virtual std::string dump(std::string prefix) const {
		std::stringstream result;
		result << prefix << "BinaryNumericalFunctionExpression" << std::endl;
		result << this->getLeft()->dump(prefix + "\t");
		switch (functionType) {
		case PLUS: result << prefix << "+" << std::endl; break;
		case MINUS: result << prefix << "-" << std::endl; break;
		case TIMES: result << prefix << "*" << std::endl; break;
		case DIVIDE: result << prefix << "/" << std::endl; break;
		}
		result << this->getRight()->dump(prefix + "\t");
		return result.str();
	}
private:
	FunctionType functionType;
};

}

}

}

#endif /* BINARYFUNCTIONEXPRESSION_H_ */
