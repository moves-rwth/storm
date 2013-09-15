/* 
 * File:   VariableState.h
 * Author: nafur
 *
 * Created on April 10, 2013, 4:43 PM
 */

#ifndef VARIABLESTATE_H
#define	VARIABLESTATE_H

#include <iostream>

#include "src/ir/IR.h"
#include "Includes.h"
#include "Tokens.h"

namespace storm {
    
namespace parser {
    
namespace prism {

using namespace storm::ir;
using namespace storm::ir::expressions;

template<typename T>
std::ostream& operator<<(std::ostream& out, qi::symbols<char, T>& symbols);

/*!
 * This class contains the internal state that is needed for parsing a PRISM model.
 */
class VariableState {
public:
    /*!
     * Creates a new variable state object. By default, this object will be set to a state in which
     * it is ready for performing a first run on some input. The first run creates all variables
     * while the second one checks for the correct usage of variables in expressions.
     *
     * @param firstRun If set, this object will be in a state ready for performing the first run. If
     * set to false, this object will assume that it has all variable data already.
     */
	VariableState(bool firstRun = true);
	
	/*!
	 * Indicator, if we are still in the first run.
	 */
	bool firstRun;
    
	/*!
	 * A parser for all reserved keywords.
	 */
	keywordsStruct keywords;

    /*!
     * Internal counter for the local index of the next new boolean variable.
     */
    uint_fast64_t nextLocalBooleanVariableIndex;
    
    /*!
     * Retrieves the next free local index for a boolean variable.
     *
     * @return The next free local index for a boolean variable.
     */
    uint_fast64_t getNextLocalBooleanVariableIndex() const;

    /*!
     * Internal counter for the local index of the next new integer variable.
     */
    uint_fast64_t nextLocalIntegerVariableIndex;
    
    /*!
     * Retrieves the next free global index for a integer variable.
     *
     * @return The next free global index for a integer variable.
     */
    uint_fast64_t getNextLocalIntegerVariableIndex() const;
    
	/*!
	 * Internal counter for the index of the next new boolean variable.
	 */
	uint_fast64_t nextGlobalBooleanVariableIndex;
    
    /*!
     * Retrieves the next free global index for a boolean variable.
     *
     * @return The next free global index for a boolean variable.
     */
    uint_fast64_t getNextGlobalBooleanVariableIndex() const;
    
	/*!
	 * Internal counter for the index of the next new integer variable.
	 */
	uint_fast64_t nextGlobalIntegerVariableIndex;

    /*!
     * Retrieves the next free global index for a integer variable.
     *
     * @return The next free global index for a integer variable.
     */
    uint_fast64_t getNextGlobalIntegerVariableIndex() const;
    
    /*!
     * Internal counter for the index of the next command.
     */
    uint_fast64_t nextGlobalCommandIndex;
    
    /*!
     * Retrieves the next free global index for a command.
     *
     * @return The next free global index for a command.
     */
    uint_fast64_t getNextGlobalCommandIndex() const;
    
	// Structures mapping variable and constant names to the corresponding expression nodes of
	// the intermediate representation.
	struct qi::symbols<char, std::shared_ptr<VariableExpression>> integerVariables_, booleanVariables_;
	struct qi::symbols<char, std::shared_ptr<BaseExpression>> integerConstants_, booleanConstants_, doubleConstants_;

	// A structure representing the identity function over identifier names.
	struct variableNamesStruct : qi::symbols<char, std::string> { }
            integerVariableNames_, booleanVariableNames_, commandNames_, labelNames_, allConstantNames_, moduleNames_,
			localBooleanVariables_, localIntegerVariables_, assignedLocalBooleanVariables_, assignedLocalIntegerVariables_;
	
	/*!
	 * Adds a new boolean variable with the given name.
     *
	 * @param name The name of the variable.
	 * @return The global index of the variable.
	 */
    uint_fast64_t addBooleanVariable(std::string const& name);
    
	/*!
	 * Adds a new integer variable with the given name.
     *
	 * @param name The name of the variable.
	 * @return The global index of the variable.
	 */
    uint_fast64_t addIntegerVariable(std::string const& name);

	/*!
	 * Retrieves the variable expression for the boolean variable with the given name.
     *
	 * @param name The name of the boolean variable for which to retrieve the variable expression.
	 * @return The variable expression for the boolean variable with the given name.
	 */
	std::shared_ptr<VariableExpression> getBooleanVariableExpression(std::string const& name) const;
    
	/*!
	 * Retrieves the variable expression for the integer variable with the given name.
     *
	 * @param name The name of the integer variable for which to retrieve the variable expression.
	 * @return The variable expression for the integer variable with the given name.
	 */
	std::shared_ptr<VariableExpression> getIntegerVariableExpression(std::string const& name) const;
    
	/*!
	 * Retrieve the variable expression for the variable with the given name.
     *
	 * @param name The name of the variable for which to retrieve the variable expression.
	 * @return The variable expression for the variable with the given name.
	 */
	std::shared_ptr<VariableExpression> getVariableExpression(std::string const& name) const;

	/*!
	 * Clears all local variables.
	 */
	void clearLocalVariables();

	/*!
	 * Check if the given string is a free identifier.
     *
	 * @param identifier A string to be checked.
	 * @return True iff the given string is a free identifier.
	 */
	bool isFreeIdentifier(std::string const& identifier) const;
    
	/*!
	 * Check if given string is a valid identifier.
     *
	 * @param identifier A string to be checked.
	 * @return True iff the given string is an identifier.
	 */
	bool isIdentifier(std::string const& identifier) const;

	/*!
	 * Prepare state to proceed to second parser run. Currently, this clears the constants.
	 */
	void prepareForSecondRun();
};

} // namespace prism
} // namespace parser
} // namespace storm

#endif	/* VARIABLESTATE_H */

