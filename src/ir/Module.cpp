/*
 * Module.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include "Module.h"

#include "src/exceptions/InvalidArgumentException.h"

#include <sstream>
#include <iostream>

namespace storm {

namespace ir {

// Initializes all members with their default constructors.
Module::Module() : moduleName(), booleanVariables(), integerVariables(), booleanVariablesToIndexMap(),
		  integerVariablesToIndexMap(), commands(), actions(), actionsToCommandIndexMap() {
	// Nothing to do here.
}

// Initializes all members according to the given values.
Module::Module(std::string moduleName, std::vector<storm::ir::BooleanVariable> booleanVariables,
		std::vector<storm::ir::IntegerVariable> integerVariables,
		std::map<std::string, uint_fast64_t> booleanVariableToIndexMap,
		std::map<std::string, uint_fast64_t> integerVariableToIndexMap,
		std::vector<std::shared_ptr<storm::ir::Command>> commands)
	: moduleName(moduleName), booleanVariables(booleanVariables), integerVariables(integerVariables),
	  booleanVariablesToIndexMap(booleanVariableToIndexMap),
	  integerVariablesToIndexMap(integerVariableToIndexMap), commands(commands), actions(), actionsToCommandIndexMap() {
	std::cout << "Created module " << this << ":" << std::endl << this->toString() << std::endl;
	this->collectActions();
}

Module::Module(const Module& module, const std::string& moduleName, const std::map<std::string, std::string>& renaming, std::shared_ptr<VariableAdder> adder)
	: moduleName(moduleName) {
	std::cout << "Renaming module " << module.moduleName << " to " << moduleName << " with " << renaming.size() << " renamings:" << std::endl;
	for (auto it: renaming) {
		std::cout << "\t" << it.first << " -> " << it.second << std::endl;
	}
	std::cout << "Current module " << &module << ":" << std::endl << module.toString() << std::endl;
	this->booleanVariables.reserve(module.booleanVariables.size());
	for (BooleanVariable it: module.booleanVariables) {
		if (renaming.count(it.getName()) > 0) {
			this->booleanVariablesToIndexMap[renaming.at(it.getName())] = adder->addBooleanVariable(renaming.at(it.getName()), it.getInitialValue());
		} else std::cerr << "ERROR: " << moduleName << "." << it.getName() << " was not renamed!" << std::endl;
	}
	this->integerVariables.reserve(module.integerVariables.size());
	for (IntegerVariable it: module.integerVariables) {
		if (renaming.count(it.getName()) > 0) {
			this->integerVariablesToIndexMap[renaming.at(it.getName())] = adder->addIntegerVariable(renaming.at(it.getName()), it.getLowerBound(), it.getUpperBound(), it.getInitialValue());
		} else std::cerr << "ERROR: " << moduleName << "." << it.getName() << " was not renamed!" << std::endl;
	}
	this->booleanVariables.reserve(module.booleanVariables.size());
	for (BooleanVariable it: module.booleanVariables) {
		if (renaming.count(it.getName()) > 0) {
			this->booleanVariables.emplace_back(it, renaming.at(it.getName()), renaming, this->booleanVariablesToIndexMap, this->integerVariablesToIndexMap);
		} else std::cerr << "ERROR: " << moduleName << "." << it.getName() << " was not renamed!" << std::endl;
	}
	this->integerVariables.reserve(module.integerVariables.size());
	for (IntegerVariable it: module.integerVariables) {
		if (renaming.count(it.getName()) > 0) {
			this->integerVariables.emplace_back(it, renaming.at(it.getName()), renaming, this->booleanVariablesToIndexMap, this->integerVariablesToIndexMap);
		} else std::cerr << "ERROR: " << moduleName << "." << it.getName() << " was not renamed!" << std::endl;
	}
	
	this->commands.reserve(module.commands.size());
	for (std::shared_ptr<Command> cmd: module.commands) {
		std::cout << "2: Current command: " << cmd->toString() << std::endl;
		this->commands.emplace_back(new Command(*cmd, renaming, this->booleanVariablesToIndexMap, this->integerVariablesToIndexMap));
	}

	this->collectActions();
}

// Return the number of boolean variables.
uint_fast64_t Module::getNumberOfBooleanVariables() const {
	return this->booleanVariables.size();
}

// Return the requested boolean variable.
storm::ir::BooleanVariable const& Module::getBooleanVariable(uint_fast64_t index) const {
	return this->booleanVariables[index];
}

// Return the number of integer variables.
uint_fast64_t Module::getNumberOfIntegerVariables() const {
	return this->integerVariables.size();
}

// Return the requested integer variable.
storm::ir::IntegerVariable const& Module::getIntegerVariable(uint_fast64_t index) const {
	return this->integerVariables[index];
}

// Return the number of commands.
uint_fast64_t Module::getNumberOfCommands() const {
	return this->commands.size();
}

// Return the index of the variable if it exists and throw exception otherwise.
uint_fast64_t Module::getBooleanVariableIndex(std::string variableName) const {
	auto it = booleanVariablesToIndexMap.find(variableName);
	if (it != booleanVariablesToIndexMap.end()) {
		return it->second;
	}
	throw storm::exceptions::InvalidArgumentException() << "Cannot retrieve index of unknown "
			<< "boolean variable " << variableName << ".";
}

// Return the index of the variable if it exists and throw exception otherwise.
uint_fast64_t Module::getIntegerVariableIndex(std::string variableName) const {
	auto it = integerVariablesToIndexMap.find(variableName);
	if (it != integerVariablesToIndexMap.end()) {
		return it->second;
	}
	throw storm::exceptions::InvalidArgumentException() << "Cannot retrieve index of unknown "
			<< "variable " << variableName << ".";
}

// Return the requested command.
std::shared_ptr<storm::ir::Command> const Module::getCommand(uint_fast64_t index) const {
	return this->commands[index];
}

// Build a string representation of the variable.
std::string Module::toString() const {
	std::stringstream result;
	result << "module " << moduleName << std::endl;
	for (auto variable : booleanVariables) {
		result << "\t" << variable.toString() << std::endl;
	}
	for (auto variable : integerVariables) {
		result << "\t" << variable.toString() << std::endl;
	}
	for (auto command : commands) {
		result << "\t" << command->toString() << std::endl;
	}
	result << "endmodule" << std::endl;
	return result.str();
}

// Return set of actions.
std::set<std::string> const& Module::getActions() const {
	return this->actions;
}

// Return commands with given action.
std::shared_ptr<std::set<uint_fast64_t>> const Module::getCommandsByAction(std::string const& action) const {
	auto res = this->actionsToCommandIndexMap.find(action);
	if (res == this->actionsToCommandIndexMap.end()) {
		return std::shared_ptr<std::set<uint_fast64_t>>(new std::set<uint_fast64_t>());
	} else {
		return res->second;
	}
}

void Module::collectActions() {
	for (unsigned int id = 0; id < this->commands.size(); id++) {
		std::string action = this->commands[id]->getActionName();
		if (action != "") {
			if (this->actionsToCommandIndexMap.count(action) == 0) {
				this->actionsToCommandIndexMap[action] = std::shared_ptr<std::set<uint_fast64_t>>(new std::set<uint_fast64_t>());
			}
			this->actionsToCommandIndexMap[action]->insert(id);
			this->actions.insert(action);
		}
	}
}

} // namespace ir

} // namespace storm
