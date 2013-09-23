#include "src/adapters/ExplicitModelAdapter.h"

#include "src/storage/SparseMatrix.h"
#include "src/settings/Settings.h"
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

#include <algorithm>
#include <functional>
#include <boost/algorithm/string.hpp>

#include <sstream>
#include <ostream>

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

bool ExplicitModelAdapterOptionsRegistered = storm::settings::Settings::registerNewModule([] (storm::settings::Settings* instance) -> bool {
	instance->addOption(storm::settings::OptionBuilder("ExplicitModelAdapter", "constants", "", "Specifies the constant replacements to use in Explicit Models").addArgument(storm::settings::ArgumentBuilder::createStringArgument("constantString", "A comma separated list of constants and their value, e.g. a=1,b=2,c=3").setDefaultValueString("").build()).build());
	return true;
});

namespace storm {
    namespace adapters {
        
        ExplicitModelAdapter::ExplicitModelAdapter(storm::ir::Program program) : program(program), booleanVariables(),
        integerVariables(), booleanVariableToIndexMap(), integerVariableToIndexMap(), allStates(), stateToIndexMap(),
        numberOfTransitions(0), numberOfChoices(0), choiceLabeling(), transitionMap() {
            // Get variables from program.
            this->initializeVariables();
            storm::settings::Settings* s = storm::settings::Settings::getInstance();
            this->precision = s->getOptionByLongName("precision").getArgument(0).getValueAsDouble();
        }
        
        ExplicitModelAdapter::~ExplicitModelAdapter() {
            this->clearInternalState();
        }
        
        void ExplicitModelAdapter::defineUndefinedConstants(std::string const& constantDefinitionString) {
            if (!constantDefinitionString.empty()) {
                // Parse the string that defines the undefined constants of the model and make sure that it contains exactly
                // one value for each undefined constant of the model.
                std::vector<std::string> definitions;
                boost::split(definitions, constantDefinitionString, boost::is_any_of(","));
                for (auto& definition : definitions) {
                    boost::trim(definition);
                    
                    // Check whether the token could be a legal constant definition.
                    uint_fast64_t positionOfAssignmentOperator = definition.find('=');
                    if (positionOfAssignmentOperator == std::string::npos) {
                        throw storm::exceptions::InvalidArgumentException() << "Illegal constant definition string: syntax error.";
                    }
                    
                    // Now extract the variable name and the value from the string.
                    std::string constantName = definition.substr(0, positionOfAssignmentOperator);
                    boost::trim(constantName);
                    std::string value = definition.substr(positionOfAssignmentOperator + 1);
                    boost::trim(value);
                    
                    // Check whether the constant is a legal undefined constant of the program and if so, of what type it is.
                    if (program.hasUndefinedBooleanConstant(constantName)) {
                        if (value == "true") {
                            program.getUndefinedBooleanConstantExpression(constantName)->define(true);
                        } else if (value == "false") {
                            program.getUndefinedBooleanConstantExpression(constantName)->define(false);
                        } else {
                            throw storm::exceptions::InvalidArgumentException() << "Illegal value for boolean constant: " << value << ".";
                        }
                    } else if (program.hasUndefinedIntegerConstant(constantName)) {
                        try {
                            int_fast64_t integerValue = std::stoi(value);
                            program.getUndefinedIntegerConstantExpression(constantName)->define(integerValue);
                        } catch (std::invalid_argument const&) {
                            throw storm::exceptions::InvalidArgumentException() << "Illegal value of integer constant: " << value << ".";
                        } catch (std::out_of_range const&) {
                            throw storm::exceptions::InvalidArgumentException() << "Illegal value of integer constant: " << value << " (value too big).";
                        }
                    } else if (program.hasUndefinedDoubleConstant(constantName)) {
                        try {
                            double doubleValue = std::stod(value);
                            program.getUndefinedDoubleConstantExpression(constantName)->define(doubleValue);
                        } catch (std::invalid_argument const&) {
                            throw storm::exceptions::InvalidArgumentException() << "Illegal value of double constant: " << value << ".";
                        } catch (std::out_of_range const&) {
                            throw storm::exceptions::InvalidArgumentException() << "Illegal value of double constant: " << value << " (value too big).";
                        }
                        
                    } else {
                        throw storm::exceptions::InvalidArgumentException() << "Illegal constant definition string: unknown undefined constant " << constantName << ".";
                    }
                }
            }
        }
        
