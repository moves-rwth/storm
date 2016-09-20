#include "src/generator/JaniNextStateGenerator.h"

#include "src/models/sparse/StateLabeling.h"

#include "src/storage/expressions/SimpleValuation.h"

#include "src/solver/SmtSolver.h"

#include "src/utility/constants.h"
#include "src/utility/macros.h"
#include "src/utility/solver.h"
#include "src/exceptions/InvalidSettingsException.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace generator {
        
        template<typename ValueType, typename StateType>
        JaniNextStateGenerator<ValueType, StateType>::JaniNextStateGenerator(storm::jani::Model const& model, NextStateGeneratorOptions const& options) : JaniNextStateGenerator(model.substituteConstants(), options, false) {
            // Intentionally left empty.
        }
        
        template<typename ValueType, typename StateType>
        JaniNextStateGenerator<ValueType, StateType>::JaniNextStateGenerator(storm::jani::Model const& model, NextStateGeneratorOptions const& options, bool flag) : NextStateGenerator<ValueType, StateType>(model.getExpressionManager(), options), model(model), rewardVariables(), hasStateActionRewards(false) {
            STORM_LOG_THROW(model.hasStandardComposition(), storm::exceptions::WrongFormatException, "The explicit next-state generator currently does not support custom system compositions.");
            STORM_LOG_THROW(!model.hasNonGlobalTransientVariable(), storm::exceptions::InvalidSettingsException, "The explicit next-state generator currently does not support automata-local transient variables.");
            STORM_LOG_THROW(!this->options.isBuildChoiceLabelsSet(), storm::exceptions::InvalidSettingsException, "JANI next-state generator cannot generate choice labels.");
            
            // Only after checking validity of the program, we initialize the variable information.
            this->checkValid();
            this->variableInformation = VariableInformation(model);
            
            if (this->options.isBuildAllRewardModelsSet()) {
                for (auto const& variable : model.getGlobalVariables()) {
                    if (variable.isTransient()) {
                        rewardVariables.push_back(variable.getExpressionVariable());
                    }
                }
            } else {
                // Extract the reward models from the program based on the names we were given.
                auto const& globalVariables = model.getGlobalVariables();
                for (auto const& rewardModelName : this->options.getRewardModelNames()) {
                    if (globalVariables.hasVariable(rewardModelName)) {
                        rewardVariables.push_back(globalVariables.getVariable(rewardModelName).getExpressionVariable());
                    } else {
                        STORM_LOG_THROW(rewardModelName.empty(), storm::exceptions::InvalidArgumentException, "Cannot build unknown reward model '" << rewardModelName << "'.");
                        STORM_LOG_THROW(globalVariables.getNumberOfTransientVariables() == 1, storm::exceptions::InvalidArgumentException, "Reference to standard reward model is ambiguous.");
                    }
                }
                
                // If no reward model was yet added, but there was one that was given in the options, we try to build the
                // standard reward model.
                if (rewardVariables.empty() && !this->options.getRewardModelNames().empty()) {
                    rewardVariables.push_back(globalVariables.getTransientVariables().front()->getExpressionVariable());
                }
            }
            
            // Build the information structs for the reward models.
            buildRewardModelInformation();
            
            // If there are terminal states we need to handle, we now need to translate all labels to expressions.
            if (this->options.hasTerminalStates()) {
                for (auto const& expressionOrLabelAndBool : this->options.getTerminalStates()) {
                    if (expressionOrLabelAndBool.first.isExpression()) {
                        this->terminalStates.push_back(std::make_pair(expressionOrLabelAndBool.first.getExpression(), expressionOrLabelAndBool.second));
                    } else {
                        STORM_LOG_THROW(expressionOrLabelAndBool.first.getLabel() == "init" || expressionOrLabelAndBool.first.getLabel() == "deadlock", storm::exceptions::InvalidSettingsException, "Terminal states refer to illegal label '" << expressionOrLabelAndBool.first.getLabel() << "'.");
                    }
                }
            }
        }
        
        template<typename ValueType, typename StateType>
        ModelType JaniNextStateGenerator<ValueType, StateType>::getModelType() const {
            switch (model.getModelType()) {
                case storm::jani::ModelType::DTMC: return ModelType::DTMC;
                case storm::jani::ModelType::CTMC: return ModelType::CTMC;
                case storm::jani::ModelType::MDP: return ModelType::MDP;
                case storm::jani::ModelType::MA: return ModelType::MA;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Invalid model type.");
            }
        }
        
        template<typename ValueType, typename StateType>
        bool JaniNextStateGenerator<ValueType, StateType>::isDeterministicModel() const {
            return model.isDeterministicModel();
        }
        
        template<typename ValueType, typename StateType>
        bool JaniNextStateGenerator<ValueType, StateType>::isDiscreteTimeModel() const {
            return model.isDiscreteTimeModel();
        }
        
        template<typename ValueType, typename StateType>
        uint64_t JaniNextStateGenerator<ValueType, StateType>::getLocation(CompressedState const& state, LocationVariableInformation const& locationVariable) const {
            if (locationVariable.bitWidth == 0) {
                return 0;
            } else {
                return state.getAsInt(locationVariable.bitOffset, locationVariable.bitWidth);
            }
        }
        
        template<typename ValueType, typename StateType>
        void JaniNextStateGenerator<ValueType, StateType>::setLocation(CompressedState& state, LocationVariableInformation const& locationVariable, uint64_t locationIndex) const {
            if (locationVariable.bitWidth != 0) {
                state.setFromInt(locationVariable.bitOffset, locationVariable.bitWidth, locationIndex);
            }
        }
        
        template<typename ValueType, typename StateType>
        std::vector<uint64_t> JaniNextStateGenerator<ValueType, StateType>::getLocations(CompressedState const& state) const {
            std::vector<uint64_t> result(this->variableInformation.locationVariables.size());
            
            auto resultIt = result.begin();
            for (auto it = this->variableInformation.locationVariables.begin(), ite = this->variableInformation.locationVariables.end(); it != ite; ++it, ++resultIt) {
                *resultIt = getLocation(state, *it);
            }
            
            return result;
        }
        
        template<typename ValueType, typename StateType>
        std::vector<StateType> JaniNextStateGenerator<ValueType, StateType>::getInitialStates(StateToIdCallback const& stateToIdCallback) {
            // Prepare an SMT solver to enumerate all initial states.
            storm::utility::solver::SmtSolverFactory factory;
            std::unique_ptr<storm::solver::SmtSolver> solver = factory.create(model.getExpressionManager());
            
            std::vector<storm::expressions::Expression> rangeExpressions = model.getAllRangeExpressions();
            for (auto const& expression : rangeExpressions) {
                solver->add(expression);
            }
            solver->add(model.getInitialStatesExpression(true));
            
            // Proceed as long as the solver can still enumerate initial states.
            std::vector<StateType> initialStateIndices;
            while (solver->check() == storm::solver::SmtSolver::CheckResult::Sat) {
                // Create fresh state.
                CompressedState initialState(this->variableInformation.getTotalBitOffset());
                
                // Read variable assignment from the solution of the solver. Also, create an expression we can use to
                // prevent the variable assignment from being enumerated again.
                storm::expressions::Expression blockingExpression;
                std::shared_ptr<storm::solver::SmtSolver::ModelReference> model = solver->getModel();
                for (auto const& booleanVariable : this->variableInformation.booleanVariables) {
                    bool variableValue = model->getBooleanValue(booleanVariable.variable);
                    storm::expressions::Expression localBlockingExpression = variableValue ? !booleanVariable.variable : booleanVariable.variable;
                    blockingExpression = blockingExpression.isInitialized() ? blockingExpression || localBlockingExpression : localBlockingExpression;
                    initialState.set(booleanVariable.bitOffset, variableValue);
                }
                for (auto const& integerVariable : this->variableInformation.integerVariables) {
                    int_fast64_t variableValue = model->getIntegerValue(integerVariable.variable);
                    storm::expressions::Expression localBlockingExpression = integerVariable.variable != model->getManager().integer(variableValue);
                    blockingExpression = blockingExpression.isInitialized() ? blockingExpression || localBlockingExpression : localBlockingExpression;
                    initialState.setFromInt(integerVariable.bitOffset, integerVariable.bitWidth, static_cast<uint_fast64_t>(variableValue - integerVariable.lowerBound));
                }
                
                // Gather iterators to the initial locations of all the automata.
                std::vector<std::set<uint64_t>::const_iterator> initialLocationsIterators;
                uint64_t currentLocationVariable = 0;
                for (auto const& automaton : this->model.getAutomata()) {
                    initialLocationsIterators.push_back(automaton.getInitialLocationIndices().cbegin());
                    
                    // Initialize the locations to the first possible combination.
                    setLocation(initialState, this->variableInformation.locationVariables[currentLocationVariable], *initialLocationsIterators.back());
                    ++currentLocationVariable;
                }
                
                // Now iterate through all combinations of initial locations.
                while (true) {
                    // Register initial state.
                    StateType id = stateToIdCallback(initialState);
                    initialStateIndices.push_back(id);
                    
                    uint64_t index = 0;
                    for (; index < initialLocationsIterators.size(); ++index) {
                        ++initialLocationsIterators[index];
                        if (initialLocationsIterators[index] == this->model.getAutomata()[index].getInitialLocationIndices().cend()) {
                            initialLocationsIterators[index] = this->model.getAutomata()[index].getInitialLocationIndices().cbegin();
                        } else {
                            break;
                        }
                    }
                    
                    // If we are at the end, leave the loop. Otherwise, create the next initial state.
                    if (index == initialLocationsIterators.size()) {
                        break;
                    } else {
                        for (uint64_t j = 0; j <= index; ++j) {
                            setLocation(initialState, this->variableInformation.locationVariables[j], *initialLocationsIterators[j]);
                        }
                    }
                }
                
                // Block the current initial state to search for the next one.
                if (!blockingExpression.isInitialized()) {
                    break;
                }
                solver->add(blockingExpression);
            }
            
            return initialStateIndices;
        }
        
        template<typename ValueType, typename StateType>
        CompressedState JaniNextStateGenerator<ValueType, StateType>::applyUpdate(CompressedState const& state, storm::jani::EdgeDestination const& destination) {
            CompressedState newState(state);
            
            auto assignmentIt = destination.getAssignments().getNonTransientAssignments().begin();
            auto assignmentIte = destination.getAssignments().getNonTransientAssignments().end();
            
            // Iterate over all boolean assignments and carry them out.
            auto boolIt = this->variableInformation.booleanVariables.begin();
            for (; assignmentIt != assignmentIte && assignmentIt->getAssignedExpression().hasBooleanType(); ++assignmentIt) {
                while (assignmentIt->getExpressionVariable() != boolIt->variable) {
                    ++boolIt;
                }
                newState.set(boolIt->bitOffset, this->evaluator.asBool(assignmentIt->getAssignedExpression()));
            }
            
            // Iterate over all integer assignments and carry them out.
            auto integerIt = this->variableInformation.integerVariables.begin();
            for (; assignmentIt != assignmentIte && assignmentIt->getAssignedExpression().hasIntegerType(); ++assignmentIt) {
                while (assignmentIt->getExpressionVariable() != integerIt->variable) {
                    ++integerIt;
                }
                int_fast64_t assignedValue = this->evaluator.asInt(assignmentIt->getAssignedExpression());
                STORM_LOG_THROW(assignedValue <= integerIt->upperBound, storm::exceptions::WrongFormatException, "The update " << assignmentIt->getExpressionVariable().getName() << " := " << assignmentIt->getAssignedExpression() << " leads to an out-of-bounds value (" << assignedValue << ") for the variable '" << assignmentIt->getExpressionVariable().getName() << "'.");
                newState.setFromInt(integerIt->bitOffset, integerIt->bitWidth, assignedValue - integerIt->lowerBound);
                STORM_LOG_ASSERT(static_cast<int_fast64_t>(newState.getAsInt(integerIt->bitOffset, integerIt->bitWidth)) + integerIt->lowerBound == assignedValue, "Writing to the bit vector bucket failed (read " << newState.getAsInt(integerIt->bitOffset, integerIt->bitWidth) << " but wrote " << assignedValue << ").");
            }
            
            // Check that we processed all assignments.
            STORM_LOG_ASSERT(assignmentIt == assignmentIte, "Not all assignments were consumed.");
            
            return newState;
        }
        
        template<typename ValueType, typename StateType>
        StateBehavior<ValueType, StateType> JaniNextStateGenerator<ValueType, StateType>::expand(StateToIdCallback const& stateToIdCallback) {
            // Prepare the result, in case we return early.
            StateBehavior<ValueType, StateType> result;
            
            // Retrieve the locations from the state.
            std::vector<uint64_t> locations = getLocations(*this->state);

            // First, construct the state rewards, as we may return early if there are no choices later and we already
            // need the state rewards then.
            std::vector<ValueType> stateRewards(this->rewardVariables.size(), storm::utility::zero<ValueType>());
            uint64_t automatonIndex = 0;
            for (auto const& automaton : model.getAutomata()) {
                uint64_t currentLocationIndex = locations[automatonIndex];
                storm::jani::Location const& location = automaton.getLocation(currentLocationIndex);
                auto valueIt = stateRewards.begin();
                performTransientAssignments(location.getAssignments().getTransientAssignments(), [&valueIt] (ValueType const& value) { *valueIt += value; ++valueIt; } );
                ++automatonIndex;
            }
            result.addStateRewards(std::move(stateRewards));
            
            // If a terminal expression was set and we must not expand this state, return now.
            if (!this->terminalStates.empty()) {
                for (auto const& expressionBool : this->terminalStates) {
                    if (this->evaluator.asBool(expressionBool.first) == expressionBool.second) {
                        return result;
                    }
                }
            }
            
            // Get all choices for the state.
            result.setExpanded();
            std::vector<Choice<ValueType>> allChoices = getSilentActionChoices(locations, *this->state, stateToIdCallback);
            std::vector<Choice<ValueType>> allLabeledChoices = getNonsilentActionChoices(locations, *this->state, stateToIdCallback);
            for (auto& choice : allLabeledChoices) {
                allChoices.push_back(std::move(choice));
            }
            
            std::size_t totalNumberOfChoices = allChoices.size();
            
            // If there is not a single choice, we return immediately, because the state has no behavior (other than
            // the state reward).
            if (totalNumberOfChoices == 0) {
                return result;
            }
            
            // If the model is a deterministic model, we need to fuse the choices into one.
            if (this->isDeterministicModel() && totalNumberOfChoices > 1) {
                Choice<ValueType> globalChoice;
                
                // For CTMCs, we need to keep track of the total exit rate to scale the action rewards later. For DTMCs
                // this is equal to the number of choices, which is why we initialize it like this here.
                ValueType totalExitRate = this->isDiscreteTimeModel() ? static_cast<ValueType>(totalNumberOfChoices) : storm::utility::zero<ValueType>();

                // Iterate over all choices and combine the probabilities/rates into one choice.
                for (auto const& choice : allChoices) {
                    for (auto const& stateProbabilityPair : choice) {
                        if (this->isDiscreteTimeModel()) {
                            globalChoice.addProbability(stateProbabilityPair.first, stateProbabilityPair.second / totalNumberOfChoices);
                        } else {
                            globalChoice.addProbability(stateProbabilityPair.first, stateProbabilityPair.second);
                        }
                    }
                    
                    if (hasStateActionRewards && !this->isDiscreteTimeModel()) {
                        totalExitRate += choice.getTotalMass();
                    }
                    
                    if (this->options.isBuildChoiceLabelsSet()) {
                        globalChoice.addLabels(choice.getLabels());
                    }
                }
             
                std::vector<ValueType> stateActionRewards(rewardVariables.size(), storm::utility::zero<ValueType>());
                for (auto const& choice : allChoices) {
                    for (uint_fast64_t rewardVariableIndex = 0; rewardVariableIndex < rewardVariables.size(); ++rewardVariableIndex) {
                        stateActionRewards[rewardVariableIndex] += choice.getRewards()[rewardVariableIndex] * choice.getTotalMass() / totalExitRate;
                    }
                }
                globalChoice.addRewards(std::move(stateActionRewards));
                                
                // Move the newly fused choice in place.
                allChoices.clear();
                allChoices.push_back(std::move(globalChoice));
            }
            
            // Move all remaining choices in place.
            for (auto& choice : allChoices) {
                result.addChoice(std::move(choice));
            }
            
            this->postprocess(result);
            
            return result;
        }
        
        template<typename ValueType, typename StateType>
        std::vector<Choice<ValueType>> JaniNextStateGenerator<ValueType, StateType>::getSilentActionChoices(std::vector<uint64_t> const& locations, CompressedState const& state, StateToIdCallback stateToIdCallback) {
            std::vector<Choice<ValueType>> result;
            
            // Iterate over all automata.
            uint64_t automatonIndex = 0;
            for (auto const& automaton : model.getAutomata()) {
                uint64_t location = locations[automatonIndex];
                
                // Iterate over all edges from the source location.
                for (auto const& edge : automaton.getEdgesFromLocation(location)) {
                    // Skip the edge if it is labeled with a non-silent action.
                    if (edge.getActionIndex() != model.getSilentActionIndex()) {
                        continue;
                    }
                    
                    // Skip the command, if it is not enabled.
                    if (!this->evaluator.asBool(edge.getGuard())) {
                        continue;
                    }
                    
                    // Determine the exit rate if it's a Markovian edge.
                    boost::optional<ValueType> exitRate = boost::none;
                    if (edge.hasRate()) {
                        exitRate = this->evaluator.asRational(edge.getRate());
                    }
                    
                    result.push_back(Choice<ValueType>(edge.getActionIndex(), static_cast<bool>(exitRate)));
                    Choice<ValueType>& choice = result.back();
                    
                    // Iterate over all updates of the current command.
                    ValueType probabilitySum = storm::utility::zero<ValueType>();
                    for (auto const& destination : edge.getDestinations()) {
                        // Obtain target state index and add it to the list of known states. If it has not yet been
                        // seen, we also add it to the set of states that have yet to be explored.
                        StateType stateIndex = stateToIdCallback(applyUpdate(state, destination));
                        
                        // Update the choice by adding the probability/target state to it.
                        ValueType probability = this->evaluator.asRational(destination.getProbability());
                        probability = exitRate ? exitRate.get() * probability : probability;
                        choice.addProbability(stateIndex, probability);
                        probabilitySum += probability;
                    }
                    
                    // Create the state-action reward for the newly created choice.
                    performTransientAssignments(edge.getAssignments().getTransientAssignments(), [&choice] (ValueType const& value) { choice.addReward(value); } );

                    // Check that the resulting distribution is in fact a distribution.
                    STORM_LOG_THROW(!this->isDiscreteTimeModel() || this->comparator.isOne(probabilitySum), storm::exceptions::WrongFormatException, "Probabilities do not sum to one for edge (actually sum to " << probabilitySum << ").");
                }
                
                ++automatonIndex;
            }
            
            return result;
        }
        
        template<typename ValueType, typename StateType>
        std::vector<Choice<ValueType>> JaniNextStateGenerator<ValueType, StateType>::getNonsilentActionChoices(std::vector<uint64_t> const& locations, CompressedState const& state, StateToIdCallback stateToIdCallback) {
            std::vector<Choice<ValueType>> result;
            
            for (uint64_t actionIndex : model.getNonsilentActionIndices()) {
                std::vector<std::vector<storm::jani::Edge const*>> enabledEdges = getEnabledEdges(locations, actionIndex);
                
                // Only process this action, if there is at least one feasible solution.
                if (!enabledEdges.empty()) {
                    // Check whether a global variable is written multiple times in any combination.
                    checkGlobalVariableWritesValid(enabledEdges);
                    
                    std::vector<std::vector<storm::jani::Edge const*>::const_iterator> iteratorList(enabledEdges.size());
                    
                    // Initialize the list of iterators.
                    for (size_t i = 0; i < enabledEdges.size(); ++i) {
                        iteratorList[i] = enabledEdges[i].cbegin();
                    }
                    
                    // As long as there is one feasible combination of commands, keep on expanding it.
                    bool done = false;
                    while (!done) {
                        boost::container::flat_map<CompressedState, ValueType>* currentTargetStates = new boost::container::flat_map<CompressedState, ValueType>();
                        boost::container::flat_map<CompressedState, ValueType>* newTargetStates = new boost::container::flat_map<CompressedState, ValueType>();
                        std::vector<ValueType> stateActionRewards(rewardVariables.size(), storm::utility::zero<ValueType>());
                        
                        currentTargetStates->emplace(state, storm::utility::one<ValueType>());
                        
                        for (uint_fast64_t i = 0; i < iteratorList.size(); ++i) {
                            storm::jani::Edge const& edge = **iteratorList[i];
                            
                            for (auto const& destination : edge.getDestinations()) {
                                for (auto const& stateProbabilityPair : *currentTargetStates) {
                                    // Compute the new state under the current update and add it to the set of new target states.
                                    CompressedState newTargetState = applyUpdate(stateProbabilityPair.first, destination);
                                    
                                    // If the new state was already found as a successor state, update the probability
                                    // and otherwise insert it.
                                    auto targetStateIt = newTargetStates->find(newTargetState);
                                    if (targetStateIt != newTargetStates->end()) {
                                        targetStateIt->second += stateProbabilityPair.second * this->evaluator.asRational(destination.getProbability());
                                    } else {
                                        newTargetStates->emplace(newTargetState, stateProbabilityPair.second * this->evaluator.asRational(destination.getProbability()));
                                    }
                                }
                                
                                // Create the state-action reward for the newly created choice.
                                auto valueIt = stateActionRewards.begin();
                                performTransientAssignments(edge.getAssignments().getTransientAssignments(), [&valueIt] (ValueType const& value) { *valueIt += value; ++valueIt; } );
                            }
                            
                            // If there is one more command to come, shift the target states one time step back.
                            if (i < iteratorList.size() - 1) {
                                delete currentTargetStates;
                                currentTargetStates = newTargetStates;
                                newTargetStates = new boost::container::flat_map<CompressedState, ValueType>();
                            }
                        }
                        
                        // At this point, we applied all commands of the current command combination and newTargetStates
                        // contains all target states and their respective probabilities. That means we are now ready to
                        // add the choice to the list of transitions.
                        result.push_back(Choice<ValueType>(actionIndex));
                        
                        // Now create the actual distribution.
                        Choice<ValueType>& choice = result.back();
                        
                        // Add the rewards to the choice.
                        choice.addRewards(std::move(stateActionRewards));

                        // Add the probabilities/rates to the newly created choice.
                        ValueType probabilitySum = storm::utility::zero<ValueType>();
                        for (auto const& stateProbabilityPair : *newTargetStates) {
                            StateType actualIndex = stateToIdCallback(stateProbabilityPair.first);
                            choice.addProbability(actualIndex, stateProbabilityPair.second);
                            probabilitySum += stateProbabilityPair.second;
                        }
                        
                        // Check that the resulting distribution is in fact a distribution.
                        STORM_LOG_THROW(!this->isDiscreteTimeModel() || !this->comparator.isConstant(probabilitySum) || this->comparator.isOne(probabilitySum), storm::exceptions::WrongFormatException, "Sum of update probabilities do not some to one for some command (actually sum to " << probabilitySum << ").");
                        
                        // Dispose of the temporary maps.
                        delete currentTargetStates;
                        delete newTargetStates;
                        
                        // Now, check whether there is one more command combination to consider.
                        bool movedIterator = false;
                        for (uint64_t j = 0; !movedIterator && j < iteratorList.size(); ++j) {
                            ++iteratorList[j];
                            if (iteratorList[j] != enabledEdges[j].end()) {
                                movedIterator = true;
                            } else {
                                // Reset the iterator to the beginning of the list.
                                iteratorList[j] = enabledEdges[j].begin();
                            }
                        }
                        
                        done = !movedIterator;
                    }
                }
            }
            
            return result;
        }
        
        template<typename ValueType, typename StateType>
        std::vector<std::vector<storm::jani::Edge const*>> JaniNextStateGenerator<ValueType, StateType>::getEnabledEdges(std::vector<uint64_t> const& locationIndices, uint64_t actionIndex) {
            std::vector<std::vector<storm::jani::Edge const*>> result;
            
            // Iterate over all automata.
            uint64_t automatonIndex = 0;
            for (auto const& automaton : model.getAutomata()) {
                
                // If the automaton has no edge labeled with the given action, we can skip it.
                if (!automaton.hasEdgeLabeledWithActionIndex(actionIndex)) {
                    continue;
                }
                
                auto edges = automaton.getEdgesFromLocation(locationIndices[automatonIndex], actionIndex);
                
                // If the automaton contains the action, but there is no edge available labeled with
                // this action, we don't have any feasible command combinations.
                if (edges.empty()) {
                    return std::vector<std::vector<storm::jani::Edge const*>>();
                }
                
                std::vector<storm::jani::Edge const*> edgePointers;
                for (auto const& edge : edges) {
                    if (this->evaluator.asBool(edge.getGuard())) {
                        edgePointers.push_back(&edge);
                    }
                }
                
                // If there was no enabled edge although the automaton has some edge with the required action, we must
                // not return anything.
                if (edgePointers.empty()) {
                    return std::vector<std::vector<storm::jani::Edge const*>>();
                }
                
                result.emplace_back(std::move(edgePointers));
                ++automatonIndex;
            }
            
            return result;
        }
        
        template<typename ValueType, typename StateType>
        void JaniNextStateGenerator<ValueType, StateType>::checkGlobalVariableWritesValid(std::vector<std::vector<storm::jani::Edge const*>> const& enabledEdges) const {
            std::map<storm::expressions::Variable, uint64_t> writtenGlobalVariables;
            for (auto edgeSetIt = enabledEdges.begin(), edgeSetIte = enabledEdges.end(); edgeSetIt != edgeSetIte; ++edgeSetIt) {
                for (auto const& edge : *edgeSetIt) {
                    for (auto const& globalVariable : edge->getWrittenGlobalVariables()) {
                        auto it = writtenGlobalVariables.find(globalVariable);
                        
                        auto index = std::distance(enabledEdges.begin(), edgeSetIt);
                        if (it != writtenGlobalVariables.end()) {
                            STORM_LOG_THROW(it->second == index, storm::exceptions::WrongFormatException, "Multiple writes to global variable '" << globalVariable.getName() << "' in synchronizing edges.");
                        } else {
                            writtenGlobalVariables.emplace(globalVariable, index);
                        }
                    }
                }
            }
        }
        
        template<typename ValueType, typename StateType>
        std::size_t JaniNextStateGenerator<ValueType, StateType>::getNumberOfRewardModels() const {
            return rewardVariables.size();
        }
        
        template<typename ValueType, typename StateType>
        RewardModelInformation JaniNextStateGenerator<ValueType, StateType>::getRewardModelInformation(uint64_t const& index) const {
            return rewardModelInformation[index];
        }
        
        template<typename ValueType, typename StateType>
        storm::models::sparse::StateLabeling JaniNextStateGenerator<ValueType, StateType>::label(storm::storage::BitVectorHashMap<StateType> const& states, std::vector<StateType> const& initialStateIndices, std::vector<StateType> const& deadlockStateIndices) {
            return NextStateGenerator<ValueType, StateType>::label(states, initialStateIndices, deadlockStateIndices, {});
        }
        
        template<typename ValueType, typename StateType>
        void JaniNextStateGenerator<ValueType, StateType>::performTransientAssignments(storm::jani::detail::ConstAssignments const& transientAssignments, std::function<void (ValueType const&)> const& callback) {
            // If there are no reward variables, there is no need to iterate at all.
            if (rewardVariables.empty()) {
                return;
            }
            
            // Otherwise, perform the callback for all selected reward variables.
            auto rewardVariableIt = rewardVariables.begin();
            auto rewardVariableIte = rewardVariables.end();
            for (auto const& assignment : transientAssignments) {
                while (rewardVariableIt != rewardVariableIte && *rewardVariableIt < assignment.getExpressionVariable()) {
                    callback(storm::utility::zero<ValueType>());
                    ++rewardVariableIt;
                }
                if (rewardVariableIt == rewardVariableIte) {
                    break;
                } else if (*rewardVariableIt == assignment.getExpressionVariable()) {
                    callback(ValueType(this->evaluator.asRational(assignment.getAssignedExpression())));
                    ++rewardVariableIt;
                }
            }
            // Add a value of zero for all variables that have no assignment.
            for (; rewardVariableIt != rewardVariableIte; ++rewardVariableIt) {
                callback(storm::utility::zero<ValueType>());
            }
        }
        
        template<typename ValueType, typename StateType>
        void JaniNextStateGenerator<ValueType, StateType>::buildRewardModelInformation() {
            // Prepare all reward model information structs.
            for (auto const& variable : rewardVariables) {
                rewardModelInformation.emplace_back(variable.getName(), false, false, false);
            }
            
            // Then fill them.
            for (auto const& automaton : model.getAutomata()) {
                for (auto const& location : automaton.getLocations()) {
                    auto rewardVariableIt = rewardVariables.begin();
                    auto rewardVariableIte = rewardVariables.end();
                    
                    for (auto const& assignment : location.getAssignments().getTransientAssignments()) {
                        while (rewardVariableIt != rewardVariableIte && *rewardVariableIt < assignment.getExpressionVariable()) {
                            ++rewardVariableIt;
                        }
                        if (rewardVariableIt == rewardVariableIte) {
                            break;
                        }
                        if (*rewardVariableIt == assignment.getExpressionVariable()) {
                            rewardModelInformation[std::distance(rewardVariables.begin(), rewardVariableIt)].setHasStateRewards();
                            ++rewardVariableIt;
                        }
                    }
                }

                for (auto const& edge : automaton.getEdges()) {
                    auto rewardVariableIt = rewardVariables.begin();
                    auto rewardVariableIte = rewardVariables.end();
                    
                    for (auto const& assignment : edge.getAssignments().getTransientAssignments()) {
                        while (rewardVariableIt != rewardVariableIte && *rewardVariableIt < assignment.getExpressionVariable()) {
                            ++rewardVariableIt;
                        }
                        if (rewardVariableIt == rewardVariableIte) {
                            break;
                        }
                        if (*rewardVariableIt == assignment.getExpressionVariable()) {
                            rewardModelInformation[std::distance(rewardVariables.begin(), rewardVariableIt)].setHasStateActionRewards();
                            hasStateActionRewards = true;
                            ++rewardVariableIt;
                        }
                    }
                }
            }
        }
        
        template<typename ValueType, typename StateType>
        void JaniNextStateGenerator<ValueType, StateType>::checkValid() const {
            // If the program still contains undefined constants and we are not in a parametric setting, assemble an appropriate error message.
#ifdef STORM_HAVE_CARL
            if (!std::is_same<ValueType, storm::RationalFunction>::value && model.hasUndefinedConstants()) {
#else
            if (model.hasUndefinedConstants()) {
#endif
                std::vector<std::reference_wrapper<storm::jani::Constant const>> undefinedConstants = model.getUndefinedConstants();
                std::stringstream stream;
                bool printComma = false;
                for (auto const& constant : undefinedConstants) {
                    if (printComma) {
                        stream << ", ";
                    } else {
                        printComma = true;
                    }
                    stream << constant.get().getName() << " (" << constant.get().getType() << ")";
                }
                stream << ".";
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Program still contains these undefined constants: " + stream.str());
            }
                
#ifdef STORM_HAVE_CARL
            else if (std::is_same<ValueType, storm::RationalFunction>::value && !model.undefinedConstantsAreGraphPreserving()) {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The input model contains undefined constants that influence the graph structure of the underlying model, which is not allowed.");
            }
#endif
        }
        
        template class JaniNextStateGenerator<double>;

#ifdef STORM_HAVE_CARL
        template class JaniNextStateGenerator<storm::RationalNumber>;
        template class JaniNextStateGenerator<storm::RationalFunction>;
#endif
    }
}
