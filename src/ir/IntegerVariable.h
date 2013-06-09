/*
 * IntegerVariable.h
 *
 *  Created on: 08.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_INTEGERVARIABLE_H_
#define STORM_IR_INTEGERVARIABLE_H_

#include <memory>

#include "src/ir/Variable.h"
#include "expressions/BaseExpression.h"

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
	 * Creates a boolean variable with the given name and the given initial value.
     *
	 * @param globalIndex A globally unique index for the variable.
     * @param localIndex A module-local unique index for the variable.
	 * @param variableName The name of the variable.
	 * @param lowerBound the lower bound of the domain of the variable.
	 * @param upperBound the upper bound of the domain of the variable.
	 * @param initialValue the expression that defines the initial value of the variable.
	 */
	IntegerVariable(uint_fast64_t globalIndex, uint_fast64_t localIndex, std::string const& variableName, std::shared_ptr<storm::ir::expressions::BaseExpression> lowerBound, std::shared_ptr<storm::ir::expressions::BaseExpression> upperBound, std::shared_ptr<storm::ir::expressions::BaseExpression> initialValue = std::shared_ptr<storm::ir::expressions::BaseExpression>(nullptr));

    /*!
	 * Creates a copy of the given integer variable and performs the provided renaming.
     *
	 * @param oldVariable The variable to copy.
	 * @param newName New name of this variable.
     * @param newGlobalIndex The new global index of the variable.
     * @param renaming A mapping from names that are to be renamed to the names they are to be
     * replaced with.
     * @param booleanVariableToIndexMap A mapping from boolean variable names to their global indices.
     * @param integerVariableToIndexMap A mapping from integer variable names to their global indices.
	 */
	IntegerVariable(IntegerVariable const& oldVariable, std::string const& newName, uint_fast64_t newGlobalIndex, std::map<std::string, std::string> const& renaming, std::map<std::string, uint_fast64_t> const& booleanVariableToIndexMap, std::map<std::string, uint_fast64_t> const& integerVariableToIndexMap);
	
	/*!
	 * Retrieves the lower bound for this integer variable.
	 * @returns the lower bound for this integer variable.
	 */
	std::shared_ptr<storm::ir::expressions::BaseExpression> getLowerBound() const;

	/*!
	 * Retrieves the upper bound for this integer variable.
	 * @returns the upper bound for this integer variable.
	 */
	std::shared_ptr<storm::ir::expressions::BaseExpression> getUpperBound() const;

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
