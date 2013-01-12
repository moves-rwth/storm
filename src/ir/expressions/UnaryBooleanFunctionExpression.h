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
	std::shared_ptr<BaseExpression> child;
	enum FunctorType {NOT} functor;

	UnaryBooleanFunctionExpression(std::shared_ptr<BaseExpression> child, FunctorType functor) {
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

#endif /* UNARYBOOLEANFUNCTIONEXPRESSION_H_ */
