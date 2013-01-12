/*
 * Assignment.h
 *
 *  Created on: 06.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_ASSIGNMENT_H_
#define STORM_IR_ASSIGNMENT_H_

#include "expressions/BaseExpression.h"

#include <memory>

namespace storm {

namespace ir {

/*!
 * A class representing the assignment of an expression to a variable.
 */
class Assignment {
public:
	/*!
	 * Default constructor. Creates an empty assignment.
	 */
	Assignment();

	/*!
	 * Constructs an assignment using the given variable name and expression.
	 * @param variableName the variable that this assignment targets.
	 * @param expression the expression to assign to the variable.
	 */
	Assignment(std::string variableName, std::shared_ptr<storm::ir::expressions::BaseExpression> expression);

	/*!
	 * Retrieves the name of the variable that this assignment targets.
	 * @returns the name of the variable that this assignment targets.
	 */
	std::string const& getVariableName() const;

	/*!
	 * Retrieves the expression that is assigned to the variable.
	 * @returns the expression that is assigned to the variable.
	 */
	std::shared_ptr<storm::ir::expressions::BaseExpression> const& getExpression() const;

	/*!
	 * Retrieves a string representation of this assignment.
	 * @returns a string representation of this assignment.
	 */
	std::string toString() const;

private:
	// The name of the variable that this assignment targets.
	std::string variableName;

	// The expression that is assigned to the variable.
	std::shared_ptr<storm::ir::expressions::BaseExpression> expression;
};

} // namespace ir

} // namespace storm

#endif /* STORM_IR_ASSIGNMENT_H_ */
