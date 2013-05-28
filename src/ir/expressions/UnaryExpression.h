/*
 * UnaryExpression.h
 *
 *  Created on: 27.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_EXPRESSIONS_UNARYEXPRESSION_H_
#define STORM_IR_EXPRESSIONS_UNARYEXPRESSION_H_

#include "BaseExpression.h"

namespace storm {

namespace ir {

namespace expressions {

class UnaryExpression : public BaseExpression {
public:
	UnaryExpression(ReturnType type, std::shared_ptr<BaseExpression> child) : BaseExpression(type), child(child) {

	}

	std::shared_ptr<BaseExpression> const& getChild() const {
		return child;
	}

private:
	std::shared_ptr<BaseExpression> child;
};

} // namespace expressions

} // namespace ir

} // namespace storm

#endif /* STORM_IR_EXPRESSIONS_UNARYEXPRESSION_H_ */
