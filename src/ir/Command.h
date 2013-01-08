/*
 * Command.h
 *
 *  Created on: 06.01.2013
 *      Author: chris
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include "IR.h"

namespace storm {

namespace ir {

class Command {
public:
	std::string commandName;
	std::shared_ptr<storm::ir::expressions::BaseExpression> guardExpression;
	std::vector<storm::ir::Update> updates;

	std::string toString() {
		return "command!";
	}

};

}

}

BOOST_FUSION_ADAPT_STRUCT(
    storm::ir::Command,
    (std::string, commandName)
    (std::shared_ptr<storm::ir::expressions::BaseExpression>, guardExpression)
    (std::vector<storm::ir::Update>, updates)
)

#endif /* COMMAND_H_ */