        void ExplicitModelAdapter::undefineUndefinedConstants() {
            for (auto nameExpressionPair : program.getBooleanUndefinedConstantExpressionsMap()) {
                nameExpressionPair.second->undefine();
            }
            for (auto nameExpressionPair : program.getIntegerUndefinedConstantExpressionsMap()) {
                nameExpressionPair.second->undefine();
            }
            for (auto nameExpressionPair : program.getDoubleUndefinedConstantExpressionsMap()) {
                nameExpressionPair.second->undefine();
            }
        }
        
        std::shared_ptr<storm::models::AbstractModel<double>> ExplicitModelAdapter::getModel(std::string const& constantDefinitionString, std::string const& rewardModelName) {
            // Start by defining the remaining constants in the model.
            this->defineUndefinedConstants(constantDefinitionString);
            
            // Initialize reward model.
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
            
            std::shared_ptr<storm::models::AbstractModel<double>> result;
            // Build and return actual model.
            switch (this->program.getModelType())
            {
                case storm::ir::Program::DTMC:
                {
                    storm::storage::SparseMatrix<double> matrix = this->buildDeterministicMatrix();
                    result = std::shared_ptr<storm::models::AbstractModel<double>>(new storm::models::Dtmc<double>(matrix, stateLabeling, stateRewards, this->transitionRewards, this->choiceLabeling));
                    break;
                }
                case storm::ir::Program::CTMC:
                {
                    storm::storage::SparseMatrix<double> matrix = this->buildDeterministicMatrix();
                    result = std::shared_ptr<storm::models::AbstractModel<double>>(new storm::models::Ctmc<double>(matrix, stateLabeling, stateRewards, this->transitionRewards, this->choiceLabeling));
                    break;
                }
                case storm::ir::Program::MDP:
                {
                    storm::storage::SparseMatrix<double> matrix = this->buildNondeterministicMatrix();
                    result = std::shared_ptr<storm::models::AbstractModel<double>>(new storm::models::Mdp<double>(matrix, stateLabeling, this->choiceIndices, stateRewards, this->transitionRewards, this->choiceLabeling));
                    break;
                }
                case storm::ir::Program::CTMDP:
                {
                    storm::storage::SparseMatrix<double> matrix = this->buildNondeterministicMatrix();
                    result = std::shared_ptr<storm::models::AbstractModel<double>>(new storm::models::Ctmdp<double>(matrix, stateLabeling, this->choiceIndices, stateRewards, this->transitionRewards, this->choiceLabeling));
                    break;
                }
                default:
                    LOG4CPLUS_ERROR(logger, "Error while creating model from probabilistic program: cannot handle this model type.");
                    throw storm::exceptions::WrongFormatException() << "Error while creating model from probabilistic program: cannot handle this model type.";
                    break;
            }
            
            // Undefine the constants so that the program can be used again somewhere else.
            undefineUndefinedConstants();
            
            return result;
        }
        
        void ExplicitModelAdapter::setValue(StateType* state, uint_fast64_t index, bool value) {
            std::get<0>(*state)[index] = value;
        }
        
        void ExplicitModelAdapter::setValue(StateType* state, uint_fast64_t index, int_fast64_t value) {
            std::get<1>(*state)[index] = value;
        }
        
        std::string ExplicitModelAdapter::toString(StateType const* state) {
            std::stringstream ss;
            for (unsigned int i = 0; i < state->first.size(); i++) ss << state->first[i] << "\t";
            for (unsigned int i = 0; i < state->second.size(); i++) ss << state->second[i] << "\t";
            return ss.str();
        }
        
        std::vector<double> ExplicitModelAdapter::getStateRewards(std::vector<storm::ir::StateReward> const& rewards) {
            std::vector<double> result(this->allStates.size());
            for (uint_fast64_t index = 0; index < this->allStates.size(); index++) {
                result[index] = 0;
                for (auto const& reward : rewards) {
                    // Add this reward to the state if the state is included in the state reward.
                    if (reward.getStatePredicate()->getValueAsBool(this->allStates[index]) == true) {
                        result[index] += reward.getRewardValue()->getValueAsDouble(this->allStates[index]);
                    }
                }
            }
            return result;
        }
        
