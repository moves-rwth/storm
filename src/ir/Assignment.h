/*
 * Assignment.h
 *
 *  Created on: 06.01.2013
 *      Author: chris
 */

#ifndef ASSIGNMENT_H_
#define ASSIGNMENT_H_

#include "expressions/BaseExpression.h"

namespace storm {

namespace ir {

class Assignment {
public:

	Assignment() : variableName(""), expression(nullptr) {

	}

	Assignment(std::string variableName, std::shared_ptr<storm::ir::expressions::BaseExpression> expression)
		: variableName(variableName), expression(expression) {

	}

	std::string getVariableName() {
		return variableName;
	}

	std::shared_ptr<storm::ir::expressions::BaseExpression> getExpression() {
		return expression;
	}

	std::string toString() {
		return "(" + variableName + "' = " + expression->toString() + ")";
	}

private:
	std::string variableName;
	std::shared_ptr<storm::ir::expressions::BaseExpression> expression;
};

}

}

#endif /* ASSIGNMENT_H_ */
