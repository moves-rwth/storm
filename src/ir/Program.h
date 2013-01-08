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
	std::map<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>> booleanUndefinedConstantExpressions;
	std::map<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>> integerUndefinedConstantExpressions;
	std::map<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>> doubleUndefinedConstantExpressions;
	std::vector<storm::ir::Module> modules;
	std::map<std::string, storm::ir::RewardModel> rewards;

	std::string toString() {
		std::string result = "";
		for (auto element : booleanUndefinedConstantExpressions) {
			result += "const bool " + element.first + ";\n";
		}
		for (auto element : integerUndefinedConstantExpressions) {
			result += "const int " + element.first + ";\n";
		}
		for (auto element : doubleUndefinedConstantExpressions) {
			result += "const double " + element.first + ";\n";
		}
		for (auto mod : modules) {
			result += mod.toString();
		}

		return result;
	}

	void addBooleanUndefinedConstantExpression(std::string constantName, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression> constantExpression) {
		booleanUndefinedConstantExpressions[constantName] = constantExpression;
	}

	void addIntegerUndefinedConstantExpression(std::string constantName, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression> constantExpression) {
		integerUndefinedConstantExpressions[constantName] = constantExpression;
	}

	void addDoubleUndefinedConstantExpression(std::string constantName, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression> constantExpression) {
		doubleUndefinedConstantExpressions[constantName] = constantExpression;
	}

	void setModules(std::vector<storm::ir::Module> modules) {
		this->modules = modules;
	}
};

}

}

BOOST_FUSION_ADAPT_STRUCT(
    storm::ir::Program,
    (std::vector<storm::ir::Module>, modules)
)

#endif /* PROGRAM_H_ */
