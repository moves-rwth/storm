/*
 * Program.h
 *
 *  Created on: 04.01.2013
 *      Author: chris
 */

#ifndef PROGRAM_H_
#define PROGRAM_H_

#include "expressions/BaseExpression.h"
#include "expressions/BooleanConstantExpression.h"
#include "expressions/IntegerConstantExpression.h"
#include "expressions/DoubleConstantExpression.h"
#include "Module.h"
#include "RewardModel.h"

namespace storm {

namespace ir {

class Program {
public:

	enum ModelType {UNDEFINED, DTMC, CTMC, MDP, CTMDP};

	Program() : modelType(UNDEFINED), booleanUndefinedConstantExpressions(), integerUndefinedConstantExpressions(), doubleUndefinedConstantExpressions(), modules(), rewards() {

	}

	Program(ModelType modelType, std::map<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>> booleanUndefinedConstantExpressions, std::map<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>> integerUndefinedConstantExpressions, std::map<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>> doubleUndefinedConstantExpressions, std::vector<storm::ir::Module> modules, std::map<std::string, storm::ir::RewardModel> rewards, std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>> labels)
		: modelType(modelType), booleanUndefinedConstantExpressions(booleanUndefinedConstantExpressions), integerUndefinedConstantExpressions(integerUndefinedConstantExpressions), doubleUndefinedConstantExpressions(doubleUndefinedConstantExpressions), modules(modules), rewards(rewards), labels(labels) {

	}

	Program(ModelType modelType, std::map<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>> booleanUndefinedConstantExpressions, std::map<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>> integerUndefinedConstantExpressions, std::map<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>> doubleUndefinedConstantExpressions, std::vector<storm::ir::Module> modules)
		: modelType(modelType), booleanUndefinedConstantExpressions(booleanUndefinedConstantExpressions), integerUndefinedConstantExpressions(integerUndefinedConstantExpressions), doubleUndefinedConstantExpressions(doubleUndefinedConstantExpressions), modules(modules), rewards() {

	}


	Program(ModelType modelType, std::vector<storm::ir::Module> modules) : modelType(modelType), booleanUndefinedConstantExpressions(), integerUndefinedConstantExpressions(), doubleUndefinedConstantExpressions(), modules(modules), rewards() {

	}

	std::string toString() {
		std::string result = "";
		switch (modelType) {
		case UNDEFINED: result += "undefined\n\n"; break;
		case DTMC: result += "dtmc\n\n"; break;
		case CTMC: result += "ctmc\n\n"; break;
		case MDP: result += "mdp\n\n"; break;
		case CTMDP: result += "ctmdp\n\n"; break;
		}
		for (auto element : booleanUndefinedConstantExpressions) {
			result += "const bool " + element.first + ";\n";
		}
		for (auto element : integerUndefinedConstantExpressions) {
			result += "const int " + element.first + ";\n";
		}
		for (auto element : doubleUndefinedConstantExpressions) {
			result += "const double " + element.first + ";\n";
		}
		result += "\n";
		for (auto mod : modules) {
			result += mod.toString();
			result += "\n";
		}

		for (auto rewardModel : rewards) {
			result += rewardModel.second.toString();
			result +="\n";
		}

		for (auto label : labels) {
			result += "label " + label.first + " = " + label.second->toString() + ";\n";
		}

		return result;
	}

private:
	ModelType modelType;
	std::map<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>> booleanUndefinedConstantExpressions;
	std::map<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>> integerUndefinedConstantExpressions;
	std::map<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>> doubleUndefinedConstantExpressions;
	std::vector<storm::ir::Module> modules;
	std::map<std::string, storm::ir::RewardModel> rewards;
	std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>> labels;
};

}

}

#endif /* PROGRAM_H_ */
