/*
 * Variable.h
 *
 *  Created on: 06.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_VARIABLE_H_
#define STORM_IR_VARIABLE_H_

#include "expressions/BaseExpression.h"

#include <string>

namespace storm {

namespace ir {

/*!
 * A class representing a untyped variable.
 */
class Variable {
public:
	/*!
	 * Default constructor. Creates an unnamed, untyped variable without initial value.
	 */
	Variable();

	/*!
	 * Creates an untyped variable with the given name and initial value.
	 * @param variableName the name of the variable.
	 * @param initialValue the expression that defines the initial value of the variable.
	 */
	Variable(std::string variableName, std::shared_ptr<storm::ir::expressions::BaseExpression> initialValue = std::shared_ptr<storm::ir::expressions::BaseExpression>());

	/*!
	 * Retrieves the name of the variable.
	 * @returns the name of the variable.
	 */
	std::string const& getVariableName() const;

	/*!
	 * Retrieves the expression defining the initial value of the variable.
	 * @returns the expression defining the initial value of the variable.
	 */
	std::shared_ptr<storm::ir::expressions::BaseExpression> const& getInitialValue() const;

private:
	// The name of the variable.
	std::string variableName;

	// The expression defining the initial value of the variable.
	std::shared_ptr<storm::ir::expressions::BaseExpression> initialValue;
};

} // namespace ir

} // namespace storm


#endif /* STORM_IR_VARIABLE_H_ */