        storm::models::AtomicPropositionsLabeling ExplicitModelAdapter::getStateLabeling(std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>> labels) {
            storm::models::AtomicPropositionsLabeling results(this->allStates.size(), labels.size() + 1);
            // Initialize labeling.
            for (auto const& labelExpressionPair : labels) {
                results.addAtomicProposition(labelExpressionPair.first);
            }
            for (uint_fast64_t index = 0; index < this->allStates.size(); index++) {
                for (auto const& labelExpressionPair: labels) {
                    // Add label to state, if the corresponding expression is true.
                    if (labelExpressionPair.second->getValueAsBool(this->allStates[index])) {
                        results.addAtomicPropositionToState(labelExpressionPair.first, index);
                    }
                }
            }
            
            // Also label the initial state with the special label "init".
            results.addAtomicProposition("init");
            StateType* initialState = this->getInitialState();
            uint_fast64_t initialIndex = this->stateToIndexMap[initialState];
            results.addAtomicPropositionToState("init", initialIndex);
            delete initialState;
            
            return results;
        }
        
        void ExplicitModelAdapter::initializeVariables() {
            uint_fast64_t numberOfIntegerVariables = 0;
            uint_fast64_t numberOfBooleanVariables = 0;
            
            // Count number of variables.
            numberOfBooleanVariables += program.getNumberOfGlobalBooleanVariables();
            numberOfIntegerVariables += program.getNumberOfGlobalIntegerVariables();
            for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
                numberOfBooleanVariables += program.getModule(i).getNumberOfBooleanVariables();
                numberOfIntegerVariables += program.getModule(i).getNumberOfIntegerVariables();
            }
            
            this->booleanVariables.resize(numberOfBooleanVariables);
            this->integerVariables.resize(numberOfIntegerVariables);
            
            // Create variables.
            for (uint_fast64_t i = 0; i < program.getNumberOfGlobalBooleanVariables(); ++i) {
                storm::ir::BooleanVariable var = program.getGlobalBooleanVariable(i);
                this->booleanVariables[var.getGlobalIndex()] = var;
                this->booleanVariableToIndexMap[var.getName()] = var.getGlobalIndex();
            }
            for (uint_fast64_t i = 0; i < program.getNumberOfGlobalIntegerVariables(); ++i) {
                storm::ir::IntegerVariable var = program.getGlobalIntegerVariable(i);
                this->integerVariables[var.getGlobalIndex()] = var;
                this->integerVariableToIndexMap[var.getName()] = var.getGlobalIndex();
            }
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
        
        boost::optional<std::vector<std::list<storm::ir::Command>>> ExplicitModelAdapter::getActiveCommandsByAction(StateType const* state, std::string const& action) {
            boost::optional<std::vector<std::list<storm::ir::Command>>> result((std::vector<std::list<storm::ir::Command>>()));
            
            // Iterate over all modules.
            for (uint_fast64_t i = 0; i < this->program.getNumberOfModules(); ++i) {
                storm::ir::Module const& module = this->program.getModule(i);
                
                // If the module has no command labeled with the given action, we can skip this module.
                if (!module.hasAction(action)) {
                    continue;
                }
                
                std::set<uint_fast64_t> const& commandIndices = module.getCommandsByAction(action);
                std::list<storm::ir::Command> commands;
                
                // Look up commands by their indices and add them if the guard evaluates to true in the given state.
                for (uint_fast64_t commandIndex : commandIndices) {
                    storm::ir::Command const& command = module.getCommand(commandIndex);
                    if (command.getGuard()->getValueAsBool(state)) {
                        commands.push_back(command);
                    }
                }
                
                // If there was no enabled command although the module has some command with the required action label,
                // we must not return anything.
                if (commands.size() == 0) {
                    return boost::optional<std::vector<std::list<storm::ir::Command>>>();
                }
                
                result.get().push_back(std::move(commands));
            }
            return result;
        }
        
