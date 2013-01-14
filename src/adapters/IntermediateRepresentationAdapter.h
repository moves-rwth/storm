/*
 * IntermediateRepresentationAdapter.h
 *
 *  Created on: 13.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_INTERMEDIATEREPRESENTATIONADAPTER_H_
#define STORM_IR_INTERMEDIATEREPRESENTATIONADAPTER_H_

#include "src/storage/SparseMatrix.h"

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

class StateCompare {
public:
	bool operator()(StateType* state1, StateType* state2) const {
		return *state1 == *state2;
	}
};

class IntermediateRepresentationAdapter {
public:
	template<class T>
	static storm::storage::SparseMatrix<T>* toSparseMatrix(storm::ir::Program const& program) {

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

		StateType* initialState = getNewState(numberOfBooleanVariables, numberOfIntegerVariables);

		for (uint_fast64_t i = 0; i < numberOfBooleanVariables; ++i) {
			bool initialValue = booleanVariables[i].getInitialValue()->getValueAsBool(*initialState);
			std::get<0>(*initialState)[i] = initialValue;
		}

		for (uint_fast64_t i = 0; i < numberOfIntegerVariables; ++i) {
			int_fast64_t initialValue = integerVariables[i].getInitialValue()->getValueAsInt(*initialState);
			std::get<1>(*initialState)[i] = initialValue;
		}

		uint_fast64_t nextIndex = 1;
		std::unordered_map<StateType*, uint_fast64_t, StateHash, StateCompare> stateToIndexMap;
		std::vector<StateType*> allStates;
		std::queue<StateType*> stateQueue;

		allStates.push_back(initialState);
		stateQueue.push(initialState);
		stateToIndexMap[initialState] = 0;

		uint_fast64_t totalNumberOfTransitions = 0;
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
					if (command.getGuard()->getValueAsBool(*currentState)) {
						std::unordered_map<StateType*, double, StateHash, StateCompare> stateToProbabilityMap;
						for (uint_fast64_t k = 0; k < command.getNumberOfUpdates(); ++k) {
							storm::ir::Update const& update = command.getUpdate(k);

							StateType* newState = new StateType(*currentState);

							std::map<std::string, storm::ir::Assignment> const& booleanAssignmentMap = update.getBooleanAssignments();
							for (auto assignedVariable : booleanAssignmentMap) {
								setValue(newState, booleanVariableToIndexMap[assignedVariable.first], assignedVariable.second.getExpression()->getValueAsBool(*currentState));
							}
							std::map<std::string, storm::ir::Assignment> const& integerAssignmentMap = update.getIntegerAssignments();
							for (auto assignedVariable : integerAssignmentMap) {
								setValue(newState, integerVariableToIndexMap[assignedVariable.first], assignedVariable.second.getExpression()->getValueAsInt(*currentState));
							}

							auto probIt = stateToProbabilityMap.find(newState);
							if (probIt != stateToProbabilityMap.end()) {
								stateToProbabilityMap[newState] += update.getLikelihoodExpression()->getValueAsDouble(*currentState);
							} else {
								++totalNumberOfTransitions;
								stateToProbabilityMap[newState] = update.getLikelihoodExpression()->getValueAsDouble(*currentState);
							}

							auto it = stateToIndexMap.find(newState);
							if (it != stateToIndexMap.end()) {
								// Delete the state object directly as we have already seen that state.
								delete newState;
							} else {
								// Add state to the queue of states that are still to be explored.
								stateQueue.push(newState);

								// Add state to list of all states so that we can delete it at the end.
								allStates.push_back(newState);

								// Give a unique index to the newly found state.
								stateToIndexMap[newState] = nextIndex;
								++nextIndex;
							}
						}
					}
				}
			}
		}

		std::cout << "Found " << allStates.size() << " reachable states and " << totalNumberOfTransitions << " transitions.";

		storm::storage::SparseMatrix<T>* resultMatrix = new storm::storage::SparseMatrix<T>(allStates.size());
		resultMatrix->initialize(totalNumberOfTransitions);

		uint_fast64_t currentIndex = 0;
		for (StateType* currentState : allStates) {
			// Iterate over all modules.
			for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
				storm::ir::Module const& module = program.getModule(i);

				// Iterate over all commands.
				for (uint_fast64_t j = 0; j < module.getNumberOfCommands(); ++j) {
					storm::ir::Command const& command = module.getCommand(j);

					// Check if this command is enabled in the current state.
					if (command.getGuard()->getValueAsBool(*currentState)) {
						std::map<uint_fast64_t, double> stateIndexToProbabilityMap;
						for (uint_fast64_t k = 0; k < command.getNumberOfUpdates(); ++k) {
							storm::ir::Update const& update = command.getUpdate(k);

							StateType* newState = new StateType(*currentState);

							std::map<std::string, storm::ir::Assignment> const& booleanAssignmentMap = update.getBooleanAssignments();
							for (auto assignedVariable : booleanAssignmentMap) {
								setValue(newState, booleanVariableToIndexMap[assignedVariable.first], assignedVariable.second.getExpression()->getValueAsBool(*currentState));
							}
							std::map<std::string, storm::ir::Assignment> const& integerAssignmentMap = update.getIntegerAssignments();
							for (auto assignedVariable : integerAssignmentMap) {
								setValue(newState, integerVariableToIndexMap[assignedVariable.first], assignedVariable.second.getExpression()->getValueAsInt(*currentState));
							}

							uint_fast64_t targetIndex = (*stateToIndexMap.find(newState)).second;
							delete newState;

							auto probIt = stateIndexToProbabilityMap.find(targetIndex);
							if (probIt != stateIndexToProbabilityMap.end()) {
								stateIndexToProbabilityMap[targetIndex] += update.getLikelihoodExpression()->getValueAsDouble(*currentState);
							} else {
								stateIndexToProbabilityMap[targetIndex] = update.getLikelihoodExpression()->getValueAsDouble(*currentState);
							}
						}

						// Now insert the actual values into the matrix.
						for (auto targetIndex : stateIndexToProbabilityMap) {
							resultMatrix->addNextValue(currentIndex, targetIndex.first, targetIndex.second);
						}
					}
				}
			}
			++currentIndex;
		}

		resultMatrix->finalize();

		// Now free all the elements we allocated.
		for (auto element : allStates) {
			delete element;
		}
		return resultMatrix;
	}

private:
	static StateType* getNewState(uint_fast64_t numberOfBooleanVariables, uint_fast64_t numberOfIntegerVariables) {
		StateType* result = new StateType();
		result->first.resize(numberOfBooleanVariables);
		result->second.resize(numberOfIntegerVariables);
		return result;
	}

	static void setValue(StateType* state, uint_fast64_t index, bool value) {
		std::get<0>(*state)[index] = value;
	}

	static void setValue(StateType* state, uint_fast64_t index, int_fast64_t value) {
		std::get<1>(*state)[index] = value;
	}
};

}

}


#endif /* STORM_IR_INTERMEDIATEREPRESENTATIONADAPTER_H_ */
