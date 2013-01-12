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
	std::shared_ptr<BaseExpression> left;
	std::shared_ptr<BaseExpression> right;
	enum FunctorType {PLUS, MINUS, TIMES, DIVIDE} functor;

	BinaryNumericalFunctionExpression(std::shared_ptr<BaseExpression> left, std::shared_ptr<BaseExpression> right, FunctorType functor) {
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

#endif /* BINARYFUNCTIONEXPRESSION_H_ */
