/*
 * VariableExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef VARIABLEEXPRESSION_H_
#define VARIABLEEXPRESSION_H_

#include <memory>
#include <iostream>

#include "src/ir/VariableStateInterface.h"
#include "BaseExpression.h"
#include "src/exceptions/InvalidArgumentException.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {

namespace ir {

namespace expressions {

class VariableExpression : public BaseExpression {
public:
    VariableExpression(ReturnType type, std::string variableName) : BaseExpression(type), localIndex(0), globalIndex(0), variableName(variableName) {
        // Nothing to do here.
    }
    
	VariableExpression(ReturnType type, uint_fast64_t localIndex, uint_fast64_t globalIndex, std::string variableName)
			: BaseExpression(type), localIndex(localIndex), globalIndex(globalIndex), variableName(variableName) {
        // Nothing to do here.
	}
    
    VariableExpression(VariableExpression const& oldExpression, std::string const& newName, uint_fast64_t newGlobalIndex)
        : BaseExpression(oldExpression.getType()), localIndex(oldExpression.localIndex), globalIndex(newGlobalIndex), variableName(newName) {
        // Nothing to do here.
    }

	virtual ~VariableExpression() {
        // Nothing to do here.
	}

    virtual std::shared_ptr<BaseExpression> clone(std::map<std::string, std::string> const& renaming, std::shared_ptr<VariableStateInterface> const& variableState) {
        // Perform the proper cloning.
    }
    
	virtual std::shared_ptr<BaseExpression> clone(std::map<std::string, std::string> const& renaming, std::map<std::string, uint_fast64_t> const& booleanVariableToIndexMap, std::map<std::string, uint_fast64_t> const& integerVariableToIndexMap) {
        auto renamingPair = renaming.find(this->variableName);
        if (renamingPair != renaming.end()) {
			if (this->getType() == bool_) {
				return std::shared_ptr<BaseExpression>(new VariableExpression(bool_, this->localIndex, booleanVariableToIndexMap.at(renamingPair->second), renamingPair->second));
			} else if (this->getType() == int_) {
				return std::shared_ptr<BaseExpression>(new VariableExpression(int_, this->localIndex, integerVariableToIndexMap.at(renamingPair->second), renamingPair->second));
			} else {
				LOG4CPLUS_ERROR(logger, "Renaming variable " << this->variableName << " that is neither bool nor int.");
				throw storm::exceptions::InvalidArgumentException() << "Renaming variable " << this->variableName << " that is neither bool nor int.";
			}
		} else {
			return std::shared_ptr<BaseExpression>(new VariableExpression(this->getType(), this->localIndex, this->globalIndex, this->variableName));
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
		return result.str();
	}

	virtual int_fast64_t getValueAsInt(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		if (this->getType() != int_) {
			BaseExpression::getValueAsInt(variableValues);
		}

		if (variableValues != nullptr) {
			return variableValues->second[globalIndex];
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
			return variableValues->first[globalIndex];
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

	uint_fast64_t getLocalVariableIndex() const {
		return this->localIndex;
	}
    
    uint_fast64_t getGlobalVariableIndex() const {
		return this->globalIndex;
	}

private:
	uint_fast64_t localIndex;
    uint_fast64_t globalIndex;
	std::string variableName;
};

}

}

}

#endif /* VARIABLEEXPRESSION_H_ */
