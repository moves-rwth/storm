#include "storm/generator/JaniNextStateGenerator.h"

#include "storm/models/sparse/StateLabeling.h"

#include "storm/storage/expressions/SimpleValuation.h"

#include "storm/solver/SmtSolver.h"

#include "storm/storage/jani/Edge.h"
#include "storm/storage/jani/EdgeDestination.h"
#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Automaton.h"
#include "storm/storage/jani/Location.h"
#include "storm/storage/jani/AutomatonComposition.h"
#include "storm/storage/jani/ParallelComposition.h"
#include "storm/storage/jani/CompositionInformationVisitor.h"

#include "storm/storage/sparse/JaniChoiceOrigins.h"

#include "storm/builder/jit/Distribution.h"

#include "storm/utility/constants.h"
#include "storm/utility/macros.h"
#include "storm/utility/solver.h"
#include "storm/utility/combinatorics.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace generator {
        
        template<typename ValueType, typename StateType>
        JaniNextStateGenerator<ValueType, StateType>::JaniNextStateGenerator(storm::jani::Model const& model, NextStateGeneratorOptions const& options) : JaniNextStateGenerator(model.substituteConstants(), options, false) {
            // Intentionally left empty.
        }
        
        template<typename ValueType, typename StateType>
        JaniNextStateGenerator<ValueType, StateType>::JaniNextStateGenerator(storm::jani::Model const& model, NextStateGeneratorOptions const& options, bool) : NextStateGenerator<ValueType, StateType>(model.getExpressionManager(), options), model(model), rewardVariables(), hasStateActionRewards(false) {
            STORM_LOG_THROW(!model.hasNonGlobalTransientVariable(), storm::exceptions::InvalidSettingsException, "The explicit next-state generator currently does not support automata-local transient variables.");
            STORM_LOG_THROW(!model.usesAssignmentLevels(), storm::exceptions::InvalidSettingsException, "The explicit next-state generator currently does not support assignment levels.");
            STORM_LOG_THROW(!this->options.isBuildChoiceLabelsSet(), storm::exceptions::InvalidSettingsException, "JANI next-state generator cannot generate choice labels.");

            // Lift the transient edge destinations. We can do so, as we know that there are no assignment levels (because that's not supported anyway).
            if (this->model.hasTransientEdgeDestinationAssignments()) {
                this->model.liftTransientEdgeDestinationAssignments();
            }
            STORM_LOG_THROW(!this->model.hasTransientEdgeDestinationAssignments(), storm::exceptions::InvalidSettingsException, "The explicit next-state generator currently does not support transient edge destination assignments.");
            
            // Create all synchronization-related information, e.g. the automata that are put in parallel.
            this->createSynchronizationInformation();

            // Now we are ready to initialize the variable information.
            this->checkValid();
            this->variableInformation = VariableInformation(model, this->parallelAutomata, options.isAddOutOfBoundsStateSet());
            
            // Create a proper evalator.
            this->evaluator = std::make_unique<storm::expressions::ExpressionEvaluator<ValueType>>(model.getManager());
            
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
                        STORM_LOG_THROW(globalVariables.getNumberOfRealTransientVariables() + globalVariables.getNumberOfUnboundedIntegerTransientVariables() == 1, storm::exceptions::InvalidArgumentException, "Reference to standard reward model is ambiguous.");
                    }
                }
                
                // If no reward model was yet added, but there was one that was given in the options, we try to build the
                // standard reward model.
                if (rewardVariables.empty() && !this->options.getRewardModelNames().empty()) {
                    bool foundTransientVariable = false;
                    for (auto const& transientVariable : globalVariables.getTransientVariables()) {
                        if (transientVariable.isUnboundedIntegerVariable() || transientVariable.isRealVariable()) {
                            rewardVariables.push_back(transientVariable.getExpressionVariable());
                            foundTransientVariable = true;
                            break;
                        }
                    }
                    STORM_LOG_ASSERT(foundTransientVariable, "Expected to find a fitting transient variable.");
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
                        // If it's a label, i.e. refers to a transient boolean variable we need to derive the expression
                        // for the label so we can cut off the exploration there.
                        if (expressionOrLabelAndBool.first.getLabel() != "init" && expressionOrLabelAndBool.first.getLabel() != "deadlock") {
                            STORM_LOG_THROW(model.getGlobalVariables().hasVariable(expressionOrLabelAndBool.first.getLabel()) , storm::exceptions::InvalidSettingsException, "Terminal states refer to illegal label '" << expressionOrLabelAndBool.first.getLabel() << "'.");
                            
                            storm::jani::Variable const& variable = model.getGlobalVariables().getVariable(expressionOrLabelAndBool.first.getLabel());
                            STORM_LOG_THROW(variable.isBooleanVariable(), storm::exceptions::InvalidSettingsException, "Terminal states refer to non-boolean variable '" << expressionOrLabelAndBool.first.getLabel() << "'.");
                            STORM_LOG_THROW(variable.isTransient(), storm::exceptions::InvalidSettingsException, "Terminal states refer to non-transient variable '" << expressionOrLabelAndBool.first.getLabel() << "'.");
                            
                            this->terminalStates.push_back(std::make_pair(this->model.getLabelExpression(variable.asBooleanVariable(), this->parallelAutomata), expressionOrLabelAndBool.second));
                        }
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
                if (it->bitWidth == 0) {
                    *resultIt = 0;
                } else {
                    *resultIt = state.getAsInt(it->bitOffset, it->bitWidth);
                }
            }
            
            return result;
        }
        
        template<typename ValueType, typename StateType>
        std::vector<StateType> JaniNextStateGenerator<ValueType, StateType>::getInitialStates(StateToIdCallback const& stateToIdCallback) {
            // Prepare an SMT solver to enumerate all initial states.
            storm::utility::solver::SmtSolverFactory factory;
            std::unique_ptr<storm::solver::SmtSolver> solver = factory.create(model.getExpressionManager());
            
            std::vector<storm::expressions::Expression> rangeExpressions = model.getAllRangeExpressions(this->parallelAutomata);
            for (auto const& expression : rangeExpressions) {
                solver->add(expression);
            }
            solver->add(model.getInitialStatesExpression(this->parallelAutomata));
            
            // Proceed as long as the solver can still enumerate initial states.
            std::vector<StateType> initialStateIndices;
            while (solver->check() == storm::solver::SmtSolver::CheckResult::Sat) {
                // Create fresh state.
                CompressedState initialState(this->variableInformation.getTotalBitOffset(true));
                
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
                std::vector<std::set<uint64_t>::const_iterator> initialLocationsIts;
                std::vector<std::set<uint64_t>::const_iterator> initialLocationsItes;
                for (auto const& automatonRef : this->parallelAutomata) {
                    auto const& automaton = automatonRef.get();
                    initialLocationsIts.push_back(automaton.getInitialLocationIndices().cbegin());
                    initialLocationsItes.push_back(automaton.getInitialLocationIndices().cend());
                }
                storm::utility::combinatorics::forEach(initialLocationsIts, initialLocationsItes, [this,&initialState] (uint64_t index, uint64_t value) { setLocation(initialState, this->variableInformation.locationVariables[index], value); }, [&stateToIdCallback,&initialStateIndices,&initialState] () {
                    // Register initial state.
                    StateType id = stateToIdCallback(initialState);
                    initialStateIndices.push_back(id);
                    return true;
                });
                
                // Block the current initial state to search for the next one.
                if (!blockingExpression.isInitialized()) {
                    break;
                }
                solver->add(blockingExpression);
            }
            
            return initialStateIndices;
        }
        
        template<typename ValueType, typename StateType>
        CompressedState JaniNextStateGenerator<ValueType, StateType>::applyUpdate(CompressedState const& state, storm::jani::EdgeDestination const& destination, storm::generator::LocationVariableInformation const& locationVariable) {
            CompressedState newState(state);
            
            // Update the location of the state.
            setLocation(newState, locationVariable, destination.getLocationIndex());
            
            // Then perform the assignments.
            auto assignmentIt = destination.getOrderedAssignments().getNonTransientAssignments().begin();
            auto assignmentIte = destination.getOrderedAssignments().getNonTransientAssignments().end();
            
            // Iterate over all boolean assignments and carry them out.
            auto boolIt = this->variableInformation.booleanVariables.begin();
            for (; assignmentIt != assignmentIte && assignmentIt->getAssignedExpression().hasBooleanType(); ++assignmentIt) {
                while (assignmentIt->getExpressionVariable() != boolIt->variable) {
                    ++boolIt;
                }
                newState.set(boolIt->bitOffset, this->evaluator->asBool(assignmentIt->getAssignedExpression()));
            }
            
            // Iterate over all integer assignments and carry them out.
            auto integerIt = this->variableInformation.integerVariables.begin();
            for (; assignmentIt != assignmentIte && assignmentIt->getAssignedExpression().hasIntegerType(); ++assignmentIt) {
                while (assignmentIt->getExpressionVariable() != integerIt->variable) {
                    ++integerIt;
                }
                int_fast64_t assignedValue = this->evaluator->asInt(assignmentIt->getAssignedExpression());
                if (this->options.isAddOutOfBoundsStateSet()) {
                    if (assignedValue < integerIt->lowerBound || assignedValue > integerIt->upperBound) {
                        return this->outOfBoundsState;
                    }
                } else if (this->options.isExplorationChecksSet()) {
                    STORM_LOG_THROW(assignedValue >= integerIt->lowerBound, storm::exceptions::WrongFormatException, "The update " << assignmentIt->getExpressionVariable().getName() << " := " << assignmentIt->getAssignedExpression() << " leads to an out-of-bounds value (" << assignedValue << ") for the variable '" << assignmentIt->getExpressionVariable().getName() << "'.");
                    STORM_LOG_THROW(assignedValue <= integerIt->upperBound, storm::exceptions::WrongFormatException, "The update " << assignmentIt->getExpressionVariable().getName() << " := " << assignmentIt->getAssignedExpression() << " leads to an out-of-bounds value (" << assignedValue << ") for the variable '" << assignmentIt->getExpressionVariable().getName() << "'.");
                }

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
            for (auto const& automatonRef : this->parallelAutomata) {
                auto const& automaton = automatonRef.get();
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
                    if (this->evaluator->asBool(expressionBool.first) == expressionBool.second) {
                        return result;
                    }
                }
            }
            
            // Get all choices for the state.
            result.setExpanded();
            std::vector<Choice<ValueType>> allChoices = getActionChoices(locations, *this->state, stateToIdCallback);
            std::size_t totalNumberOfChoices = allChoices.size();
            
            // If there is not a single choice, we return immediately, because the state has no behavior (other than
            // the state reward).
            if (totalNumberOfChoices == 0) {
                return result;
            }
            
            // If the model is a deterministic model, we need to fuse the choices into one.
            if (this->isDeterministicModel() && totalNumberOfChoices > 1) {
                Choice<ValueType> globalChoice;

                if (this->options.isAddOverlappingGuardLabelSet()) {
                    this->overlappingGuardStates->push_back(stateToIdCallback(*this->state));
                }
                
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
                }
                
                std::vector<ValueType> stateActionRewards(rewardVariables.size(), storm::utility::zero<ValueType>());
                for (auto const& choice : allChoices) {
                    for (uint_fast64_t rewardVariableIndex = 0; rewardVariableIndex < rewardVariables.size(); ++rewardVariableIndex) {
                        stateActionRewards[rewardVariableIndex] += choice.getRewards()[rewardVariableIndex] * choice.getTotalMass() / totalExitRate;
                    }
                    
                    if (this->options.isBuildChoiceOriginsSet() && choice.hasOriginData()) {
                        globalChoice.addOriginData(choice.getOriginData());
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
        Choice<ValueType> JaniNextStateGenerator<ValueType, StateType>::expandNonSynchronizingEdge(storm::jani::Edge const& edge, uint64_t outputActionIndex, uint64_t automatonIndex, CompressedState const& state, StateToIdCallback stateToIdCallback) {
            // Determine the exit rate if it's a Markovian edge.
            boost::optional<ValueType> exitRate = boost::none;
            if (edge.hasRate()) {
                exitRate = this->evaluator->asRational(edge.getRate());
            }
            
            Choice<ValueType> choice(edge.getActionIndex(), static_cast<bool>(exitRate));
            
            // Iterate over all updates of the current command.
            ValueType probabilitySum = storm::utility::zero<ValueType>();
            for (auto const& destination : edge.getDestinations()) {
                ValueType probability = this->evaluator->asRational(destination.getProbability());
                
                if (probability != storm::utility::zero<ValueType>()) {
                    // Obtain target state index and add it to the list of known states. If it has not yet been
                    // seen, we also add it to the set of states that have yet to be explored.
                    auto newState = applyUpdate(state, destination, this->variableInformation.locationVariables[automatonIndex]);
                    
                    StateType stateIndex = stateToIdCallback(applyUpdate(state, destination, this->variableInformation.locationVariables[automatonIndex]));
                    
                    // Update the choice by adding the probability/target state to it.
                    probability = exitRate ? exitRate.get() * probability : probability;
                    choice.addProbability(stateIndex, probability);
                    
                    if (this->options.isExplorationChecksSet()) {
                        probabilitySum += probability;
                    }
                }
            }
            
            // Create the state-action reward for the newly created choice.
            performTransientAssignments(edge.getAssignments().getTransientAssignments(), [&choice] (ValueType const& value) { choice.addReward(value); } );
            
            if (this->options.isExplorationChecksSet()) {
                // Check that the resulting distribution is in fact a distribution.
                STORM_LOG_THROW(!this->isDiscreteTimeModel() || this->comparator.isOne(probabilitySum), storm::exceptions::WrongFormatException, "Probabilities do not sum to one for edge (actually sum to " << probabilitySum << ").");
            }
            
            return choice;
        }
        
        template<typename ValueType, typename StateType>
        std::vector<Choice<ValueType>> JaniNextStateGenerator<ValueType, StateType>::expandSynchronizingEdgeCombination(AutomataEdgeSets const& edgeCombination, uint64_t outputActionIndex, CompressedState const& state, StateToIdCallback stateToIdCallback) {
            std::vector<Choice<ValueType>> result;
            
            if (this->options.isExplorationChecksSet()) {
                // Check whether a global variable is written multiple times in any combination.
                checkGlobalVariableWritesValid(edgeCombination);
            }
            
            std::vector<EdgeSetWithIndices::const_iterator> iteratorList(edgeCombination.size());
            
            // Initialize the list of iterators.
            for (size_t i = 0; i < edgeCombination.size(); ++i) {
                iteratorList[i] = edgeCombination[i].second.cbegin();
            }
            
            storm::builder::jit::Distribution<CompressedState, ValueType> currentDistribution;
            storm::builder::jit::Distribution<CompressedState, ValueType> nextDistribution;

            // As long as there is one feasible combination of commands, keep on expanding it.
            bool done = false;
            while (!done) {
                currentDistribution.clear();
                nextDistribution.clear();
                
                std::vector<ValueType> stateActionRewards(rewardVariables.size(), storm::utility::zero<ValueType>());
                
                currentDistribution.add(state, storm::utility::one<ValueType>());
                
                EdgeIndexSet edgeIndices;
                for (uint_fast64_t i = 0; i < iteratorList.size(); ++i) {
                    auto const& indexAndEdge = *iteratorList[i];
                    
                    if (this->getOptions().isBuildChoiceOriginsSet()) {
                        edgeIndices.insert(model.encodeAutomatonAndEdgeIndices(edgeCombination[i].first, indexAndEdge.first));
                    }
                    
                    storm::jani::Edge const& edge = *indexAndEdge.second;
                    
                    for (auto const& destination : edge.getDestinations()) {
                        for (auto const& stateProbability : currentDistribution) {
                            // Compute the new state under the current update and add it to the set of new target states.
                            CompressedState newTargetState = applyUpdate(stateProbability.getState(), destination, this->variableInformation.locationVariables[edgeCombination[i].first]);
                            
                            // If the new state was already found as a successor state, update the probability
                            // and otherwise insert it.
                            ValueType probability = stateProbability.getValue() * this->evaluator->asRational(destination.getProbability());
                            if (edge.hasRate()) {
                                probability *= this->evaluator->asRational(edge.getRate());
                            }
                            if (probability != storm::utility::zero<ValueType>()) {
                                nextDistribution.add(newTargetState, probability);
                            }
                        }
                    }
                    
                    // Create the state-action reward for the newly created choice.
                    auto valueIt = stateActionRewards.begin();
                    performTransientAssignments(edge.getAssignments().getTransientAssignments(), [&valueIt] (ValueType const& value) { *valueIt += value; ++valueIt; } );
                    
                    nextDistribution.compress();
                    
                    // If there is one more command to come, shift the target states one time step back.
                    if (i < iteratorList.size() - 1) {
                        currentDistribution = std::move(nextDistribution);
                    }
                }
                
                // At this point, we applied all commands of the current command combination and newTargetStates
                // contains all target states and their respective probabilities. That means we are now ready to
                // add the choice to the list of transitions.
                result.emplace_back(outputActionIndex);
                
                // Now create the actual distribution.
                Choice<ValueType>& choice = result.back();
                
                // Add the edge indices if requested.
                if (this->getOptions().isBuildChoiceOriginsSet()) {
                    choice.addOriginData(boost::any(std::move(edgeIndices)));
                }
                
                // Add the rewards to the choice.
                choice.addRewards(std::move(stateActionRewards));
                
                // Add the probabilities/rates to the newly created choice.
                ValueType probabilitySum = storm::utility::zero<ValueType>();
                for (auto const& stateProbability : nextDistribution) {
                    StateType actualIndex = stateToIdCallback(stateProbability.getState());
                    choice.addProbability(actualIndex, stateProbability.getValue());
                    
                    if (this->options.isExplorationChecksSet()) {
                        probabilitySum += stateProbability.getValue();
                    }
                }
                
                if (this->options.isExplorationChecksSet()) {
                    // Check that the resulting distribution is in fact a distribution.
                    STORM_LOG_THROW(!this->isDiscreteTimeModel() || !this->comparator.isConstant(probabilitySum) || this->comparator.isOne(probabilitySum), storm::exceptions::WrongFormatException, "Sum of update probabilities do not sum to one for some edge (actually sum to " << probabilitySum << ").");
                }
                
                // Now, check whether there is one more command combination to consider.
                bool movedIterator = false;
                for (uint64_t j = 0; !movedIterator && j < iteratorList.size(); ++j) {
                    ++iteratorList[j];
                    if (iteratorList[j] != edgeCombination[j].second.end()) {
                        movedIterator = true;
                    } else {
                        // Reset the iterator to the beginning of the list.
                        iteratorList[j] = edgeCombination[j].second.begin();
                    }
                }
                
                done = !movedIterator;
            }
            
            return result;
        }
        
        template<typename ValueType, typename StateType>
        std::vector<Choice<ValueType>> JaniNextStateGenerator<ValueType, StateType>::getActionChoices(std::vector<uint64_t> const& locations, CompressedState const& state, StateToIdCallback stateToIdCallback) {
            std::vector<Choice<ValueType>> result;
            
            for (auto const& outputAndEdges : edges) {
                auto const& edges = outputAndEdges.second;
                if (edges.size() == 1) {
                    // If the synch consists of just one element, it's non-synchronizing.
                    auto const& nonsychingEdges = edges.front();
                    uint64_t automatonIndex = nonsychingEdges.first;

                    auto edgesIt = nonsychingEdges.second.find(locations[automatonIndex]);
                    if (edgesIt != nonsychingEdges.second.end()) {
                        for (auto const& indexAndEdge : edgesIt->second) {
                            if (!this->evaluator->asBool(indexAndEdge.second->getGuard())) {
                                continue;
                            }
                        
                            Choice<ValueType> choice = expandNonSynchronizingEdge(*indexAndEdge.second, outputAndEdges.first ? outputAndEdges.first.get() : indexAndEdge.second->getActionIndex(), automatonIndex, state, stateToIdCallback);

                            if (this->getOptions().isBuildChoiceOriginsSet()) {
                                EdgeIndexSet edgeIndex { model.encodeAutomatonAndEdgeIndices(automatonIndex, indexAndEdge.first) };
                                choice.addOriginData(boost::any(std::move(edgeIndex)));
                            }
                            result.emplace_back(std::move(choice));
                        }
                    }
                } else {
                    // If the element has more than one set of edges, we need to perform a synchronization.
                    STORM_LOG_ASSERT(outputAndEdges.first, "Need output action index for synchronization.");
                    
                    AutomataEdgeSets automataEdgeSets;
                    uint64_t outputActionIndex = outputAndEdges.first.get();
                    
                    bool productiveCombination = true;
                    for (auto const& automatonAndEdges : outputAndEdges.second) {
                        uint64_t automatonIndex = automatonAndEdges.first;
                        EdgeSetWithIndices enabledEdgesOfAutomaton;
                        
                        bool atLeastOneEdge = false;
                        auto edgesIt = automatonAndEdges.second.find(locations[automatonIndex]);
                        if (edgesIt != automatonAndEdges.second.end()) {
                            for (auto const& indexAndEdge : edgesIt->second) {
                                if (!this->evaluator->asBool(indexAndEdge.second->getGuard())) {
                                    continue;
                                }
                            
                                atLeastOneEdge = true;
                                enabledEdgesOfAutomaton.emplace_back(indexAndEdge);
                            }
                        }

                        // If there is no enabled edge of this automaton, the whole combination is not productive.
                        if (!atLeastOneEdge) {
                            productiveCombination = false;
                            break;
                        }
                        
                        automataEdgeSets.emplace_back(std::make_pair(automatonIndex, std::move(enabledEdgesOfAutomaton)));
                    }
                    
                    if (productiveCombination) {
                        std::vector<Choice<ValueType>> choices = expandSynchronizingEdgeCombination(automataEdgeSets, outputActionIndex, state, stateToIdCallback);
                        
                        for (auto const& choice : choices) {
                            result.emplace_back(std::move(choice));
                        }
                    }
                 }
            }

            return result;
        }
        
        template<typename ValueType, typename StateType>
        void JaniNextStateGenerator<ValueType, StateType>::checkGlobalVariableWritesValid(AutomataEdgeSets const& enabledEdges) const {
            std::map<storm::expressions::Variable, uint64_t> writtenGlobalVariables;
            for (auto edgeSetIt = enabledEdges.begin(), edgeSetIte = enabledEdges.end(); edgeSetIt != edgeSetIte; ++edgeSetIt) {
                for (auto const& indexAndEdge : edgeSetIt->second) {
                    for (auto const& globalVariable : indexAndEdge.second->getWrittenGlobalVariables()) {
                        auto it = writtenGlobalVariables.find(globalVariable);
                        
                        auto index = std::distance(enabledEdges.begin(), edgeSetIt);
                        if (it != writtenGlobalVariables.end()) {
                            STORM_LOG_THROW(it->second == static_cast<uint64_t>(index), storm::exceptions::WrongFormatException, "Multiple writes to global variable '" << globalVariable.getName() << "' in synchronizing edges.");
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
        storm::builder::RewardModelInformation JaniNextStateGenerator<ValueType, StateType>::getRewardModelInformation(uint64_t const& index) const {
            return rewardModelInformation[index];
        }
        
        template<typename ValueType, typename StateType>
        storm::models::sparse::StateLabeling JaniNextStateGenerator<ValueType, StateType>::label(storm::storage::sparse::StateStorage<StateType> const& stateStorage, std::vector<StateType> const& initialStateIndices, std::vector<StateType> const& deadlockStateIndices) {
            // As in JANI we can use transient boolean variable assignments in locations to identify states, we need to
            // create a list of boolean transient variables and the expressions that define them.
            std::unordered_map<storm::expressions::Variable, storm::expressions::Expression> transientVariableToExpressionMap;
            for (auto const& variable : model.getGlobalVariables().getTransientVariables()) {
                if (variable.isBooleanVariable()) {
                    if (this->options.isBuildAllLabelsSet() || this->options.getLabelNames().find(variable.getName()) != this->options.getLabelNames().end()) {
                        transientVariableToExpressionMap[variable.getExpressionVariable()] = model.getLabelExpression(variable.asBooleanVariable(), this->parallelAutomata);
                    }
                }
            }
            
            std::vector<std::pair<std::string, storm::expressions::Expression>> transientVariableExpressions;
            for (auto const& element : transientVariableToExpressionMap) {
                transientVariableExpressions.push_back(std::make_pair(element.first.getName(), element.second));
            }
            
            return NextStateGenerator<ValueType, StateType>::label(stateStorage, initialStateIndices, deadlockStateIndices, transientVariableExpressions);
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
                    callback(ValueType(this->evaluator->asRational(assignment.getAssignedExpression())));
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
            for (auto const& automatonRef : this->parallelAutomata) {
                auto const& automaton = automatonRef.get();
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
        void JaniNextStateGenerator<ValueType, StateType>::createSynchronizationInformation() {
            // Create synchronizing edges information.
            storm::jani::Composition const& topLevelComposition = this->model.getSystemComposition();
            if (topLevelComposition.isAutomatonComposition()) {
                auto const& automaton = this->model.getAutomaton(topLevelComposition.asAutomatonComposition().getAutomatonName());
                this->parallelAutomata.push_back(automaton);
                
                LocationsAndEdges locationsAndEdges;
                uint64_t edgeIndex = 0;
                for (auto const& edge : automaton.getEdges()) {
                    locationsAndEdges[edge.getSourceLocationIndex()].emplace_back(std::make_pair(edgeIndex, &edge));
                    ++edgeIndex;
                }
                
                AutomataAndEdges automataAndEdges;
                automataAndEdges.emplace_back(std::make_pair(0, std::move(locationsAndEdges)));
                
                this->edges.emplace_back(std::make_pair(boost::none, std::move(automataAndEdges)));
            } else {
                STORM_LOG_THROW(topLevelComposition.isParallelComposition(), storm::exceptions::WrongFormatException, "Expected parallel composition.");
                storm::jani::ParallelComposition const& parallelComposition = topLevelComposition.asParallelComposition();
                
                uint64_t automatonIndex = 0;
                for (auto const& composition : parallelComposition.getSubcompositions()) {
                    STORM_LOG_THROW(composition->isAutomatonComposition(), storm::exceptions::WrongFormatException, "Expected flat parallel composition.");
                    this->parallelAutomata.push_back(this->model.getAutomaton(composition->asAutomatonComposition().getAutomatonName()));
                
                    // Add edges with silent action.
                    LocationsAndEdges locationsAndEdges;
                    uint64_t edgeIndex = 0;
                    for (auto const& edge : parallelAutomata.back().get().getEdges()) {
                        if (edge.getActionIndex() == storm::jani::Model::SILENT_ACTION_INDEX) {
                            locationsAndEdges[edge.getSourceLocationIndex()].emplace_back(std::make_pair(edgeIndex, &edge));
                        }
                        ++edgeIndex;
                    }

                    if (!locationsAndEdges.empty()) {
                        AutomataAndEdges automataAndEdges;
                        automataAndEdges.emplace_back(std::make_pair(automatonIndex, std::move(locationsAndEdges)));
                        this->edges.emplace_back(std::make_pair(boost::none, std::move(automataAndEdges)));
                    }
                    ++automatonIndex;
                }
                
                for (auto const& vector : parallelComposition.getSynchronizationVectors()) {
                    uint64_t outputActionIndex = this->model.getActionIndex(vector.getOutput());
                    
                    AutomataAndEdges automataAndEdges;
                    bool atLeastOneEdge = true;
                    uint64_t automatonIndex = 0;
                    for (auto const& element : vector.getInput()) {
                        if (!storm::jani::SynchronizationVector::isNoActionInput(element)) {
                            LocationsAndEdges locationsAndEdges;
                            uint64_t actionIndex = this->model.getActionIndex(element);
                            uint64_t edgeIndex = 0;
                            for (auto const& edge : parallelAutomata[automatonIndex].get().getEdges()) {
                                if (edge.getActionIndex() == actionIndex) {
                                    locationsAndEdges[edge.getSourceLocationIndex()].emplace_back(std::make_pair(edgeIndex, &edge));
                                }
                                ++edgeIndex;
                            }
                            if (locationsAndEdges.empty()) {
                                atLeastOneEdge = false;
                                break;
                            }
                            automataAndEdges.emplace_back(std::make_pair(automatonIndex, std::move(locationsAndEdges)));
                        }
                        ++automatonIndex;
                    }
                    
                    if (atLeastOneEdge) {
                        this->edges.emplace_back(std::make_pair(outputActionIndex, std::move(automataAndEdges)));
                    }
                }
            }
            
            STORM_LOG_TRACE("Number of synchronizations: " << this->edges.size() << ".");
        }
        
        template<typename ValueType, typename StateType>
        std::shared_ptr<storm::storage::sparse::ChoiceOrigins> JaniNextStateGenerator<ValueType, StateType>::generateChoiceOrigins(std::vector<boost::any>& dataForChoiceOrigins) const {
            if (!this->getOptions().isBuildChoiceOriginsSet()) {
                return nullptr;
            }
            
            std::vector<uint_fast64_t> identifiers;
            identifiers.reserve(dataForChoiceOrigins.size());
            
            std::map<EdgeIndexSet, uint_fast64_t> edgeIndexSetToIdentifierMap;
            // The empty edge set (i.e., the choices without origin) always has to get identifier getIdentifierForChoicesWithNoOrigin() -- which is assumed to be 0
            STORM_LOG_ASSERT(storm::storage::sparse::ChoiceOrigins::getIdentifierForChoicesWithNoOrigin() == 0, "The no origin identifier is assumed to be zero");
            edgeIndexSetToIdentifierMap.insert(std::make_pair(EdgeIndexSet(), 0));
            uint_fast64_t currentIdentifier = 1;
            for (boost::any& originData : dataForChoiceOrigins) {
                STORM_LOG_ASSERT(originData.empty() || boost::any_cast<EdgeIndexSet>(&originData) != nullptr, "Origin data has unexpected type: " << originData.type().name() << ".");
                
                EdgeIndexSet currentEdgeIndexSet = originData.empty() ? EdgeIndexSet() : boost::any_cast<EdgeIndexSet>(std::move(originData));
                auto insertionRes = edgeIndexSetToIdentifierMap.emplace(std::move(currentEdgeIndexSet), currentIdentifier);
                identifiers.push_back(insertionRes.first->second);
                if (insertionRes.second) {
                    ++currentIdentifier;
                }
            }
            
            std::vector<EdgeIndexSet> identifierToEdgeIndexSetMapping(currentIdentifier);
            for (auto const& setIdPair : edgeIndexSetToIdentifierMap) {
                identifierToEdgeIndexSetMapping[setIdPair.second] = setIdPair.first;
            }
            
            return std::make_shared<storm::storage::sparse::JaniChoiceOrigins>(std::make_shared<storm::jani::Model>(model), std::move(identifiers), std::move(identifierToEdgeIndexSetMapping));
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
