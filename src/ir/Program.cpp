/*
 * Program.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include "Program.h"

#include <sstream>

namespace storm {

namespace ir {

// Initializes all members with their default constructors.
Program::Program() : modelType(UNDEFINED), booleanUndefinedConstantExpressions(), integerUndefinedConstantExpressions(), doubleUndefinedConstantExpressions(), modules(), rewards() {
	// Nothing to do here.
}

// Initializes all members according to the given values.
Program::Program(ModelType modelType, std::map<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>> booleanUndefinedConstantExpressions, std::map<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>> integerUndefinedConstantExpressions, std::map<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>> doubleUndefinedConstantExpressions, std::vector<storm::ir::Module> modules, std::map<std::string, storm::ir::RewardModel> rewards, std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>> labels)
	: modelType(modelType), booleanUndefinedConstantExpressions(booleanUndefinedConstantExpressions), integerUndefinedConstantExpressions(integerUndefinedConstantExpressions), doubleUndefinedConstantExpressions(doubleUndefinedConstantExpressions), modules(modules), rewards(rewards), labels(labels) {
	// Nothing to do here.
}

// Build a string representation of the program.
std::string Program::toString() const {
	std::stringstream result;
	switch (modelType) {
	case UNDEFINED: result << "undefined"; break;
	case DTMC: result << "dtmc"; break;
	case CTMC: result << "ctmc"; break;
	case MDP: result << "mdp"; break;
	case CTMDP: result << "ctmdp"; break;
	}
	result << std::endl;

	for (auto element : booleanUndefinedConstantExpressions) {
		result << "const bool " << element.first << ";" << std::endl;
	}
	for (auto element : integerUndefinedConstantExpressions) {
		result << "const int " << element.first << ";" << std::endl;
	}
	for (auto element : doubleUndefinedConstantExpressions) {
		result << "const double " << element.first << ";" << std::endl;
	}
	result << std::endl;

	for (auto mod : modules) {
		result << mod.toString() << std::endl;
	}

	for (auto rewardModel : rewards) {
		result << rewardModel.second.toString() << std::endl;
	}

	for (auto label : labels) {
		result << "label " << label.first << " = " << label.second->toString() <<";" << std::endl;
	}

	return result.str();
}

uint_fast64_t Program::getNumberOfModules() const {
	return this->modules.size();
}

storm::ir::Module const& Program::getModule(uint_fast64_t index) const {
	return this->modules[index];
}


} // namespace ir

} // namepsace storm
