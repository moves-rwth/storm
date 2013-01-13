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
	enum RelationType {EQUAL, LESS, LESS_OR_EQUAL, GREATER, GREATER_OR_EQUAL};

	BinaryRelationExpression(std::shared_ptr<BaseExpression> left, std::shared_ptr<BaseExpression> right, RelationType relationType) : BaseExpression(bool_), left(left), right(right), relationType(relationType) {

	}

	virtual ~BinaryRelationExpression() {

	}

	virtual bool getValueAsBool(std::vector<bool> const& booleanVariableValues, std::vector<int_fast64_t> const& integerVariableValues) const {
		int_fast64_t resultLeft = left->getValueAsInt(booleanVariableValues, integerVariableValues);
		int_fast64_t resultRight = right->getValueAsInt(booleanVariableValues, integerVariableValues);
		switch(relationType) {
		case EQUAL: return resultLeft == resultRight; break;
		case LESS: return resultLeft < resultRight; break;
		case LESS_OR_EQUAL: return resultLeft <= resultRight; break;
		case GREATER: return resultLeft > resultRight; break;
		case GREATER_OR_EQUAL: return resultLeft >= resultRight; break;
		default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
				<< "Unknown boolean binary relation: '" << relationType << "'.";
		}
	}

	virtual std::string toString() const {
		std::string result = left->toString();
		switch (relationType) {
		case EQUAL: result += " = "; break;
		case LESS: result += " < "; break;
		case LESS_OR_EQUAL: result += " <= "; break;
		case GREATER: result += " > "; break;
		case GREATER_OR_EQUAL: result += " >= "; break;
		}
		result += right->toString();

		return result;
	}

private:
	std::shared_ptr<BaseExpression> left;
	std::shared_ptr<BaseExpression> right;
	RelationType relationType;
};

}

}

}

#endif /* BINARYRELATIONEXPRESSION_H_ */
