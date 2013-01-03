/*
 * BooleanLiteral.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef BOOLEANLITERAL_H_
#define BOOLEANLITERAL_H_

#include "src/ir/expressions/BaseExpression.h"

namespace storm {

namespace ir {

namespace expressions {

class BooleanLiteral : public BaseExpression {
public:
	bool value;

	BooleanLiteral(bool value) {
		this->value = value;
	}

	virtual ~BooleanLiteral() {

	}

	virtual std::string toString() const {
		if (value) {
			return std::string("true");
		} else {
			return std::string("false");
		}
	}
};

}

}

}

BOOST_FUSION_ADAPT_STRUCT(
    storm::ir::expressions::BooleanLiteral,
    (bool, value)
)

#endif /* BOOLEANLITERAL_H_ */
