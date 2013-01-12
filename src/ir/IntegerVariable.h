/*
 * IntegerVariable.h
 *
 *  Created on: 08.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_INTEGERVARIABLE_H_
#define STORM_IR_INTEGERVARIABLE_H_

#include "expressions/BaseExpression.h"
#include "Variable.h"
#include <memory>

namespace storm {

namespace ir {

/*!
 * A class representing an integer variable.
 */
class IntegerVariable : public Variable {
public:
	/*!
	 * Default constructor. Creates an integer variable without a name and lower and upper bounds.
	 */
	IntegerVariable();

	/*!
	 * Creates an integer variable with the given name, lower and upper bounds and the given initial
	 * value.
	 * @param variableName the name of the variable.
	 * @param lowerBound the lower bound of the domain of the variable.
	 * @param upperBound the upper bound of the domain of the variable.
	 * @param initialValue the expression that defines the initial value of the variable.
	 */
	IntegerVariable(std::string variableName, std::shared_ptr<storm::ir::expressions::BaseExpression> lowerBound, std::shared_ptr<storm::ir::expressions::BaseExpression> upperBound, std::shared_ptr<storm::ir::expressions::BaseExpression> initialValue = std::shared_ptr<storm::ir::expressions::BaseExpression>());

	/*!
	 * Retrieves a string representation of this variable.
	 * @returns a string representation of this variable.
	 */
	std::string toString() const;

private:
	// The lower bound of the domain of the variable.
	std::shared_ptr<storm::ir::expressions::BaseExpression> lowerBound;

	// The upper bound of the domain of the variable.
	std::shared_ptr<storm::ir::expressions::BaseExpression> upperBound;
};

} // namespace ir

} // namespace storm

#endif /* STORM_IR_INTEGERVARIABLE_H_ */
