/*
 * IntegerLiteral.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef INTEGERLITERAL_H_
#define INTEGERLITERAL_H_

#include "src/ir/expressions/BaseExpression.h"

namespace storm {

namespace ir {

namespace expressions {

class IntegerLiteral : public BaseExpression {
public:
	int_fast64_t value;

	IntegerLiteral(int_fast64_t value) : BaseExpression(int_), value(value) {
	}

	virtual ~IntegerLiteral() {
	}

	virtual std::shared_ptr<BaseExpression> clone(const std::map<std::string, std::string>& renaming, const std::map<std::string, uint_fast64_t>& bools, const std::map<std::string, uint_fast64_t>& ints) {
		return std::shared_ptr<BaseExpression>(new IntegerLiteral(this->value));
	}

	virtual int_fast64_t getValueAsInt(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		return value;
	}

	virtual void accept(ExpressionVisitor* visitor) {
		visitor->visit(this);
	}
	
	virtual std::string toString() const {
		return boost::lexical_cast<std::string>(value);
	}
	virtual std::string dump(std::string prefix) const {
		std::stringstream result;
		result << prefix << "IntegerLiteral " << this->toString() << std::endl;
		return result.str();
	}
};

}

}

}

#endif /* INTEGERLITERAL_H_ */
