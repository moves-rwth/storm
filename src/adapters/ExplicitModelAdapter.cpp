#include "src/adapters/ExplicitModelAdapter.h"

#include "src/storage/SparseMatrix.h"
#include "src/utility/Settings.h"
#include "src/exceptions/WrongFormatException.h"

#include "src/ir/Program.h"
#include "src/ir/RewardModel.h"
#include "src/ir/StateReward.h"
#include "src/ir/TransitionReward.h"

#include "src/models/AbstractModel.h"
#include "src/models/Dtmc.h"
#include "src/models/Ctmc.h"
#include "src/models/Mdp.h"
#include "src/models/Ctmdp.h"

typedef std::pair<std::vector<bool>, std::vector<int_fast64_t>> StateType;

#include <sstream>

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {

namespace adapters {

	ExplicitModelAdapter::ExplicitModelAdapter(storm::ir::Program program) : program(program), 
			booleanVariables(), integerVariables(), booleanVariableToIndexMap(), integerVariableToIndexMap(),
			allStates(), stateToIndexMap(), numberOfTransitions(0), numberOfChoices(0), transitionMap() {
		// Get variables from program.
		this->initializeVariables();
		storm::settings::Settings* s = storm::settings::instance();
		this->precision = s->get<double>("precision");
	}

	ExplicitModelAdapter::~ExplicitModelAdapter() {
		this->clearInternalState();
	}

	std::shared_ptr<storm::models::AbstractModel<double>> ExplicitModelAdapter::getModel(std::string const & rewardModelName) {
		// Initialize rewardModel.
		this->rewardModel = nullptr;
		if (rewardModelName != "") {
			this->rewardModel = std::unique_ptr<storm::ir::RewardModel>(new storm::ir::RewardModel(this->program.getRewardModel(rewardModelName)));
		}
		
		// State expansion, build temporary map, compute transition rewards.
		this->buildTransitionMap();
		
		// Compute labeling.
		storm::models::AtomicPropositionsLabeling stateLabeling = this->getStateLabeling(this->program.getLabels());
		
		// Compute state rewards.
		boost::optional<std::vector<double>> stateRewards;
		if ((this->rewardModel != nullptr) && this->rewardModel->hasStateRewards()) {
			stateRewards.reset(this->getStateRewards(this->rewardModel->getStateRewards()));
		}

		// Build and return actual model.
		switch (this->program.getModelType())
		{
			case storm::ir::Program::DTMC:
			{
				storm::storage::SparseMatrix<double> matrix = this->buildDeterministicMatrix();
				return std::shared_ptr<storm::models::AbstractModel<double>>(new storm::models::Dtmc<double>(matrix, stateLabeling, stateRewards, this->transitionRewards));
				break;
			}
			case storm::ir::Program::CTMC:
			{
				storm::storage::SparseMatrix<double> matrix = this->buildDeterministicMatrix();
				return std::shared_ptr<storm::models::AbstractModel<double>>(new storm::models::Ctmc<double>(matrix, stateLabeling, stateRewards, this->transitionRewards));
				break;
			}
			case storm::ir::Program::MDP:
			{
				storm::storage::SparseMatrix<double> matrix = this->buildNondeterministicMatrix();
				return std::shared_ptr<storm::models::AbstractModel<double>>(new storm::models::Mdp<double>(matrix, stateLabeling, this->choiceIndices, stateRewards, this->transitionRewards));
				break;
			}
			case storm::ir::Program::CTMDP:
			{
				storm::storage::SparseMatrix<double> matrix = this->buildNondeterministicMatrix();
				return std::shared_ptr<storm::models::AbstractModel<double>>(new storm::models::Ctmdp<double>(matrix, stateLabeling, this->choiceIndices, stateRewards, this->transitionRewards));
				break;
			}
			default:
				LOG4CPLUS_ERROR(logger, "Error while creating model from probabilistic program: We can't handle this model type.");
				throw storm::exceptions::WrongFormatException() << "Error while creating model from probabilistic program: We can't handle this model type.";
				break;
		}
	}

	void ExplicitModelAdapter::setValue(StateType* const state, uint_fast64_t const  index, bool const value) {
		std::get<0>(*state)[index] = value;
	}

	void ExplicitModelAdapter::setValue(StateType* const state, uint_fast64_t const index, int_fast64_t const value) {
		std::get<1>(*state)[index] = value;
	}

	std::string ExplicitModelAdapter::toString(StateType const * const state) {
		std::stringstream ss;
		for (unsigned int i = 0; i < state->first.size(); i++) ss << state->first[i] << "\t";
		for (unsigned int i = 0; i < state->second.size(); i++) ss << state->second[i] << "\t";
		return ss.str();
	}

	std::vector<double> ExplicitModelAdapter::getStateRewards(std::vector<storm::ir::StateReward> const & rewards) {
		std::vector<double> results(this->allStates.size());
		for (uint_fast64_t index = 0; index < this->allStates.size(); index++) {
			results[index] = 0;
			for (auto reward: rewards) {
				// Add this reward to the state if the state is included in the state reward.
                if (reward.getStatePredicate()->getValueAsBool(this->allStates[index]) == true) {
                    results[index] += reward.getRewardValue()->getValueAsDouble(this->allStates[index]);
                }
			}
		}
		return results;
	}

	storm::models::AtomicPropositionsLabeling ExplicitModelAdapter::getStateLabeling(std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>> labels) {
		storm::models::AtomicPropositionsLabeling results(this->allStates.size(), labels.size());
		// Initialize labeling.
		for (auto it: labels) {
			results.addAtomicProposition(it.first);
		}
		for (uint_fast64_t index = 0; index < this->allStates.size(); index++) {
			for (auto label: labels) {
				// Add label to state, if guard is true.
				if (label.second->getValueAsBool(this->allStates[index])) {
					results.addAtomicPropositionToState(label.first, index);
				}
			}
		}
		return results;
	}
	
	void ExplicitModelAdapter::initializeVariables() {
		uint_fast64_t numberOfIntegerVariables = 0;
		uint_fast64_t numberOfBooleanVariables = 0;
		// Count number of variables.
		for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
			numberOfIntegerVariables += program.getModule(i).getNumberOfIntegerVariables();
			numberOfBooleanVariables += program.getModule(i).getNumberOfBooleanVariables();
		}

		this->booleanVariables.resize(numberOfBooleanVariables);
		this->integerVariables.resize(numberOfIntegerVariables);

		// Create variables.
		for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
			storm::ir::Module const& module = program.getModule(i);

			for (uint_fast64_t j = 0; j < module.getNumberOfBooleanVariables(); ++j) {
				storm::ir::BooleanVariable var = module.getBooleanVariable(j);
				this->booleanVariables[var.getGlobalIndex()] = var;
				this->booleanVariableToIndexMap[var.getName()] = var.getGlobalIndex();
			}
			for (uint_fast64_t j = 0; j < module.getNumberOfIntegerVariables(); ++j) {
				storm::ir::IntegerVariable var = module.getIntegerVariable(j);
				this->integerVariables[var.getGlobalIndex()] = var;
				this->integerVariableToIndexMap[var.getName()] = var.getGlobalIndex();
			}
		}
	}
	
	/*!
	 * Retrieves all active command labeled by some label, ordered by modules.
	 *
	 * This function will iterate over all modules and retrieve all commands that are labeled with the given action and active for the current state.
	 * The result will be a list of lists of commands.
	 *
	 * For each module that has appropriately labeled commands, there will be a list.
	 * If none of these commands is active, this list is empty.
	 * Note the difference between *no list* and *empty list*: Modules that produce no list are not relevant for this action while an empty list means, that it is not possible to do anything with this label.
	 * @param state Current state.
	 * @param action Action label.
	 * @return Active commands.
	 */
	std::unique_ptr<std::list<std::list<storm::ir::Command>>> ExplicitModelAdapter::getActiveCommandsByAction(StateType const * state, std::string& action) {
		std::unique_ptr<std::list<std::list<storm::ir::Command>>> res = std::unique_ptr<std::list<std::list<storm::ir::Command>>>(new std::list<std::list<storm::ir::Command>>());
		
		// Iterate over all modules.
		for (uint_fast64_t i = 0; i < this->program.getNumberOfModules(); ++i) {
			storm::ir::Module const& module = this->program.getModule(i);
			
			std::set<uint_fast64_t> const& ids = module.getCommandsByAction(action);
			if (ids.size() == 0) continue;
			std::list<storm::ir::Command> commands;
			
			// Look up commands by their id. Add, if guard holds.
			for (uint_fast64_t id : ids) {
				storm::ir::Command const& cmd = module.getCommand(id);
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
	StateType* ExplicitModelAdapter::applyUpdate(StateType const * const state, storm::ir::Update const& update) const {
		return this->applyUpdate(state, state, update);
	}

	StateType* ExplicitModelAdapter::applyUpdate(StateType const * const state, StateType const * const baseState, storm::ir::Update const& update) const {
		StateType* newState = new StateType(*state);
		for (auto assignedVariable : update.getBooleanAssignments()) {
			setValue(newState, this->booleanVariableToIndexMap.at(assignedVariable.first), assignedVariable.second.getExpression()->getValueAsBool(baseState));
		}
		for (auto assignedVariable : update.getIntegerAssignments()) {
			setValue(newState, this->integerVariableToIndexMap.at(assignedVariable.first), assignedVariable.second.getExpression()->getValueAsInt(baseState));
		}
		return newState;
	}

	/*!
	 * Generates all initial states and adds them to allStates.
	 */
	void ExplicitModelAdapter::generateInitialStates() {
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
		stateToIndexMap[this->allStates[0]] = 0;
		LOG4CPLUS_DEBUG(logger, "Generated " << this->allStates.size() << " initial states.");
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
	uint_fast64_t ExplicitModelAdapter::getOrAddStateId(StateType * state) {
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
	 * @params state State to be explored.
	 * @params res Intermediate transition map.
	 */
	void ExplicitModelAdapter::addUnlabeledTransitions(const uint_fast64_t stateID, std::list<std::pair<std::string, std::map<uint_fast64_t, double>>>& res) {
		const StateType* state = this->allStates[stateID];
		// Iterate over all modules.
		for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
			storm::ir::Module const& module = program.getModule(i);
			// Iterate over all commands.
			for (uint_fast64_t j = 0; j < module.getNumberOfCommands(); ++j) {
				storm::ir::Command const& command = module.getCommand(j);
				// Only consider unlabeled commands.
				if (command.getActionName() != "") continue;
				// Omit, if command is not active.
				if (!command.getGuard()->getValueAsBool(state)) continue;

				// Add a new map and get pointer.
				res.emplace_back();
				std::map<uint_fast64_t, double>* states = &res.back().second;
				double probSum = 0;

				// Iterate over all updates.
				for (uint_fast64_t k = 0; k < command.getNumberOfUpdates(); ++k) {
					// Obtain new state id.
					storm::ir::Update const& update = command.getUpdate(k);
					uint_fast64_t newStateId = this->getOrAddStateId(this->applyUpdate(state, update));

					probSum += update.getLikelihoodExpression()->getValueAsDouble(state);
					// Check, if we already know this state, add up probabilities for every state.
					auto stateIt = states->find(newStateId);
					if (stateIt == states->end()) {
						(*states)[newStateId] = update.getLikelihoodExpression()->getValueAsDouble(state);
						this->numberOfTransitions++;
					} else {
						(*states)[newStateId] += update.getLikelihoodExpression()->getValueAsDouble(state);
					}	
				}
				if (std::abs(1 - probSum) > this->precision) {
					LOG4CPLUS_ERROR(logger, "Sum of update probabilities should be one for command:\n\t"  << command.toString());
					throw storm::exceptions::WrongFormatException() << "Sum of update probabilities should be one for command:\n\t"  << command.toString();
				}
			}
		}
	}

	/*!
	 * Explores reachable state from given state by using labeled transitions.
	 * Found transitions are stored in given map.
	 * @param stateID State to be explored.
	 * @param res Intermediate transition map.
	 */
	void ExplicitModelAdapter::addLabeledTransitions(const uint_fast64_t stateID, std::list<std::pair<std::string, std::map<uint_fast64_t, double>>>& res) {
		// Create a copy of the current state, as we will free intermediate states...
		for (std::string action : this->program.getActions()) {
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
					double probSum = 0;
					for (uint_fast64_t k = 0; k < command.getNumberOfUpdates(); ++k) {
						storm::ir::Update const update = command.getUpdate(k);

						// Iterate over all resultStates.
						for (auto it : resultStates) {
							// Apply the new update and get resulting state.
							StateType* newState = this->applyUpdate(it.first, this->allStates[stateID], update);
							probSum += update.getLikelihoodExpression()->getValueAsDouble(it.first);
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
					if (std::abs(1 - probSum) > this->precision) {
						LOG4CPLUS_ERROR(logger, "Sum of update probabilities should be one for command:\n\t"  << command.toString());
						throw storm::exceptions::WrongFormatException() << "Sum of update probabilities should be one for command:\n\t"  << command.toString();
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
				res.push_back(std::make_pair(action, std::map<uint_fast64_t, double>()));
				std::map<uint_fast64_t, double>* states = &res.back().second;

				// Now add our final result states to our global result.
				for (auto it : resultStates) {
					uint_fast64_t newStateID = this->getOrAddStateId(it.first);
					(*states)[newStateID] = it.second;
				}
				this->numberOfTransitions += states->size();
			}
		}

	}

	/*!
	 * Create matrix from intermediate mapping, assuming it is a dtmc model.
	 * @param intermediate Intermediate representation of transition mapping.
	 * @return result matrix.
	 */
	storm::storage::SparseMatrix<double> ExplicitModelAdapter::buildDeterministicMatrix() {
		// ***** ATTENTION *****
		// this->numberOfTransitions is meaningless, as we combine all choices into one for each state.
		// Hence, we compute the correct number of transitions now.
		uint_fast64_t numberOfTransitions = 0;
		for (uint_fast64_t state = 0; state < this->allStates.size(); state++) {
			// Collect all target nodes in a set to get number of distinct nodes.
			std::set<uint_fast64_t> set;
			for (auto choice : transitionMap[state]) {
				for (auto elem : choice.second) {
					set.insert(elem.first);
				}
			}
			numberOfTransitions += set.size();
		}
		LOG4CPLUS_INFO(logger, "Building deterministic transition matrix: " << allStates.size() << " x " << allStates.size() << " with " << numberOfTransitions << " transitions.");
		// Now build matrix.

		storm::storage::SparseMatrix<double> result(allStates.size());
		result.initialize(numberOfTransitions);
		if ((this->rewardModel != nullptr) && (this->rewardModel->hasTransitionRewards())) {
			this->transitionRewards.reset(std::move(storm::storage::SparseMatrix<double>(allStates.size())));
			this->transitionRewards.get().initialize(numberOfTransitions);
		}
		for (uint_fast64_t state = 0; state < this->allStates.size(); state++) {
			if (transitionMap[state].size() > 1) {
				LOG4CPLUS_WARN(logger, "State " << state << " has " << transitionMap[state].size() << " overlapping guards in deterministic model.");
			}
			// Combine choices to one map.
			std::map<uint_fast64_t, double> map;
			std::map<uint_fast64_t, double> rewardMap;
			for (auto choice : transitionMap[state]) {
				for (auto elem : choice.second) {
					map[elem.first] += elem.second;
					if ((this->rewardModel != nullptr) && (this->rewardModel->hasTransitionRewards())) {
						for (auto reward : this->rewardModel->getTransitionRewards()) {
                            if (reward.getStatePredicate()->getValueAsBool(this->allStates[state]) == true) {
                                rewardMap[elem.first] += reward.getRewardValue()->getValueAsDouble(this->allStates[state]);
                            }
						}
					}
				}
			}
			// Scale probabilities by number of choices.
			double factor = 1.0 / transitionMap[state].size();
			for (auto it : map) {
				result.addNextValue(state, it.first, it.second * factor);
				if ((this->rewardModel != nullptr) && (this->rewardModel->hasTransitionRewards())) {
					this->transitionRewards.get().addNextValue(state, it.first, rewardMap[it.first] * factor);
				}
			}

		}
		result.finalize();
		return result;
	}

	/*!
	 * Create matrix from intermediate mapping, assuming it is a mdp model.
	 * @param intermediate Intermediate representation of transition mapping.
	 * @param choices Overall number of choices for all nodes.
	 * @return result matrix.
	 */
	storm::storage::SparseMatrix<double> ExplicitModelAdapter::buildNondeterministicMatrix() {
		LOG4CPLUS_INFO(logger, "Building nondeterministic transition matrix: " << this->numberOfChoices << " x " << allStates.size() << " with " << this->numberOfTransitions << " transitions.");
		storm::storage::SparseMatrix<double> result(this->numberOfChoices, allStates.size());
		result.initialize(this->numberOfTransitions);
		if ((this->rewardModel != nullptr) && (this->rewardModel->hasTransitionRewards())) {
			this->transitionRewards.reset(storm::storage::SparseMatrix<double>(this->numberOfChoices, allStates.size()));
			this->transitionRewards.get().initialize(this->numberOfTransitions);
		}
		this->choiceIndices.clear();
		this->choiceIndices.reserve(allStates.size());
		// Build matrix.
		uint_fast64_t nextRow = 0;
		for (uint_fast64_t state = 0; state < this->allStates.size(); state++) {
			this->choiceIndices.push_back(transitionMap[state].size());
			for (auto choice : transitionMap[state]) {
				for (auto it : choice.second) {
					result.addNextValue(nextRow, it.first, it.second);
					if ((this->rewardModel != nullptr) && (this->rewardModel->hasTransitionRewards())) {
						double rewardValue = 0;
						for (auto reward : this->rewardModel->getTransitionRewards()) {
                            if (reward.getStatePredicate()->getValueAsBool(this->allStates[state]) == true) {
                                rewardValue = reward.getRewardValue()->getValueAsDouble(this->allStates[state]);
                            }
						}
						this->transitionRewards.get().addNextValue(nextRow, it.first, rewardValue);
					}
				}
				nextRow++;
			}
		}
		result.finalize();
		return result;
	}

	/*!
	 * Build matrix from model. Starts with all initial states and explores the reachable state space.
	 * While exploring, the transitions are stored in a temporary map.
	 * Afterwards, we transform this map into the actual matrix.
	 * @return result matrix.
	 */
	void ExplicitModelAdapter::buildTransitionMap() {
		LOG4CPLUS_DEBUG(logger, "Starting to create transition map from program...");
		this->clearInternalState();
		
		this->generateInitialStates();
		for (uint_fast64_t curIndex = 0; curIndex < this->allStates.size(); curIndex++)
		{
			this->addUnlabeledTransitions(curIndex, this->transitionMap[curIndex]);
			this->addLabeledTransitions(curIndex, this->transitionMap[curIndex]);

			this->numberOfChoices += this->transitionMap[curIndex].size();
			if (this->transitionMap[curIndex].size() == 0) {
				// This is a deadlock state.
				if (storm::settings::instance()->isSet("fix-deadlocks")) {
					this->numberOfTransitions++;
					this->numberOfChoices++;
					this->transitionMap[curIndex].emplace_back();
					this->transitionMap[curIndex].back().second[curIndex] = 1;
				} else {
					LOG4CPLUS_ERROR(logger, "Error while creating sparse matrix from probabilistic program: found deadlock state.");
					throw storm::exceptions::WrongFormatException() << "Error while creating sparse matrix from probabilistic program: found deadlock state.";
				}
			}
		}
		LOG4CPLUS_DEBUG(logger, "Finished creating transition map.");
	}

	void ExplicitModelAdapter::clearInternalState() {
		for (auto it : allStates) {
			delete it;
		}
		allStates.clear();
		stateToIndexMap.clear();
		this->numberOfTransitions = 0;
		this->numberOfChoices = 0;
		this->choiceIndices.clear();
		this->transitionRewards.reset();
		this->transitionMap.clear();
	}

} // namespace adapters

} // namespace storm