        StateType* ExplicitModelAdapter::applyUpdate(StateType const* state, storm::ir::Update const& update) const {
            return this->applyUpdate(state, state, update);
        }
        
        StateType* ExplicitModelAdapter::applyUpdate(StateType const* state, StateType const* baseState, storm::ir::Update const& update) const {
            StateType* newState = new StateType(*state);
            for (auto assignedVariable : update.getBooleanAssignments()) {
                setValue(newState, this->booleanVariableToIndexMap.at(assignedVariable.first), assignedVariable.second.getExpression()->getValueAsBool(baseState));
            }
            for (auto assignedVariable : update.getIntegerAssignments()) {
                setValue(newState, this->integerVariableToIndexMap.at(assignedVariable.first), assignedVariable.second.getExpression()->getValueAsInt(baseState));
            }
            return newState;
        }
        
        StateType* ExplicitModelAdapter::getInitialState() {
            StateType* initialState = new StateType();
            initialState->first.resize(this->booleanVariables.size());
            initialState->second.resize(this->integerVariables.size());
            
            // Start with boolean variables.
            for (uint_fast64_t i = 0; i < this->booleanVariables.size(); ++i) {
                // Check if an initial value is given
                if (this->booleanVariables[i].getInitialValue().get() == nullptr) {
                    // If no initial value was given, we assume that the variable is initially false.
                    std::get<0>(*initialState)[i] = false;
                } else {
                    // Initial value was given.
                    bool initialValue = this->booleanVariables[i].getInitialValue()->getValueAsBool(nullptr);
                    std::get<0>(*initialState)[i] = initialValue;
                }
            }
            
            // Now process integer variables.
            for (uint_fast64_t i = 0; i < this->integerVariables.size(); ++i) {
                // Check if an initial value was given.
                if (this->integerVariables[i].getInitialValue().get() == nullptr) {
                    // No initial value was given, so we assume that the variable initially has the least value it can take.
                    std::get<1>(*initialState)[i] = this->integerVariables[i].getLowerBound()->getValueAsInt(nullptr);
                } else {
                    // Initial value was given.
                    int_fast64_t initialValue = this->integerVariables[i].getInitialValue()->getValueAsInt(nullptr);
                    std::get<1>(*initialState)[i] = initialValue;
                }
            }
            
            LOG4CPLUS_DEBUG(logger, "Generated initial state.");
            return initialState;
        }
        
        uint_fast64_t ExplicitModelAdapter::getOrAddStateIndex(StateType* state) {
            // Check, if the state was already registered.
            auto indexIt = this->stateToIndexMap.find(state);
            
            if (indexIt == this->stateToIndexMap.end()) {
                // The state has not been seen, yet, so add it to the list of all reachable states.
                allStates.push_back(state);
                stateToIndexMap[state] = allStates.size() - 1;
                return allStates.size() - 1;
            } else {
                // The state was already encountered. Delete the copy of the old state and return its index.
                delete state;
                return indexIt->second;
            }
        }
        
