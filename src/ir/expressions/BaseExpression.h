/*
 * Expression.h
 *
 *  Created on: 03.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_EXPRESSIONS_BASEEXPRESSION_H_
#define STORM_IR_EXPRESSIONS_BASEEXPRESSION_H_

#include "src/exceptions/ExpressionEvaluationException.h"
#include "src/exceptions/NotImplementedException.h"

#include "ExpressionVisitor.h"

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace storm {
namespace ir {
namespace expressions {

class BaseExpression {

public:
	enum ReturnType {undefined, bool_, int_, double_};

	BaseExpression() : type(undefined) {

	}

	BaseExpression(ReturnType type) : type(type) {

	}

	virtual ~BaseExpression() {

	}

	virtual std::shared_ptr<BaseExpression> clone(const std::map<std::string, std::string>& renaming, const std::map<std::string, uint_fast64_t>& bools, const std::map<std::string, uint_fast64_t>& ints) = 0;

	virtual int_fast64_t getValueAsInt(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		if (type != int_) {
			throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression of type '"
					<< this->getTypeName() << "' as 'int'.";
		}
		throw storm::exceptions::NotImplementedException() << "Cannot evaluate expression of type '"
					<< this->getTypeName() << " because evaluation implementation is missing.";
	}

	virtual bool getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		if (type != bool_) {
			throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression of type '"
					<< this->getTypeName() << "' as 'bool'.";
		}
		throw storm::exceptions::NotImplementedException() << "Cannot evaluate expression of type '"
					<< this->getTypeName() << " because evaluation implementation is missing.";
	}

	virtual double getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
		if (type != bool_) {
			throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression of type '"
					<< this->getTypeName() << "' as 'double'.";
		}
		throw storm::exceptions::NotImplementedException() << "Cannot evaluate expression of type '"
					<< this->getTypeName() << " because evaluation implementation is missing.";
	}

	virtual void accept(ExpressionVisitor* visitor) {
		visitor->visit(this);
	}

	virtual std::string toString() const = 0;

	std::string getTypeName() const {
		switch(type) {
		case bool_: return std::string("bool");
		case int_: return std::string("int");
		case double_: return std::string("double");
		default: return std::string("undefined");
		}
	}

	ReturnType getType() const {
		return type;
	}

private:
	ReturnType type;
};

} // namespace expressions

} // namespace ir

} // namespace storm

#endif /* STORM_IR_EXPRESSIONS_BASEEXPRESSION_H_ */
