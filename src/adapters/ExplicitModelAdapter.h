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
#include <list>

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

		this->booleanVariables.resize(numberOfBooleanVariables);
		this->integerVariables.resize(numberOfIntegerVariables);

		uint_fast64_t nextBooleanVariableIndex = 0;
		uint_fast64_t nextIntegerVariableIndex = 0;
		for (uint_fast64_t i = 0; i < program->getNumberOfModules(); ++i) {
			storm::ir::Module const& module = program->getModule(i);

			for (uint_fast64_t j = 0; j < module.getNumberOfBooleanVariables(); ++j) {
				this->booleanVariables[nextBooleanVariableIndex] = module.getBooleanVariable(j);
				this->booleanVariableToIndexMap[module.getBooleanVariable(j).getName()] = nextBooleanVariableIndex;
				++nextBooleanVariableIndex;
			}
			for (uint_fast64_t j = 0; j < module.getNumberOfIntegerVariables(); ++j) {
				this->integerVariables[nextIntegerVariableIndex] = module.getIntegerVariable(j);
				this->integerVariableToIndexMap[module.getIntegerVariable(j).getName()] = nextIntegerVariableIndex;
				++nextIntegerVariableIndex;
			}
		}
	}
	
	std::unique_ptr<std::list<std::list<storm::ir::Command>>> getActiveCommandsByAction(StateType const * state, std::string& action) {
		std::unique_ptr<std::list<std::list<storm::ir::Command>>> res = std::unique_ptr<std::list<std::list<storm::ir::Command>>>(new std::list<std::list<storm::ir::Command>>());
		
		// Iterate over all modules.
		for (uint_fast64_t i = 0; i < this->program->getNumberOfModules(); ++i) {
			storm::ir::Module const& module = this->program->getModule(i);
			
			std::shared_ptr<std::set<uint_fast64_t>> ids = module.getCommandsByAction(action);
			std::list<storm::ir::Command> commands;
			
			// Look up commands by their id. Add, if guard holds.
			for (uint_fast64_t id : *ids) {
				storm::ir::Command cmd = module.getCommand(id);
				if (cmd.getGuard()->getValueAsBool(state)) {
					commands.push_back(module.getCommand(id));
				}
			}
			res->push_back(commands);
		}
		// Sort the result in the vague hope that having small lists at the beginning will speed up the expanding.
		// This is how lambdas may look like in C++...
		res->sort([](const std::list<storm::ir::Command>& a, const std::list<storm::ir::Command>& b){ return a.size() < b.size(); });
		return res;
	}
	
	/*!
	 * Apply an update to the given state and return resulting state.
	 * @params state Current state.
	 * @params update Update to be applied.
	 * @return Resulting state.
	 */
	StateType* applyUpdate(StateType const * const state, storm::ir::Update const & update) {
		StateType* newState = new StateType(*state);
		for (auto assignedVariable : update.getBooleanAssignments()) {
			setValue(newState, this->booleanVariableToIndexMap[assignedVariable.first], assignedVariable.second.getExpression()->getValueAsBool(state));
		}
		for (auto assignedVariable : update.getIntegerAssignments()) {
			setValue(newState, this->integerVariableToIndexMap[assignedVariable.first], assignedVariable.second.getExpression()->getValueAsInt(state));
		}
		return newState;
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
			bool initialValue = this->booleanVariables[i].getInitialValue()->getValueAsBool(initialState);
			std::get<0>(*initialState)[i] = initialValue;
		}
		for (uint_fast64_t i = 0; i < integerVariables.size(); ++i) {
			int_fast64_t initialValue = this->integerVariables[i].getInitialValue()->getValueAsInt(initialState);
			std::get<1>(*initialState)[i] = initialValue;
		}

		// Now set up the environment for a breadth-first search starting from the initial state.
		uint_fast64_t nextIndex = 1;
		this->allStates.clear();
		this->stateToIndexMap.clear();
		std::queue<StateType*> stateQueue;

		this->allStates.push_back(initialState);
		stateQueue.push(initialState);
		this->stateToIndexMap[initialState] = 0;

		this->numberOfTransitions = 0;
		while (!stateQueue.empty()) {
			// Get first state in queue.
			StateType* currentState = stateQueue.front();
			stateQueue.pop();

			// Remember whether the state has at least one transition such that transitions can be
			// inserted upon detection of a deadlock state.
			bool hasTransition = false;
			
			// First expand all transitions for commands labelled with some
			// action. For every module, we determine all commands with this
			// action whose guard holds. Then, we add a transition for each
			// combination of all updates of those commands.
			for (std::string action : this->program->getActions()) {
				// Get list of all commands.
				// This list contains a list for every module that has commands labelled by action.
				// Each such list contains all commands whose guards are fulfilled.
				// If no guard is fulfilled (i.e. there is no way to perform this action), the list will be empty!
				std::unique_ptr<std::list<std::list<storm::ir::Command>>> cmds = this->getActiveCommandsByAction(currentState, action);
				
				// Start with current state.
				std::unordered_map<StateType*, double, StateHash, StateCompare> resultStates;
				resultStates[currentState] = 1;
				std::queue<StateType*> deleteQueue;
				
				// Iterate over all modules (represented by the list of commands with the current action).
				// We will now combine every state in resultStates with every additional update in the next module.
				// The final result will be this map after we are done with all modules.
				for (std::list<storm::ir::Command> module : *cmds) {
					// If no states are left, we are done.
					// This happens, if there is a module where commands existed, but no guard was fulfilled.
					if (resultStates.size() == 0) break;
					// Put all new states in this new map.
					std::unordered_map<StateType*, double, StateHash, StateCompare> newStates;
					
					// Iterate over all commands within this module.
					for (storm::ir::Command command : module) {
						// Iterate over all updates of this command.
						for (uint_fast64_t k = 0; k < command.getNumberOfUpdates(); ++k) {
							storm::ir::Update const& update = command.getUpdate(k);
							
							// Iterate over all resultStates.
							for (auto it : resultStates) {
								// Apply the new update and get resulting state.
								StateType* newState = this->applyUpdate(it.first, update);
								// Insert the new state into newStates array.
								// Take care of calculation of likelihood, combine identical states.
								auto s = newStates.find(newState);
								if (s == newStates.end()) {
									newStates[newState] = it.second * update.getLikelihoodExpression()->getValueAsDouble(it.first);
								} else {
									newStates[newState] += it.second * update.getLikelihoodExpression()->getValueAsDouble(it.first);
								}
								// No matter what happened, we must delete the states of the previous iteration.
								deleteQueue.push(it.first);
							} 
						}
					}
					// Move new states to resultStates.
					resultStates.clear();
					resultStates.insert(newStates.begin(), newStates.end());
					// Delete old result states.
					while (!deleteQueue.empty()) {
						if (deleteQueue.front() != currentState) {
							delete deleteQueue.front();
						}
						deleteQueue.pop();
					}
				}
				// Now add our final result states to our global result.
				for (auto it : resultStates) {
					hasTransition = true;
					auto s = stateToIndexMap.find(it.first);
					if (s == stateToIndexMap.end()) {
						stateQueue.push(it.first);
						// Add state to list of all reachable states.
						allStates.push_back(it.first);
						// Give a unique index to the newly found state.
						stateToIndexMap[it.first] = nextIndex;
						++nextIndex;
					} else {
						deleteQueue.push(it.first);
					}
				}
				// Delete states we already had.
				while (!deleteQueue.empty()) {
					delete deleteQueue.front();
					deleteQueue.pop();
				}
				this->numberOfTransitions += resultStates.size();
			}
			

			// Now, expand all transitions for commands not labelled with
			// some action. Those commands each build a transition for
			// themselves.
			// Iterate over all modules.
			for (uint_fast64_t i = 0; i < program->getNumberOfModules(); ++i) {
				storm::ir::Module const& module = program->getModule(i);

				// Iterate over all commands.
				for (uint_fast64_t j = 0; j < module.getNumberOfCommands(); ++j) {
					storm::ir::Command const& command = module.getCommand(j);
					if (command.getActionName() != "") continue;

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
							StateType* newState = this->applyUpdate(currentState, update);

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
			
			std::map<uint_fast64_t, double> stateIndexToProbabilityMap;
			
			for (std::string action : this->program->getActions()) {
				std::unique_ptr<std::list<std::list<storm::ir::Command>>> cmds = this->getActiveCommandsByAction(currentState, action);
				std::unordered_map<StateType*, double, StateHash, StateCompare> resultStates;
				resultStates[currentState] = 1;
				std::queue<StateType*> deleteQueue;
				
				for (std::list<storm::ir::Command> module : *cmds) {
					std::unordered_map<StateType*, double, StateHash, StateCompare> newStates;
					for (storm::ir::Command command : module) {
						for (uint_fast64_t k = 0; k < command.getNumberOfUpdates(); ++k) {
							storm::ir::Update const& update = command.getUpdate(k);
							for (auto it : resultStates) {
								StateType* newState = this->applyUpdate(it.first, update);
								auto s = newStates.find(newState);
                                if (s == newStates.end()) {
                                    newStates[newState] = it.second * update.getLikelihoodExpression()->getValueAsDouble(it.first);
                                } else {
                                    newStates[newState] += it.second * update.getLikelihoodExpression()->getValueAsDouble(it.first);
                                }
                                deleteQueue.push(it.first);
							}
						}
					}
					resultStates.clear();
					resultStates.insert(newStates.begin(), newStates.end());
					while (!deleteQueue.empty()) {
                        if (deleteQueue.front() != currentState) {
                            delete deleteQueue.front();
                        }
                        deleteQueue.pop();
                    }
				}
				for (auto it : resultStates) {
					hasTransition = true;
					uint_fast64_t targetIndex = stateToIndexMap[it.first];
					auto s = stateIndexToProbabilityMap.find(targetIndex);
					if (s == stateIndexToProbabilityMap.end()) {
                        stateIndexToProbabilityMap[targetIndex] = it.second;
                    } else {
                    	stateIndexToProbabilityMap[targetIndex] += it.second;
                        deleteQueue.push(it.first);
                    }
				}
				while (!deleteQueue.empty()) {  
                    delete deleteQueue.front(); 
                    deleteQueue.pop();
                }
			}
			for (auto targetIndex : stateIndexToProbabilityMap) {
				resultMatrix->addNextValue(currentIndex, targetIndex.first, targetIndex.second);
			}

			// Iterate over all modules.
			for (uint_fast64_t i = 0; i < program->getNumberOfModules(); ++i) {
				storm::ir::Module const& module = program->getModule(i);

				// Iterate over all commands.
				for (uint_fast64_t j = 0; j < module.getNumberOfCommands(); ++j) {
					storm::ir::Command const& command = module.getCommand(j);
					if (command.getActionName() != "") continue;

					// Check if this command is enabled in the current state.
					if (command.getGuard()->getValueAsBool(currentState)) {
						hasTransition = true;
						std::map<uint_fast64_t, double> stateIndexToProbabilityMap;
						for (uint_fast64_t k = 0; k < command.getNumberOfUpdates(); ++k) {
							storm::ir::Update const& update = command.getUpdate(k);

							StateType* newState = this->applyUpdate(currentState, update);

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
