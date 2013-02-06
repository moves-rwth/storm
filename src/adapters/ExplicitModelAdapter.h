/*
 * IntermediateRepresentationAdapter.h
 *
 *  Created on: 13.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_EXPLICITMODELADAPTER_H_
#define STORM_IR_EXPLICITMODELADAPTER_H_

#include "src/storage/SparseMatrix.h"
#include "src/utility/Settings.h"

#include <tuple>
#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <map>
#include <queue>
#include <iostream>
#include <memory>

typedef std::pair<std::vector<bool>, std::vector<int_fast64_t>> StateType;

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

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

class ExplicitModelAdapter {
public:
	ExplicitModelAdapter(std::shared_ptr<storm::ir::Program> program) : program(program), allStates(),
		stateToIndexMap(), booleanVariables(), integerVariables(), booleanVariableToIndexMap(),
		integerVariableToIndexMap(), numberOfTransitions(0) {

	}

	template<class T>
	std::shared_ptr<storm::storage::SparseMatrix<T>> toSparseMatrix() {
		LOG4CPLUS_INFO(logger, "Creating sparse matrix for probabilistic program.");

		this->computeReachableStateSpace();
		std::shared_ptr<storm::storage::SparseMatrix<T>> resultMatrix = this->buildMatrix<T>();

		LOG4CPLUS_INFO(logger, "Created sparse matrix with " << resultMatrix->getRowCount() << " reachable states and " << resultMatrix->getNonZeroEntryCount() << " transitions.");

		this->clearReachableStateSpace();

		return resultMatrix;
	}

private:
	static void setValue(StateType* state, uint_fast64_t index, bool value) {
		std::get<0>(*state)[index] = value;
	}

	static void setValue(StateType* state, uint_fast64_t index, int_fast64_t value) {
		std::get<1>(*state)[index] = value;
	}

	void prepareAuxiliaryDatastructures() {
		uint_fast64_t numberOfIntegerVariables = 0;
		uint_fast64_t numberOfBooleanVariables = 0;
		for (uint_fast64_t i = 0; i < program->getNumberOfModules(); ++i) {
			numberOfIntegerVariables += program->getModule(i).getNumberOfIntegerVariables();
			numberOfBooleanVariables += program->getModule(i).getNumberOfBooleanVariables();
		}

		booleanVariables.resize(numberOfBooleanVariables);
		integerVariables.resize(numberOfIntegerVariables);

		uint_fast64_t nextBooleanVariableIndex = 0;
		uint_fast64_t nextIntegerVariableIndex = 0;
		for (uint_fast64_t i = 0; i < program->getNumberOfModules(); ++i) {
			storm::ir::Module const& module = program->getModule(i);

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
	}

	void computeReachableStateSpace() {
		bool nondeterministicModel = program->getModelType() == storm::ir::Program::MDP || program->getModelType() == storm::ir::Program::CTMDP;

		// Prepare some internal data structures, such as mappings from variables to indices and so on.
		this->prepareAuxiliaryDatastructures();

		// Create a fresh state which can hold as many boolean and integer variables as there are.
		StateType* initialState = new StateType();
		initialState->first.resize(booleanVariables.size());
		initialState->second.resize(integerVariables.size());

		// Now initialize all fields in the value vectors of the state according to the initial
		// values provided by the input program.
		for (uint_fast64_t i = 0; i < booleanVariables.size(); ++i) {
			bool initialValue = booleanVariables[i].getInitialValue()->getValueAsBool(initialState);
			std::get<0>(*initialState)[i] = initialValue;
		}
		for (uint_fast64_t i = 0; i < integerVariables.size(); ++i) {
			int_fast64_t initialValue = integerVariables[i].getInitialValue()->getValueAsInt(initialState);
			std::get<1>(*initialState)[i] = initialValue;
		}

		// Now set up the environment for a breadth-first search starting from the initial state.
		uint_fast64_t nextIndex = 1;
		allStates.clear();
		stateToIndexMap.clear();
		std::queue<StateType*> stateQueue;

		allStates.push_back(initialState);
		stateQueue.push(initialState);
		stateToIndexMap[initialState] = 0;

		numberOfTransitions = 0;
		while (!stateQueue.empty()) {
			// Get first state in queue.
			StateType* currentState = stateQueue.front();
			stateQueue.pop();

			// Remember whether the state has at least one transition such that transitions can be
			// inserted upon detection of a deadlock state.
			bool hasTransition = false;

			// Iterate over all modules.
			for (uint_fast64_t i = 0; i < program->getNumberOfModules(); ++i) {
				storm::ir::Module const& module = program->getModule(i);

				// Iterate over all commands.
				for (uint_fast64_t j = 0; j < module.getNumberOfCommands(); ++j) {
					storm::ir::Command const& command = module.getCommand(j);

					// Check if this command is enabled in the current state.
					if (command.getGuard()->getValueAsBool(currentState)) {
						hasTransition = true;

						// Remember what states are targeted by an update of the current command
						// in order to be able to sum those probabilities and not increase the
						// transition count.
						std::unordered_map<StateType*, double, StateHash, StateCompare> stateToProbabilityMap;

						// Keep a queue of states to delete after the current command. When one
						// command is processed and states are encountered which were already found
						// before, we can only delete them after the command has been processed,
						// because the stateToProbabilityMap will contain illegal values otherwise.
						std::queue<StateType*> statesToDelete;

						// Now iterate over all updates to see where the updates take the current
						// state.
						for (uint_fast64_t k = 0; k < command.getNumberOfUpdates(); ++k) {
							storm::ir::Update const& update = command.getUpdate(k);

							// Now copy the current state and only overwrite the entries in the
							// vectors if there is an assignment to that variable in the current
							// update.
							StateType* newState = new StateType(*currentState);
							std::map<std::string, storm::ir::Assignment> const& booleanAssignmentMap = update.getBooleanAssignments();
							for (auto assignedVariable : booleanAssignmentMap) {
								setValue(newState, booleanVariableToIndexMap[assignedVariable.first], assignedVariable.second.getExpression()->getValueAsBool(currentState));
							}
							std::map<std::string, storm::ir::Assignment> const& integerAssignmentMap = update.getIntegerAssignments();
							for (auto assignedVariable : integerAssignmentMap) {
								setValue(newState, integerVariableToIndexMap[assignedVariable.first], assignedVariable.second.getExpression()->getValueAsInt(currentState));
							}

							// If we have already found the target state of the update, we must not
							// increase the transition count.
							auto probIt = stateToProbabilityMap.find(newState);
							if (probIt != stateToProbabilityMap.end()) {
								stateToProbabilityMap[newState] += update.getLikelihoodExpression()->getValueAsDouble(currentState);
							} else {
								++numberOfTransitions;
								stateToProbabilityMap[newState] = update.getLikelihoodExpression()->getValueAsDouble(currentState);
							}

							// Depending on whether the state was found previously, we mark it for
							// deletion or add it to the reachable state space and mark it for
							// further exploration.
							auto it = stateToIndexMap.find(newState);
							if (it != stateToIndexMap.end()) {
								// Queue the state object for deletion as we have already seen that
								// state.
								statesToDelete.push(newState);
							} else {
								// Add state to the queue of states that are still to be explored.
								stateQueue.push(newState);

								// Add state to list of all reachable states.
								allStates.push_back(newState);

								// Give a unique index to the newly found state.
								stateToIndexMap[newState] = nextIndex;
								++nextIndex;
							}
						}

						// Now delete all states queued for deletion.
						while (!statesToDelete.empty()) {
							delete statesToDelete.front();
							statesToDelete.pop();
						}
					}
				}
			}

			// If the state is a deadlock state, and the corresponding flag is set, we tolerate that
			// and increase the number of transitions by one, because a self-loop is going to be
			// inserted in the next step. If the flag is not set, an exception is thrown.
			if (!hasTransition) {
				if (storm::settings::instance()->isSet("fix-deadlocks")) {
					++numberOfTransitions;
				} else {
					LOG4CPLUS_ERROR(logger, "Error while creating sparse matrix from probabilistic program: found deadlock state.");
					throw storm::exceptions::WrongFileFormatException() << "Error while creating sparse matrix from probabilistic program: found deadlock state.";
				}
			}
		}
	}

	template<class T>
	std::shared_ptr<storm::storage::SparseMatrix<T>> buildMatrix() {
		std::shared_ptr<storm::storage::SparseMatrix<T>> resultMatrix(new storm::storage::SparseMatrix<T>(allStates.size()));
		resultMatrix->initialize(numberOfTransitions);

		// Keep track of the running index to insert values into correct matrix row.
		uint_fast64_t currentIndex = 0;

		// Determine the matrix content for every row (i.e. reachable state).
		for (StateType* currentState : allStates) {
			bool hasTransition = false;

			// Iterate over all modules.
			for (uint_fast64_t i = 0; i < program->getNumberOfModules(); ++i) {
				storm::ir::Module const& module = program->getModule(i);

				// Iterate over all commands.
				for (uint_fast64_t j = 0; j < module.getNumberOfCommands(); ++j) {
					storm::ir::Command const& command = module.getCommand(j);

					// Check if this command is enabled in the current state.
					if (command.getGuard()->getValueAsBool(currentState)) {
						hasTransition = true;
						std::map<uint_fast64_t, double> stateIndexToProbabilityMap;
						for (uint_fast64_t k = 0; k < command.getNumberOfUpdates(); ++k) {
							storm::ir::Update const& update = command.getUpdate(k);

							StateType* newState = new StateType(*currentState);

							std::map<std::string, storm::ir::Assignment> const& booleanAssignmentMap = update.getBooleanAssignments();
							for (auto assignedVariable : booleanAssignmentMap) {
								setValue(newState, booleanVariableToIndexMap[assignedVariable.first], assignedVariable.second.getExpression()->getValueAsBool(currentState));
							}
							std::map<std::string, storm::ir::Assignment> const& integerAssignmentMap = update.getIntegerAssignments();
							for (auto assignedVariable : integerAssignmentMap) {
								setValue(newState, integerVariableToIndexMap[assignedVariable.first], assignedVariable.second.getExpression()->getValueAsInt(currentState));
							}

							uint_fast64_t targetIndex = (*stateToIndexMap.find(newState)).second;
							delete newState;

							auto probIt = stateIndexToProbabilityMap.find(targetIndex);
							if (probIt != stateIndexToProbabilityMap.end()) {
								stateIndexToProbabilityMap[targetIndex] += update.getLikelihoodExpression()->getValueAsDouble(currentState);
							} else {
								stateIndexToProbabilityMap[targetIndex] = update.getLikelihoodExpression()->getValueAsDouble(currentState);
							}
						}

						// Now insert the actual values into the matrix.
						for (auto targetIndex : stateIndexToProbabilityMap) {
							resultMatrix->addNextValue(currentIndex, targetIndex.first, targetIndex.second);
						}
					}
				}
			}

			// If the state is a deadlock state, a self-loop is inserted. Note that we do not have
			// to check whether --fix-deadlocks is set, because if this was not the case an
			// exception would have been thrown earlier.
			if (!hasTransition) {
				resultMatrix->addNextValue(currentIndex, currentIndex, 1);
			}

			++currentIndex;
		}

		// Finalize matrix and return it.
		resultMatrix->finalize();
		return resultMatrix;
	}

	void clearReachableStateSpace() {
		allStates.clear();
		stateToIndexMap.clear();
	}

	std::shared_ptr<storm::ir::Program> program;
	std::vector<StateType*> allStates;
	std::unordered_map<StateType*, uint_fast64_t, StateHash, StateCompare> stateToIndexMap;
	std::vector<storm::ir::BooleanVariable> booleanVariables;
	std::vector<storm::ir::IntegerVariable> integerVariables;
	std::unordered_map<std::string, uint_fast64_t> booleanVariableToIndexMap;
	std::unordered_map<std::string, uint_fast64_t> integerVariableToIndexMap;
	uint_fast64_t numberOfTransitions;
};

} // namespace adapters

} // namespace storm

#endif /* STORM_IR_EXPLICITMODELADAPTER_H_ */