        void ExplicitModelAdapter::addUnlabeledTransitions(uint_fast64_t stateIndex, std::list<std::pair<std::pair<std::string, std::list<uint_fast64_t>>, std::map<uint_fast64_t, double>>>& transitionList) {
            StateType const* state = this->allStates[stateIndex];
            
            // Iterate over all modules.
            for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
                storm::ir::Module const& module = program.getModule(i);
            
                // Iterate over all commands.
                for (uint_fast64_t j = 0; j < module.getNumberOfCommands(); ++j) {
                    storm::ir::Command const& command = module.getCommand(j);
                    
                    // Only consider unlabeled commands.
                    if (command.getActionName() != "") continue;
                    // Skip the command, if it is not enabled.
                    if (!command.getGuard()->getValueAsBool(state)) continue;
                    
                    // Add a new choice for the state.
                    transitionList.emplace_back();
                    
                    // Add the index of the current command to the labeling of the choice.
                    transitionList.back().first.second.emplace_back(command.getGlobalIndex());
                    
                    // Create an alias for all target states for convenience.
                    std::map<uint_fast64_t, double>& targetStates = transitionList.back().second;
                    
                    // Also keep track of the total probability leaving the state.
                    double probabilitySum = 0;
                    
                    // Iterate over all updates of the current command.
                    for (uint_fast64_t k = 0; k < command.getNumberOfUpdates(); ++k) {
                        storm::ir::Update const& update = command.getUpdate(k);

                        // Obtain target state index.
                        uint_fast64_t newTargetStateIndex = this->getOrAddStateIndex(this->applyUpdate(state, update));
                        
                        probabilitySum += update.getLikelihoodExpression()->getValueAsDouble(state);
                        
                        // Check, if we already saw this state in another update and, if so, add up probabilities.
                        auto stateIt = targetStates.find(newTargetStateIndex);
                        if (stateIt == targetStates.end()) {
                            targetStates[newTargetStateIndex] = update.getLikelihoodExpression()->getValueAsDouble(state);
                            this->numberOfTransitions++;
                        } else {
                            targetStates[newTargetStateIndex] += update.getLikelihoodExpression()->getValueAsDouble(state);
                        }	
                    }
                    
                    if (std::abs(1 - probabilitySum) > this->precision) {
                        LOG4CPLUS_ERROR(logger, "Sum of update probabilities should be one for command:\n\t"  << command.toString());
                        throw storm::exceptions::WrongFormatException() << "Sum of update probabilities should be one for command:\n\t"  << command.toString();
                    }
                }
            }
        }
        
        void ExplicitModelAdapter::addLabeledTransitions(uint_fast64_t stateIndex, std::list<std::pair<std::pair<std::string, std::list<uint_fast64_t>>, std::map<uint_fast64_t, double>>>& transitionList) {
            for (std::string const& action : this->program.getActions()) {
                StateType* currentState = this->allStates[stateIndex];
                boost::optional<std::vector<std::list<storm::ir::Command>>> optionalActiveCommandLists = this->getActiveCommandsByAction(currentState, action);

                // Only process this action label, if there is at least one feasible solution.
                if (optionalActiveCommandLists) {
                    std::vector<std::list<storm::ir::Command>> const& activeCommandList = optionalActiveCommandLists.get();
                    std::vector<std::list<storm::ir::Command>::const_iterator> iteratorList(activeCommandList.size());
                    
                    // Initialize the list of iterators.
                    for (size_t i = 0; i < activeCommandList.size(); ++i) {
                        iteratorList[i] = activeCommandList[i].cbegin();
                    }
                    
                    // As long as there is one feasible combination of commands, keep on expanding it.
                    bool done = false;
                    while (!done) {
                        std::unordered_map<StateType*, double, StateHash, StateCompare>* currentTargetStates = new std::unordered_map<StateType*, double, StateHash, StateCompare>();
                        std::unordered_map<StateType*, double, StateHash, StateCompare>* newTargetStates = new std::unordered_map<StateType*, double, StateHash, StateCompare>();
                        (*currentTargetStates)[new StateType(*currentState)] = 1.0;

                        // FIXME: This does not check whether a global variable is written multiple times. While the
                        // behaviour for this is undefined anyway, a warning should be issued in that case.
                        double probabilitySum = 0;
                        for (uint_fast64_t i = 0; i < iteratorList.size(); ++i) {
                            storm::ir::Command const& command = *iteratorList[i];
                            
                            for (uint_fast64_t j = 0; j < command.getNumberOfUpdates(); ++j) {
                                storm::ir::Update const& update = command.getUpdate(j);
                                
                                for (auto const& stateProbabilityPair : *currentTargetStates) {
                                    StateType* newTargetState = this->applyUpdate(stateProbabilityPair.first, currentState, update);
                                    probabilitySum += stateProbabilityPair.second * update.getLikelihoodExpression()->getValueAsDouble(currentState);
                                    
                                    auto existingStateProbabilityPair = newTargetStates->find(newTargetState);
                                    if (existingStateProbabilityPair == newTargetStates->end()) {
                                        (*newTargetStates)[newTargetState] = stateProbabilityPair.second * update.getLikelihoodExpression()->getValueAsDouble(currentState);
                                    } else {
                                        existingStateProbabilityPair->second += stateProbabilityPair.second * update.getLikelihoodExpression()->getValueAsDouble(currentState);
                                    }
                                }
                            }
                            
                            // If there is one more command to come, shift the target states one time step back.
                            if (i < iteratorList.size() - 1) {
                                for (auto const& stateProbabilityPair : *currentTargetStates) {
                                    delete stateProbabilityPair.first;
                                }
                                delete currentTargetStates;
                                currentTargetStates = newTargetStates;
                                newTargetStates = new std::unordered_map<StateType*, double, StateHash, StateCompare>();
                            }
                        }
                        
                        // At this point, we applied all commands of the current command combination and newTargetStates
                        // contains all target states and their respective probabilities. That means we are now ready to
                        // add the choice to the list of transitions.
                        transitionList.emplace_back(std::make_pair(std::make_pair(action, std::list<uint_fast64_t>()), std::map<uint_fast64_t, double>()));
                        
                        // Add the commands that were involved in creating this distribution to the labeling.
                        for (uint_fast64_t i = 0; i < iteratorList.size(); ++i) {
                            transitionList.back().first.second.push_back(iteratorList[i]->getGlobalIndex());
                        }
                        
                        // Now create the actual distribution.
                        std::map<uint_fast64_t, double>& states = transitionList.back().second;
                        for (auto const& stateProbabilityPair : *newTargetStates) {
                            uint_fast64_t newStateIndex = this->getOrAddStateIndex(stateProbabilityPair.first);
                            states[newStateIndex] = stateProbabilityPair.second;
                        }
                        this->numberOfTransitions += states.size();
                        
                        // Dispose of the temporary maps.
                        delete currentTargetStates;
                        delete newTargetStates;
                        
                        // Now, check whether there is one more command combination to consider.
                        bool movedIterator = false;
                        for (int_fast64_t j = iteratorList.size() - 1; j >= 0; --j) {
                            ++iteratorList[j];
                            if (iteratorList[j] != activeCommandList[j].end()) {
                                movedIterator = true;
                            } else {
                                // Reset the iterator to the beginning of the list.
                                iteratorList[j] = activeCommandList[j].begin();
                            }
                        }
                        
                        done = !movedIterator;
                    }
                }
            }
        }
        
