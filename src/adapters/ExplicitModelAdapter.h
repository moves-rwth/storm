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
#include <set>
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

	/*!
	 * Generates all initial states and adds them to allStates.
	 */
	void generateInitialStates() {
		// Create a fresh state which can hold as many boolean and integer variables as there are.
		this->allStates.clear();
		this->allStates.push_back(new StateType());
		this->allStates[0]->first.resize(this->booleanVariables.size());
		this->allStates[0]->second.resize(this->integerVariables.size());

		// Start with boolean variables.
		for (uint_fast64_t i = 0; i < this->booleanVariables.size(); ++i) {
			// Check if an initial value is given
			if (this->booleanVariables[i].getInitialValue().get() == nullptr) {
				// No initial value was given.
				uint_fast64_t size = this->allStates.size();
				for (uint_fast64_t pos = 0; pos < size; pos++) {
					// Duplicate each state, one with true and one with false.
					this->allStates.push_back(new StateType(*this->allStates[pos]));
					std::get<0>(*this->allStates[pos])[i] = false;
					std::get<0>(*this->allStates[size + pos])[i] = true;
				}
			} else {
				// Initial value was given.
				bool initialValue = this->booleanVariables[i].getInitialValue()->getValueAsBool(this->allStates[0]);
				for (auto it : this->allStates) {
					std::get<0>(*it)[i] = initialValue;
				}
			}
		}
		// Now process integer variables.
		for (uint_fast64_t i = 0; i < this->integerVariables.size(); ++i) {
			// Check if an initial value was given.
			if (this->integerVariables[i].getInitialValue().get() == nullptr) {
				// No initial value was given.
				uint_fast64_t size = this->allStates.size();
				int_fast64_t lower = this->integerVariables[i].getLowerBound()->getValueAsInt(this->allStates[0]);
				int_fast64_t upper = this->integerVariables[i].getUpperBound()->getValueAsInt(this->allStates[0]);

				// Duplicate all states for all values in variable interval.
				for (int_fast64_t value = lower; value <= upper; value++) {
					for (uint_fast64_t pos = 0; pos < size; pos++) {
						// If value is lower bound, we reuse the existing state, otherwise we create a new one.
						if (value > lower) this->allStates.push_back(new StateType(*this->allStates[pos]));
						// Set value to current state.
						std::get<1>(*this->allStates[(value - lower) * size + pos])[i] = value;
					}
				}
			} else {
				// Initial value was given.
				int_fast64_t initialValue = this->integerVariables[i].getInitialValue()->getValueAsInt(this->allStates[0]);
				for (auto it : this->allStates) {
					std::get<1>(*it)[i] = initialValue;
				}
			}
		}
	}

	/*!
	 * Retrieves the state id of the given state.
	 * If the state has not been hit yet, it will be added to allStates and given a new id.
	 * In this case, the pointer must not be deleted, as it is used within allStates.
	 * If the state is already known, the pointer is deleted and the old state id is returned.
	 * Hence, the given state pointer should not be used afterwards.
	 * @param state Pointer to state, shall not be used afterwards.
	 * @returns State id of given state.
	 */
	uint_fast64_t getOrAddStateId(StateType * state) {
		// Check, if we already know this state at all.
		auto indexIt = this->stateToIndexMap.find(state);
		if (indexIt == this->stateToIndexMap.end()) {
			// No, add to allStates, initialize index.
			allStates.push_back(state);
			stateToIndexMap[state] = allStates.size()-1;
			return allStates.size()-1;
		} else {
			// Yes, obtain index and delete state object.
			delete state;
			return indexIt->second;
		}
	}

	/*!
	 * Expands all unlabeled transitions for a given state and adds them to the given list of results.
	 * There will be an additional map for each Command that is active.
	 * Each such map will contain a probability distribution over the reachable states using this Command.
	 * @params state State to be expanded
	 * @params res List of
	 */
	void addUnlabeledTransitions(const uint_fast64_t stateID, std::list<std::map<uint_fast64_t, double>>& res) {
		const StateType* state = this->allStates[stateID];
		// Iterate over all modules.
		for (uint_fast64_t i = 0; i < program->getNumberOfModules(); ++i) {
			storm::ir::Module const& module = program->getModule(i);
			// Iterate over all commands.
			for (uint_fast64_t j = 0; j < module.getNumberOfCommands(); ++j) {
				storm::ir::Command const& command = module.getCommand(j);
				// Only consider unlabeled commands.
				if (command.getActionName() != "") continue;
				// Omit, if command is not active.
				if (!command.getGuard()->getValueAsBool(state)) continue;

				// Add a new map and get pointer.
				res.emplace_back();
				std::map<uint_fast64_t, double>* states = &res.back();

				// Iterate over all updates.
				for (uint_fast64_t k = 0; k < command.getNumberOfUpdates(); ++k) {
					// Obtain new state id.
					storm::ir::Update const& update = command.getUpdate(k);
					uint_fast64_t newStateId = this->getOrAddStateId(this->applyUpdate(state, update));

					// Check, if we already know this state, add up probabilities for every state.
					auto stateIt = states->find(newStateId);
					if (stateIt == states->end()) {
						(*states)[newStateId] = update.getLikelihoodExpression()->getValueAsDouble(state);
						this->numberOfTransitions++;
					} else {
						(*states)[newStateId] += update.getLikelihoodExpression()->getValueAsDouble(state);
					}	
				}
			}
		}
	}

	void addLabeledTransitions(const uint_fast64_t stateID, std::list<std::map<uint_fast64_t, double>>& res) {
		// Create a copy of the current state, as we will free intermediate states...
		for (std::string action : this->program->getActions()) {
			StateType* state = new StateType(*this->allStates[stateID]);
			std::unique_ptr<std::list<std::list<storm::ir::Command>>> cmds = this->getActiveCommandsByAction(state, action);

			// Start with current state
			std::unordered_map<StateType*, double, StateHash, StateCompare> resultStates;
			resultStates[state] = 1.0;
			
			for (std::list<storm::ir::Command> module : *cmds) {
				if (resultStates.size() == 0) break;
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
						}
					}
				}
				for (auto it: resultStates) {
					delete it.first;
				}
				// Move new states to resultStates.
				resultStates.clear();
				resultStates.insert(newStates.begin(), newStates.end());

			}

			if (resultStates.size() > 0) {
				res.emplace_back();
				std::map<uint_fast64_t, double>* states = &res.back();

				// Now add our final result states to our global result.
				for (auto it : resultStates) {
					uint_fast64_t newStateID = this->getOrAddStateId(it.first);
					(*states)[newStateID] = it.second;
				}
				this->numberOfTransitions += states->size();
			}
		}

	}

	void dump(std::map<uint_fast64_t, double>& obj) {
		std::cout << "Some map:" << std::endl;
		for (auto it: obj) {
			std::cout << "\t" << it.first << ": " << it.second << std::endl;
		}
	}

	/*!
	 * Create matrix from intermediate mapping, assuming it is a dtmc model.
	 * @param intermediate Intermediate representation of transition mapping.
	 * @return result matrix.
	 */
	template<class T>
	std::shared_ptr<storm::storage::SparseMatrix<T>> buildDTMCMatrix(std::map<uint_fast64_t, std::list<std::map<uint_fast64_t, double>>> intermediate) {
		std::cout << "Building DTMC matrix" << std::endl;
		std::shared_ptr<storm::storage::SparseMatrix<T>> result(new storm::storage::SparseMatrix<T>(allStates.size()));
		uint_fast64_t numberOfTransitions = 0;
		for (uint_fast64_t state = 0; state < this->allStates.size(); state++) {
			std::set<uint_fast64_t> set;
			for (auto choice : intermediate[state]) {
				for (auto elem : choice) {
					set.insert(elem.first);
				}
			}
			numberOfTransitions += set.size();
		}
		std::cout << "number of Transitions: " << numberOfTransitions << std::endl;
		result->initialize(numberOfTransitions);
		for (uint_fast64_t state = 0; state < this->allStates.size(); state++) {
			if (intermediate[state].size() > 1) {
				std::cout << "Warning: state " << state << " has " << intermediate[state].size() << " overlapping guards in dtmc" << std::endl;
			}
			std::map<uint_fast64_t, double> map;
			for (auto choice : intermediate[state]) {
				for (auto elem : choice) {
					map[elem.first] += elem.second;
				}
			}
			double factor = 1.0 / intermediate[state].size();
			for (auto it : map) {
				result->addNextValue(state, it.first, it.second * factor);
			}

		}
		result->finalize();
		return result;
	}

	/*!
	 * Create matrix from intermediate mapping, assuming it is a mdp model.
	 * @param intermediate Intermediate representation of transition mapping.
	 * @param choices Overall number of choices for all nodes.
	 * @return result matrix.
	 */
	template<class T>
	std::shared_ptr<storm::storage::SparseMatrix<T>> buildMDPMatrix(std::map<uint_fast64_t, std::list<std::map<uint_fast64_t, double>>> intermediate, uint_fast64_t choices) {
		std::cout << "Building MDP matrix" << std::endl;
		std::shared_ptr<storm::storage::SparseMatrix<T>> result(new storm::storage::SparseMatrix<T>(allStates.size(), choices));
		result->initialize(this->numberOfTransitions);
		uint_fast64_t nextRow = 0;
		for (uint_fast64_t state = 0; state < this->allStates.size(); state++) {
			for (auto choice : intermediate[state]) {
				for (auto it : choice) {
					result->addNextValue(nextRow, it.first, it.second);
				}
				nextRow++;
			}
		}
		result->finalize();
		return result;
	}

	template<class T>
	std::shared_ptr<storm::storage::SparseMatrix<T>> buildMatrix() {
		this->prepareAuxiliaryDatastructures();
		this->allStates.clear();
		this->stateToIndexMap.clear();
		this->numberOfTransitions = 0;
		uint_fast64_t choices;
		std::map<uint_fast64_t, std::list<std::map<uint_fast64_t, double>>> intermediate;
		
		this->generateInitialStates();
		for (uint_fast64_t curIndex = 0; curIndex < this->allStates.size(); curIndex++)
		{
			this->addUnlabeledTransitions(curIndex, intermediate[curIndex]);
			this->addLabeledTransitions(curIndex, intermediate[curIndex]);

			choices += intermediate[curIndex].size();
			if (intermediate[curIndex].size() == 0) {
				// This is a deadlock state.
				if (storm::settings::instance()->isSet("fix-deadlocks")) {
					this->numberOfTransitions++;
					intermediate[curIndex].emplace_back();
					intermediate[curIndex].back()[curIndex] = 1;
				} else {
					LOG4CPLUS_ERROR(logger, "Error while creating sparse matrix from probabilistic program: found deadlock state.");
					throw storm::exceptions::WrongFileFormatException() << "Error while creating sparse matrix from probabilistic program: found deadlock state.";
				}
			}
		}

		
		switch (this->program->getModelType()) {
			case storm::ir::Program::DTMC:
			case storm::ir::Program::CTMC:
				return this->buildDTMCMatrix<T>(intermediate);
			case storm::ir::Program::MDP:
			case storm::ir::Program::CTMDP:
				return this->buildMDPMatrix<T>(intermediate, choices);
			default:
				LOG4CPLUS_ERROR(logger, "Error while creating sparse matrix from probabilistic program: We can't handle this model type.");
				throw storm::exceptions::WrongFileFormatException() << "Error while creating sparse matrix from probabilistic program: We can't handle this model type.";
				break;
		}
		return std::shared_ptr<storm::storage::SparseMatrix<T>>(nullptr);
	}

	void clearReachableStateSpace() {
		for (auto it : allStates) {
			delete it;
		}
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
