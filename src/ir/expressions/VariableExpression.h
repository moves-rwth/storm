/*
 * VariableExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef VARIABLEEXPRESSION_H_
#define VARIABLEEXPRESSION_H_

#include "src/ir/expressions/BaseExpression.h"

#include <memory>
#include <iostream>

namespace storm {

namespace ir {

namespace expressions {

class VariableExpression : public BaseExpression {
public:
	VariableExpression(ReturnType type, uint_fast64_t index, std::string variableName,
			std::shared_ptr<BaseExpression> lowerBound = std::shared_ptr<storm::ir::expressions::BaseExpression>(nullptr),
			std::shared_ptr<BaseExpression> upperBound = std::shared_ptr<storm::ir::expressions::BaseExpression>(nullptr))
			: BaseExpression(type), index(index), variableName(variableName),
			  lowerBound(lowerBound), upperBound(upperBound) {

	}

	virtual ~VariableExpression() {

	}

	virtual std::shared_ptr<BaseExpression> clone(const std::map<std::string, std::string>& renaming, const std::map<std::string, uint_fast64_t>& bools, const std::map<std::string, uint_fast64_t>& ints) {
		if (renaming.count(this->variableName) > 0) {
			std::string newName = renaming.at(this->variableName);
			if (this->getType() == bool_) {
				return std::shared_ptr<BaseExpression>(new VariableExpression(bool_, bools.at(newName), newName, this->lowerBound, this->upperBound));
			} else if (this->getType() == int_) {
				return std::shared_ptr<BaseExpression>(new VariableExpression(int_, ints.at(newName), newName, this->lowerBound, this->upperBound));
			} else {
				std::cerr << "ERROR: Renaming variable " << this->variableName << " that is neither bool nor int." << std::endl;
				return std::shared_ptr<BaseExpression>(this);
			}
		} else {
			return std::shared_ptr<BaseExpression>(this);
		}
	}


	virtual void accept(ExpressionVisitor* visitor) {
		visitor->visit(this);
	}

	virtual std::string toString() const {
		return variableName;
	}

	virtual int_fast64_t getValueAsInt(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		if (this->getType() != int_) {
			BaseExpression::getValueAsInt(variableValues);
		}

		if (variableValues != nullptr) {
			return variableValues->second[index];
		} else {
			throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression"
					<< " involving variables without variable values.";
		}
	}

	virtual bool getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		if (this->getType() != bool_) {
			BaseExpression::getValueAsBool(variableValues);
		}

		if (variableValues != nullptr) {
			return variableValues->first[index];
		} else {
			throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression"
					<< " involving variables without variable values.";
		}
	}

	virtual double getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		if (this->getType() != double_) {
			BaseExpression::getValueAsDouble(variableValues);
		}

		throw storm::exceptions::NotImplementedException() << "Cannot evaluate expression with "
				<< " variable '" << variableName << "' of type double.";
	}

	std::string const& getVariableName() const {
		return variableName;
	}

	std::shared_ptr<BaseExpression> const& getLowerBound() const {
		return lowerBound;
	}

	std::shared_ptr<BaseExpression> const& getUpperBound() const {
		return upperBound;
	}

private:
	uint_fast64_t index;
	std::string variableName;

	std::shared_ptr<BaseExpression> lowerBound;
	std::shared_ptr<BaseExpression> upperBound;
};

}

}

}

#endif /* VARIABLEEXPRESSION_H_ */