        storm::storage::SparseMatrix<double> ExplicitModelAdapter::buildDeterministicMatrix() {
            // Note: this->numberOfTransitions may be meaningless here, as we need to combine all nondeterministic
            // choices for all states into one determinstic one, which might reduce the number of transitions.
            // Hence, we compute the correct number now.
            uint_fast64_t numberOfTransitions = 0;
            for (uint_fast64_t state = 0; state < this->allStates.size(); ++state) {
                // Collect all target nodes in a set to get the number of distinct nodes.
                std::set<uint_fast64_t> set;
                for (auto const& choice : transitionMap[state]) {
                    for (auto const& targetStates : choice.second) {
                        set.insert(targetStates.first);
                    }
                }
                numberOfTransitions += set.size();
            }
            
            LOG4CPLUS_INFO(logger, "Building deterministic transition matrix: " << allStates.size() << " x " << allStates.size() << " with " << numberOfTransitions << " transitions.");
            
            this->choiceLabeling.clear();
            this->choiceLabeling.reserve(allStates.size());
            
            // Now build the actual matrix.
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
                
                // Combine nondeterministic choices into one by weighting them equally.
                std::map<uint_fast64_t, double> map;
                std::map<uint_fast64_t, double> rewardMap;
                std::list<uint_fast64_t> commandList;
                for (auto const& choice : transitionMap[state]) {
                    commandList.insert(commandList.end(), choice.first.second.begin(), choice.first.second.end());
                    for (auto const& targetStates : choice.second) {
                        map[targetStates.first] += targetStates.second;
                        if ((this->rewardModel != nullptr) && (this->rewardModel->hasTransitionRewards())) {
                            for (auto const& reward : this->rewardModel->getTransitionRewards()) {
                                if (reward.getStatePredicate()->getValueAsBool(this->allStates[state]) == true) {
                                    rewardMap[targetStates.first] += reward.getRewardValue()->getValueAsDouble(this->allStates[state]);
                                }
                            }
                        }
                    }
                }
                // Make sure that each command is only added once to the label of the transition.
                commandList.sort();
                commandList.unique();
                
                // Add the labeling for the behaviour of the current state.
                this->choiceLabeling.emplace_back(std::move(commandList));
                
                // Scale probabilities by number of choices.
                double factor = 1.0 / transitionMap[state].size();
                for (auto& targetStates : map) {
                    result.addNextValue(state, targetStates.first, targetStates.second * factor);
                    if ((this->rewardModel != nullptr) && (this->rewardModel->hasTransitionRewards())) {
                        this->transitionRewards.get().addNextValue(state, targetStates.first, rewardMap[targetStates.first] * factor);
                    }
                }
                
            }
            
