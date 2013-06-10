/*
 * VariableStateInterface.h
 *
 *  Created on: 10.06.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_VARIABLESTATEINTERFACE_H_
#define STORM_IR_VARIABLESTATEINTERFACE_H_

namespace storm {
namespace ir {
    
struct VariableStateInterface {
    /*!
     * Adds an integer variable with the given name, lower and upper bound.
     *
     * @param name The name of the boolean variable to add.
     */
    virtual uint_fast64_t addBooleanVariable(std::string const& name) = 0;
    
    /*!
     * Adds an integer variable with the given name, lower and upper bound.
     *
     * @param name The name of the integer variable to add.
     * @param lower The lower bound of the integer variable.
     * @param upper The upper bound of the integer variable.
     */
    virtual uint_fast64_t addIntegerVariable(std::string const& name) = 0;
    
	/*!
	 * Retrieves the variable expression for the boolean variable with the given name.
     *
	 * @param name The name of the boolean variable for which to retrieve the variable expression.
	 * @return The variable expression for the boolean variable with the given name.
	 */
	std::shared_ptr<VariableExpression> getBooleanVariableExpression(std::string const& name) const = 0;
    
	/*!
	 * Retrieves the variable expression for the integer variable with the given name.
     *
	 * @param name The name of the integer variable for which to retrieve the variable expression.
	 * @return The variable expression for the integer variable with the given name.
	 */
	std::shared_ptr<VariableExpression> getIntegerVariableExpression(std::string const& name) const = 0;
    
	/*!
	 * Retrieve the variable expression for the variable with the given name.
     *
	 * @param name The name of the variable for which to retrieve the variable expression.
	 * @return The variable expression for the variable with the given name.
	 */
	std::shared_ptr<VariableExpression> getVariableExpression(std::string const& name) const = 0;
    
    /*!
     * Retrieves the next free (global) index for a boolean variable.
     */
    virtual uint_fast64_t getNextGlobalBooleanVariableIndex() const = 0;
    
    /*!
     * Retrieves the next free (global) index for a integer variable.
     */
    virtual uint_fast64_t getNextGlobalIntegerVariableIndex() const = 0;
};

} // namespace ir
} // namespace storm

#endif /* STORM_IR_VARIABLESTATEINTERFACE_H_ */