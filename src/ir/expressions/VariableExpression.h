/*
 * VariableExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef VARIABLEEXPRESSION_H_
#define VARIABLEEXPRESSION_H_

#include "src/ir/expressions/BaseExpression.h"

#include <iostream>

namespace storm {

namespace ir {

namespace expressions {

class VariableExpression : public BaseExpression {
public:
	std::string variableName;

	VariableExpression(std::string variableName) : variableName(variableName) {

	}

	virtual ~VariableExpression() {

	}

	virtual std::string toString() const {
		return variableName;
	}
};

}

}

}

BOOST_FUSION_ADAPT_STRUCT(
    storm::ir::expressions::VariableExpression,
    (std::string, variableName)
)

#endif /* VARIABLEEXPRESSION_H_ */