            result.finalize();
            return result;
        }
        
        storm::storage::SparseMatrix<double> ExplicitModelAdapter::buildNondeterministicMatrix() {
            LOG4CPLUS_INFO(logger, "Building nondeterministic transition matrix: " << this->numberOfChoices << " x " << allStates.size() << " with " << this->numberOfTransitions << " transitions.");
            storm::storage::SparseMatrix<double> result(this->numberOfChoices, allStates.size());
            result.initialize(this->numberOfTransitions);
            if ((this->rewardModel != nullptr) && (this->rewardModel->hasTransitionRewards())) {
                this->transitionRewards.reset(storm::storage::SparseMatrix<double>(this->numberOfChoices, allStates.size()));
                this->transitionRewards.get().initialize(this->numberOfTransitions);
            }
            this->choiceIndices.clear();
            this->choiceIndices.reserve(allStates.size() + 1);
            this->choiceLabeling.clear();
            this->choiceLabeling.reserve(allStates.size());
            
            // Now build the actual matrix.
            uint_fast64_t nextRow = 0;
            this->choiceIndices.push_back(0);
            for (uint_fast64_t state = 0; state < this->allStates.size(); state++) {
                this->choiceIndices.push_back(this->choiceIndices[state] + transitionMap[state].size());
                for (auto const& choice : transitionMap[state]) {
                    this->choiceLabeling.emplace_back(std::move(choice.first.second));
                    for (auto const& targetStates : choice.second) {
                        result.addNextValue(nextRow, targetStates.first, targetStates.second);
                        if ((this->rewardModel != nullptr) && (this->rewardModel->hasTransitionRewards())) {
                            double rewardValue = 0;
                            for (auto const& reward : this->rewardModel->getTransitionRewards()) {
                                if (reward.getStatePredicate()->getValueAsBool(this->allStates[state]) == true) {
                                    rewardValue = reward.getRewardValue()->getValueAsDouble(this->allStates[state]);
                                }
                            }
                            this->transitionRewards.get().addNextValue(nextRow, targetStates.first, rewardValue);
                        }
                    }
                    ++nextRow;
                }
            }
            
            result.finalize();
            return result;
        }
        
        void ExplicitModelAdapter::buildTransitionMap() {
            LOG4CPLUS_DEBUG(logger, "Creating transition map from program.");
            this->clearInternalState();
            this->allStates.clear();
            this->allStates.push_back(this->getInitialState());
            stateToIndexMap[this->allStates[0]] = 0;
            
            for (uint_fast64_t state = 0; state < this->allStates.size(); ++state) {
                this->addUnlabeledTransitions(state, this->transitionMap[state]);
                this->addLabeledTransitions(state, this->transitionMap[state]);
                
                this->numberOfChoices += this->transitionMap[state].size();
                
                // Check whether the current state is a deadlock state and treat it accordingly.
                if (this->transitionMap[state].size() == 0) {
                    if (storm::settings::Settings::getInstance()->isSet("fixDeadlocks")) {
                        ++this->numberOfTransitions;
                        ++this->numberOfChoices;
                        this->transitionMap[state].emplace_back();
                        this->transitionMap[state].back().second[state] = 1;
                    } else {
                        LOG4CPLUS_ERROR(logger, "Error while creating sparse matrix from probabilistic program: found deadlock state. For fixing these, please provide the appropriate option.");
                        throw storm::exceptions::WrongFormatException() << "Error while creating sparse matrix from probabilistic program: found deadlock state. For fixing these, please provide the appropriate option.";
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
