/*
 * BinaryRelationExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef BINARYRELATIONEXPRESSION_H_
#define BINARYRELATIONEXPRESSION_H_

#include "src/ir/expressions/BaseExpression.h"

namespace storm {

namespace ir {

namespace expressions {

class BinaryRelationExpression : public BaseExpression {
public:
	std::shared_ptr<BaseExpression> left;
	std::shared_ptr<BaseExpression> right;
	enum RelationType {EQUAL, LESS, LESS_OR_EQUAL, GREATER, GREATER_OR_EQUAL} relation;

	BinaryRelationExpression(std::shared_ptr<BaseExpression> left, std::shared_ptr<BaseExpression> right, RelationType relation) {
		this->left = left;
		this->right = right;
		this->relation = relation;
	}

	virtual ~BinaryRelationExpression() {

	}

	virtual std::string toString() const {
		std::string result = left->toString();
		switch (relation) {
		case EQUAL: result += " = "; break;
		case LESS: result += " < "; break;
		case LESS_OR_EQUAL: result += " <= "; break;
		case GREATER: result += " > "; break;
		case GREATER_OR_EQUAL: result += " >= "; break;
		}
		result += right->toString();

		return result;
	}

};

}

}

}

BOOST_FUSION_ADAPT_STRUCT(
    storm::ir::expressions::BinaryRelationExpression,
    (std::shared_ptr<storm::ir::expressions::BaseExpression>, left)
    (std::shared_ptr<storm::ir::expressions::BaseExpression>, right)
    (storm::ir::expressions::BinaryRelationExpression::RelationType, relation)
)

#endif /* BINARYRELATIONEXPRESSION_H_ */
