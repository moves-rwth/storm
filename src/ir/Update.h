/*
 * Update.h
 *
 *  Created on: 06.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_UPDATE_H_
#define STORM_IR_UPDATE_H_

#include "expressions/BaseExpression.h"
#include "Assignment.h"

#include <map>
#include <memory>

namespace storm {

namespace ir {

/*!
 * A class representing an update of a command.
 */
class Update {
public:
	/*!
	 * Default constructor. Creates an empty update.
	 */
	Update();

	/*!
	 * Creates an update with the given expression specifying the likelihood and the mapping of
	 * variable to their assignments.
	 * @param likelihoodExpression an expression specifying the likelihood of this update.
	 * @param assignments a map of variable names to their assignments.
	 */
	Update(std::shared_ptr<storm::ir::expressions::BaseExpression> likelihoodExpression, std::map<std::string, storm::ir::Assignment> assignments);

	/*!
	 * Retrieves a string representation of this update.
	 * @returns a string representation of this update.
	 */
	std::string toString() const;

private:
	// An expression specifying the likelihood of taking this update.
	std::shared_ptr<storm::ir::expressions::BaseExpression> likelihoodExpression;

	// A mapping of variable names to their assignments in this update.
	std::map<std::string, storm::ir::Assignment> assignments;
};

} // namespace ir

} // namespace storm

#endif /*STORM_IR_UPDATE_H_ */
