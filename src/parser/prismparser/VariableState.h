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
 * This class contains the state that is needed during the parsing of a prism model.
 */
struct VariableState : public storm::ir::VariableAdder {

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
	std::shared_ptr<VariableExpression> getBooleanVariableExpression(std::string const& name);
    
	/*!
	 * Retrieves the variable expression for the integer variable with the given name.
     *
	 * @param name The name of the integer variable for which to retrieve the variable expression.
	 * @return The variable expression for the integer variable with the given name.
	 */
	std::shared_ptr<VariableExpression> getIntegerVariableExpression(std::string const& name);
    
	/*!
	 * Retrieve the variable expression for the variable with the given name.
     *
	 * @param name The name of the variable for which to retrieve the variable expression.
	 * @return The variable expression for the variable with the given name.
	 */
	std::shared_ptr<VariableExpression> getVariableExpression(std::string const& name);

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

