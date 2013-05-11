/*
 * DoubleLiteral.h
 *
 *  Created on: 03.01.2013
 *      Author: chris
 */

#ifndef DOUBLELITERAL_H_
#define DOUBLELITERAL_H_

#include "src/ir/expressions/BaseExpression.h"

#include "boost/lexical_cast.hpp"

namespace storm {

namespace ir {

namespace expressions {

class DoubleLiteral : public BaseExpression {
public:
	double value;

	DoubleLiteral(double value) : BaseExpression(double_), value(value) {

	}

	virtual ~DoubleLiteral() {

	}

	virtual std::shared_ptr<BaseExpression> clone(const std::map<std::string, std::string>& renaming, const std::map<std::string, uint_fast64_t>& bools, const std::map<std::string, uint_fast64_t>& ints) {
		return std::shared_ptr<BaseExpression>(new DoubleLiteral(this->value));
	}

	virtual double getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
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
		result << prefix << "DoubleLiteral " << this->toString() << std::endl;
		return result.str();
	}
};

}

}

}

#endif /* DOUBLELITERAL_H_ */
