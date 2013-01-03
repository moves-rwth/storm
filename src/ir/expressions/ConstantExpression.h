/*
 * ConstantExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef CONSTANTEXPRESSION_H_
#define CONSTANTEXPRESSION_H_

#include "src/ir/expressions/BaseExpression.h"

namespace storm {

namespace ir {

namespace expressions {

class ConstantExpression : public BaseExpression {
public:
	std::string constantName;

	ConstantExpression(std::string constantName) {
		this->constantName = constantName;
	}

	virtual ~ConstantExpression() {

	}

	virtual std::string toString() const {
		return constantName;
	}
};

}

}

}

BOOST_FUSION_ADAPT_STRUCT(
    storm::ir::expressions::ConstantExpression,
    (std::string, constantName)
)

#endif /* CONSTANTEXPRESSION_H_ */
