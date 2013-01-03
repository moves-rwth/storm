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

class BinaryNumericalFunctionExpression : public BaseExpression {
public:
	BaseExpression* left;
	BaseExpression* right;
	enum FunctorType {PLUS, MINUS, TIMES, DIVIDE} functor;

	BinaryNumericalFunctionExpression(BaseExpression* left, BaseExpression* right, FunctorType functor) {
		this->left = left;
		this->right = right;
		this->functor = functor;
	}

	virtual ~BinaryNumericalFunctionExpression() {

	}

	virtual std::string toString() const {
		std::string result = left->toString();
		switch (functor) {
		case PLUS: result += " + "; break;
		case MINUS: result += " - "; break;
		case TIMES: result += " * "; break;
		case DIVIDE: result += " / "; break;
		}
		result += right->toString();

		return result;
	}

};

}

}

}

BOOST_FUSION_ADAPT_STRUCT(
    storm::ir::expressions::BinaryNumericalFunctionExpression,
    (storm::ir::expressions::BaseExpression*, left)
    (storm::ir::expressions::BaseExpression*, right)
    (storm::ir::expressions::BinaryNumericalFunctionExpression::FunctorType, functor)
)

#endif /* BINARYFUNCTIONEXPRESSION_H_ */
