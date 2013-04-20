/*
 * DoubleConstantExpression.h
 *
 *  Created on: 04.01.2013
 *      Author: chris
 */

#ifndef DOUBLECONSTANTEXPRESSION_H_
#define DOUBLECONSTANTEXPRESSION_H_

#include "ConstantExpression.h"

namespace storm {

namespace ir {

namespace expressions {

class DoubleConstantExpression : public ConstantExpression {
public:
	DoubleConstantExpression(std::string constantName) : ConstantExpression(double_, constantName), defined(false), value(0) {

	}

	virtual ~DoubleConstantExpression() {

	}

	virtual std::shared_ptr<BaseExpression> clone(const std::map<std::string, std::string>& renaming, const std::map<std::string, uint_fast64_t>& bools, const std::map<std::string, uint_fast64_t>& ints) {
		return std::shared_ptr<BaseExpression>(this);
	}

	virtual double getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		if (!defined) {
			throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
					<< "Double constant '" << this->getConstantName() << "' is undefined.";
		} else {
			return value;
		}
	}

	virtual void accept(ExpressionVisitor* visitor) {
		visitor->visit(this);
	}

	virtual std::string toString() const {
		std::string result = this->constantName;
		if (defined) {
			result += "[" + boost::lexical_cast<std::string>(value) + "]";
		}
		return result;
	}
	
	virtual std::string dump(std::string prefix) const {
		std::stringstream result;
		result << prefix << "DoubleConstantExpression " << this->toString() << std::endl;
		return result.str();
	}

	bool isDefined() {
		return defined;
	}

	double getValue() {
		return value;
	}

	void define(double value) {
		defined = true;
		this->value = value;
	}

private:
	bool defined;
	double value;
};

}

}

}

#endif /* DOUBLECONSTANTEXPRESSION_H_ */
