/*
 * BinaryBooleanFunctionExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef BINARYBOOLEANFUNCTIONEXPRESSION_H_
#define BINARYBOOLEANFUNCTIONEXPRESSION_H_

#include "src/ir/expressions/BaseExpression.h"
#include <boost/fusion/include/adapt_struct.hpp>

namespace storm {

namespace ir {

namespace expressions {

class BinaryBooleanFunctionExpression : public BaseExpression {
public:
	enum FunctorType {AND, OR, XOR, IMPLIES} functor;
	std::shared_ptr<storm::ir::expressions::BaseExpression> left;
	std::shared_ptr<BaseExpression> right;

	BinaryBooleanFunctionExpression(std::shared_ptr<BaseExpression> left, std::shared_ptr<BaseExpression> right, FunctorType functor) {
		this->left = left;
		this->right = right;
		this->functor = functor;
	}

	virtual ~BinaryBooleanFunctionExpression() {

	}

	virtual std::string toString() const {
		std::string result = left->toString();
		switch (functor) {
		case AND: result += " & "; break;
		case OR: result += " | "; break;
		case XOR: result += " ^ "; break;
		case IMPLIES: result += " => "; break;
		}
		result += right->toString();

		return result;
	}
};

}

}

}

#endif /* BINARYBOOLEANFUNCTIONEXPRESSION_H_ */
