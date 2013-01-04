/*
 * Program.h
 *
 *  Created on: 04.01.2013
 *      Author: chris
 */

#ifndef PROGRAM_H_
#define PROGRAM_H_

#include "src/ir/expressions/ConstantExpression.h"
#include "Module.h"
#include "RewardModel.h"

namespace storm {

namespace ir {

class Program {
public:
	enum ModelType {DTMC, CTMC, MDP, CTMDP} modelType;
	std::map<std::string, storm::ir::expressions::ConstantExpression> undefinedConstantExpressions;
	std::vector<storm::ir::Module> modules;
	std::map<std::string, storm::ir::RewardModel> rewards;

	std::string toString() {
		return std::string("prog!");
	}
};

}

}

#endif /* PROGRAM_H_ */
