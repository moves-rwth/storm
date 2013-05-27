/* 
 * File:   VariableState.h
 * Author: nafur
 *
 * Created on April 10, 2013, 4:43 PM
 */

#ifndef VARIABLESTATE_H
#define	VARIABLESTATE_H

#include "src/ir/IR.h"
#include "Includes.h"
#include "Tokens.h"
#include <iostream>

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
	uint_fast64_t nextBooleanVariableIndex;
	/*!
	 * Internal counter for the index of the next new integer variable.
	 */
	uint_fast64_t nextIntegerVariableIndex;

	// Structures mapping variable and constant names to the corresponding expression nodes of
	// the intermediate representation.
	struct qi::symbols<char, std::shared_ptr<VariableExpression>> integerVariables_, booleanVariables_;
	struct qi::symbols<char, std::shared_ptr<BaseExpression>> integerConstants_, booleanConstants_, doubleConstants_;

	// A structure representing the identity function over identifier names.
	struct variableNamesStruct : qi::symbols<char, std::string> { } integerVariableNames_, booleanVariableNames_, commandNames_, labelNames_, allConstantNames_, moduleNames_,
			localBooleanVariables_, localIntegerVariables_, assignedLocalBooleanVariables_, assignedLocalIntegerVariables_;
	
	/*!
	 * Add a new boolean variable with the given name.
	 * @param name Name of the variable.
	 * @return Index of the variable.
	 */
	uint_fast64_t addBooleanVariable(const std::string& name);
	/*!
	 * Add a new integer variable with the given name and constraints.
	 * @param name Name of the variable.
	 * @param lower Lower bound for the variable value.
	 * @param upper Upper bound for the variable value.
	 * @return Index of the variable.
	 */
	uint_fast64_t addIntegerVariable(const std::string& name, const std::shared_ptr<storm::ir::expressions::BaseExpression> lower, const std::shared_ptr<storm::ir::expressions::BaseExpression> upper);

	/*!
	 * Retrieve boolean Variable with given name.
	 * @param name Variable name.
	 * @returns Variable.
	 */
	std::shared_ptr<VariableExpression> getBooleanVariable(const std::string& name);
	/*!
	 * Retrieve integer Variable with given name.
	 * @param name Variable name.
	 * @returns Variable.
	 */
	std::shared_ptr<VariableExpression> getIntegerVariable(const std::string& name);
	/*!
	 * Retrieve any Variable with given name.
	 * @param name Variable name.
	 * @returns Variable.
	 */
	std::shared_ptr<VariableExpression> getVariable(const std::string& name);

	/*!
	 * Perform operations necessary for a module renaming.
	 * This includes creating new variables and constants.
	 * @param renaming String mapping for renaming operation.
	 */
	void performRenaming(const std::map<std::string, std::string>& renaming);

	/*!
	 * Start with a new module.
	 * Clears sets of local variables.
	 */
	void startModule();

	/*!
	 * Check if given string is a free identifier.
	 * @param s String.
	 * @returns If s is a free identifier.
	 */
	bool isFreeIdentifier(std::string& s) const;
	/*!
	 * Check if given string is a valid identifier.
	 * @param s String.
	 * @returns If s is a valid identifier.
	 */
	bool isIdentifier(std::string& s) const;

	/*!
	 * Prepare state to proceed to second parser run.
	 * Clears constants.
	 */
	void prepareForSecondRun();
};

}
}
}

#endif	/* VARIABLESTATE_H */

