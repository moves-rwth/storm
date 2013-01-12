/*
 * Command.h
 *
 *  Created on: 06.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_COMMAND_H_
#define STORM_IR_COMMAND_H_

#include "expressions/BaseExpression.h"
#include "Update.h"

#include <vector>
#include <string>

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
	 * @param commandName the name of the command.
	 * @param guardExpression the expression that defines the guard of the command.
	 */
	Command(std::string commandName, std::shared_ptr<storm::ir::expressions::BaseExpression> guardExpression, std::vector<storm::ir::Update> updates);

	/*!
	 * Retrieves a string representation of this command.
	 * @returns a string representation of this command.
	 */
	std::string toString() const;

private:
	// The name of the command.
	std::string commandName;

	// The expression that defines the guard of the command.
	std::shared_ptr<storm::ir::expressions::BaseExpression> guardExpression;

	// The list of updates of the command.
	std::vector<storm::ir::Update> updates;
};

} // namespace ir

} // namespace storm

#endif /* STORM_IR_COMMAND_H_ */
