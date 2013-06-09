/*
 * Command.h
 *
 *  Created on: 06.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_COMMAND_H_
#define STORM_IR_COMMAND_H_

#include <vector>
#include <string>
#include <map>

#include "expressions/BaseExpression.h"
#include "Update.h"

namespace storm {

namespace ir {

/*!
 * A class representing a command.
 */
class Command {
public:
	/*!
	 * Default constructor. Creates a a command without name, guard and updates.
	 */
	Command();

	/*!
	 * Creates a command with the given name, guard and updates.
     *
	 * @param actionName The action name of the command.
	 * @param guardExpression the expression that defines the guard of the command.
     * @param updates A list of updates that is associated with this command.
	 */
	Command(std::string const& actionName, std::shared_ptr<storm::ir::expressions::BaseExpression> guardExpression, std::vector<storm::ir::Update> const& updates);

    /*!
	 * Creates a copy of the given command and performs the provided renaming.
     *
	 * @param oldCommand The command to copy.
     * @param renaming A mapping from names that are to be renamed to the names they are to be
     * replaced with.
     * @param booleanVariableToIndexMap A mapping from boolean variable names to their global indices.
     * @param integerVariableToIndexMap A mapping from integer variable names to their global indices.
	 */
	Command(Command const& oldCommand, std::map<std::string, std::string> const& renaming, std::map<std::string, uint_fast64_t> const& booleanVariableToIndexMap, std::map<std::string, uint_fast64_t> const& integerVariableToIndexMap);

	/*!
	 * Retrieves the action name of this command.
     *
	 * @return The action name of this command.
	 */
	std::string const& getActionName() const;

	/*!
	 * Retrieves a reference to the guard of the command.
     *
	 * @return A reference to the guard of the command.
	 */
	std::shared_ptr<storm::ir::expressions::BaseExpression> const& getGuard() const;

	/*!
	 * Retrieves the number of updates associated with this command.
     *
	 * @return The number of updates associated with this command.
	 */
	uint_fast64_t getNumberOfUpdates() const;

	/*!
	 * Retrieves a reference to the update with the given index.
     *
	 * @return A reference to the update with the given index.
	 */
	storm::ir::Update const& getUpdate(uint_fast64_t index) const;

	/*!
	 * Retrieves a string representation of this command.
     *
	 * @return A string representation of this command.
	 */
	std::string toString() const;

private:
	// The name of the command.
	std::string actionName;

	// The expression that defines the guard of the command.
	std::shared_ptr<storm::ir::expressions::BaseExpression> guardExpression;

	// The list of updates of the command.
	std::vector<storm::ir::Update> updates;
};

} // namespace ir

} // namespace storm

#endif /* STORM_IR_COMMAND_H_ */
