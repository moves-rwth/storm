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
	BaseExpression* child;
	enum FunctorType {NOT} functor;

	UnaryBooleanFunctionExpression(BaseExpression* child, FunctorType functor) {
		this->child = child;
		this->functor = functor;
	}

	virtual ~UnaryBooleanFunctionExpression() {

	}

	virtual std::string toString() const {
		std::string result = "";
		switch (functor) {
		case NOT: result += "!"; break;
		}
		result += child->toString();

		return result;
	}
};

}

}

}

BOOST_FUSION_ADAPT_STRUCT(
    storm::ir::expressions::UnaryBooleanFunctionExpression,
    (storm::ir::expressions::BaseExpression*, child)
    (storm::ir::expressions::UnaryBooleanFunctionExpression::FunctorType, functor)
)

#endif /* UNARYBOOLEANFUNCTIONEXPRESSION_H_ */
