/*
 * UnaryFunctionExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef UNARYFUNCTIONEXPRESSION_H_
#define UNARYFUNCTIONEXPRESSION_H_

#include "src/ir/expressions/BaseExpression.h"

namespace storm {

namespace ir {

namespace expressions {

class UnaryNumericalFunctionExpression : public BaseExpression {
public:
	BaseExpression* child;
	enum FunctorType {MINUS} functor;

	UnaryNumericalFunctionExpression(BaseExpression* child, FunctorType functor) {
		this->child = child;
		this->functor = functor;
	}

	virtual ~UnaryNumericalFunctionExpression() {

	}

	virtual std::string toString() const {
		std::string result = "";
		switch (functor) {
		case MINUS: result += "-"; break;
		}
		result += child->toString();

		return result;
	}
};

}

}

}

BOOST_FUSION_ADAPT_STRUCT(
    storm::ir::expressions::UnaryNumericalFunctionExpression,
    (storm::ir::expressions::BaseExpression*, child)
    (storm::ir::expressions::UnaryNumericalFunctionExpression::FunctorType, functor)
)

#endif /* UNARYFUNCTIONEXPRESSION_H_ */
