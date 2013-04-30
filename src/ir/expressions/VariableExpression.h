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

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

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
		std::shared_ptr<BaseExpression> lower = this->lowerBound, upper = this->upperBound;
		if (lower != nullptr) lower = lower->clone(renaming, bools, ints);
		if (upper != nullptr) upper = upper->clone(renaming, bools, ints);
		if (renaming.count(this->variableName) > 0) {
			std::string newName = renaming.at(this->variableName);
			if (this->getType() == bool_) {
				return std::shared_ptr<BaseExpression>(new VariableExpression(bool_, bools.at(newName), newName, lower, upper));
			} else if (this->getType() == int_) {
				return std::shared_ptr<BaseExpression>(new VariableExpression(int_, ints.at(newName), newName, lower, upper));
			} else {
				LOG4CPLUS_ERROR(logger, "ERROR: Renaming variable " << this->variableName << " that is neither bool nor int.");
				return std::shared_ptr<BaseExpression>(nullptr);
			}
		} else {
			return std::shared_ptr<BaseExpression>(new VariableExpression(this->getType(), this->index, this->variableName, lower, upper));
		}
	}


	virtual void accept(ExpressionVisitor* visitor) {
		std::cout << "Visitor!" << std::endl;
		visitor->visit(this);
	}

	virtual std::string toString() const {
		return this->variableName;
	}
	
	virtual std::string dump(std::string prefix) const {
		std::stringstream result;
		result << prefix << this->variableName << " " << index << std::endl;
		if (this->lowerBound != nullptr) {
			result << prefix << "lower bound" << std::endl;
			result << this->lowerBound->dump(prefix + "\t");
		}
		if (this->upperBound != nullptr) {
			result << prefix << "upper bound" << std::endl;
			result << this->upperBound->dump(prefix + "\t");
		}
		return result.str();
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

	uint_fast64_t getVariableIndex() const {
		return this->index;
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
