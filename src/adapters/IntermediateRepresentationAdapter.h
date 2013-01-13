/*
 * IntermediateRepresentationAdapter.h
 *
 *  Created on: 13.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_INTERMEDIATEREPRESENTATIONADAPTER_H_
#define STORM_IR_INTERMEDIATEREPRESENTATIONADAPTER_H_

#include <tuple>
#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <map>
#include <queue>
#include <iostream>

typedef std::pair<std::vector<bool>, std::vector<int_fast64_t>> StateType;

namespace storm {

namespace adapters {

class StateHash {
public:
	std::size_t operator()(StateType* state) const {
		size_t seed = 0;
		for (auto it = state->first.begin(); it != state->first.end(); ++it) {
			boost::hash_combine<bool>(seed, *it);
		}
		for (auto it = state->second.begin(); it != state->second.end(); ++it) {
			boost::hash_combine<int_fast64_t>(seed, *it);
		}
		return seed;
	}
};

class IntermediateRepresentationAdapter {
public:
	template<class T>
	static storm::storage::SquareSparseMatrix<T>* toSparseMatrix(storm::ir::Program const& program) {

		uint_fast64_t numberOfIntegerVariables = 0;
		uint_fast64_t numberOfBooleanVariables = 0;
		for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
			numberOfIntegerVariables += program.getModule(i).getNumberOfIntegerVariables();
			numberOfBooleanVariables += program.getModule(i).getNumberOfBooleanVariables();
		}

		std::vector<storm::ir::BooleanVariable> booleanVariables;
		std::vector<storm::ir::IntegerVariable> integerVariables;
		booleanVariables.resize(numberOfBooleanVariables);
		integerVariables.resize(numberOfIntegerVariables);

		std::unordered_map<std::string, uint_fast64_t> booleanVariableToIndexMap;
		std::unordered_map<std::string, uint_fast64_t> integerVariableToIndexMap;

		uint_fast64_t nextBooleanVariableIndex = 0;
		uint_fast64_t nextIntegerVariableIndex = 0;
		for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
			storm::ir::Module const& module = program.getModule(i);

			for (uint_fast64_t j = 0; j < module.getNumberOfBooleanVariables(); ++j) {
				booleanVariables[nextBooleanVariableIndex] = module.getBooleanVariable(j);
				booleanVariableToIndexMap[module.getBooleanVariable(j).getName()] = nextBooleanVariableIndex;
				++nextBooleanVariableIndex;
			}
			for (uint_fast64_t j = 0; j < module.getNumberOfIntegerVariables(); ++j) {
				integerVariables[nextIntegerVariableIndex] = module.getIntegerVariable(j);
				integerVariableToIndexMap[module.getIntegerVariable(j).getName()] = nextIntegerVariableIndex;
				++nextIntegerVariableIndex;
			}
		}

		StateType* initialState = new StateType(std::vector<bool>(), std::vector<int_fast64_t>());
		std::get<0>(*initialState).resize(numberOfBooleanVariables);
		std::get<1>(*initialState).resize(numberOfIntegerVariables);

		for (uint_fast64_t i = 0; i < numberOfBooleanVariables; ++i) {
			bool initialValue = booleanVariables[i].getInitialValue()->getValueAsBool(std::get<0>(*initialState), std::get<1>(*initialState));
			std::get<0>(*initialState)[i] = initialValue;
		}

		for (uint_fast64_t i = 0; i < numberOfIntegerVariables; ++i) {
			int_fast64_t initialValue = integerVariables[i].getInitialValue()->getValueAsInt(std::get<0>(*initialState), std::get<1>(*initialState));
			std::get<1>(*initialState)[i] = initialValue;
		}

		std::cout << "Initial State:" << std::get<0>(*initialState) << " / " << std::get<1>(*initialState) << std::endl;

		uint_fast64_t nextIndex = 1;
		std::unordered_map<StateType*, uint_fast64_t, StateHash> stateToIndexMap;
		std::vector<StateType*> allStates;
		std::queue<StateType*> stateQueue;

		allStates.push_back(initialState);
		stateQueue.push(initialState);
		stateToIndexMap[initialState] = 0;

		while (!stateQueue.empty()) {
			// Get first state in queue.
			StateType* currentState = stateQueue.front();
			stateQueue.pop();

			// Iterate over all modules.
			for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
				storm::ir::Module const& module = program.getModule(i);

				// Iterate over all commands.
				for (uint_fast64_t j = 0; j < module.getNumberOfCommands(); ++j) {
					storm::ir::Command const& command = module.getCommand(j);

					// Check if this command is enabled in the current state.
					if (command.getGuard()->getValueAsBool(std::get<0>(*currentState), std::get<1>(*currentState))) {
						for (uint_fast64_t k = 0; k < command.getNumberOfUpdates(); ++k) {
							storm::ir::Update const& update = command.getUpdate(k);

							std::map<std::string, storm::ir::Assignment> const& booleanAssignmentMap = update.getBooleanAssignments();
							for (auto assignedVariable : booleanAssignmentMap) {
								// Check if the variable that is being assigned is a boolean or an integer.
								// auto boolIt =

							}
						}
					}
				}
			}
		}

		// Now free all the elements we allocated.
		for (auto element : allStates) {
			delete element;
		}
		return nullptr;
	}

};

}

}


#endif /* STORM_IR_INTERMEDIATEREPRESENTATIONADAPTER_H_ */
