#include "storm/generator/PrismNextStateGenerator.h"

#include <boost/container/flat_map.hpp>
#include <boost/any.hpp>

#include "storm/models/sparse/StateLabeling.h"

#include "storm/storage/expressions/SimpleValuation.h"
#include "storm/storage/sparse/PrismChoiceOrigins.h"

#include "storm/builder/jit/Distribution.h"

#include "storm/solver/SmtSolver.h"

#include "storm/utility/constants.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/exceptions/UnexpectedException.h"

namespace storm {
    namespace generator {
        
        template<typename ValueType, typename StateType>
        PrismNextStateGenerator<ValueType, StateType>::PrismNextStateGenerator(storm::prism::Program const& program, NextStateGeneratorOptions const& options) : PrismNextStateGenerator<ValueType, StateType>(program.substituteConstants(), options, false) {
            // Intentionally left empty.
        }
        
        template<typename ValueType, typename StateType>
        PrismNextStateGenerator<ValueType, StateType>::PrismNextStateGenerator(storm::prism::Program const& program, NextStateGeneratorOptions const& options, bool) : NextStateGenerator<ValueType, StateType>(program.getManager(), options), program(program), rewardModels(), hasStateActionRewards(false) {
            STORM_LOG_TRACE("Creating next-state generator for PRISM program: " << program);
            STORM_LOG_THROW(!this->program.specifiesSystemComposition(), storm::exceptions::WrongFormatException, "The explicit next-state generator currently does not support custom system compositions.");
                        
            // Only after checking validity of the program, we initialize the variable information.
            this->checkValid();
            this->variableInformation = VariableInformation(program);
            
            // Create a proper evalator.
            this->evaluator = std::make_unique<storm::expressions::ExpressionEvaluator<ValueType>>(program.getManager());
            
            if (this->options.isBuildAllRewardModelsSet()) {
                for (auto const& rewardModel : this->program.getRewardModels()) {
                    rewardModels.push_back(rewardModel);
                }
            } else {
                // Extract the reward models from the program based on the names we were given.
                for (auto const& rewardModelName : this->options.getRewardModelNames()) {
                    if (this->program.hasRewardModel(rewardModelName)) {
                        rewardModels.push_back(this->program.getRewardModel(rewardModelName));
                    } else {
                        STORM_LOG_THROW(rewardModelName.empty(), storm::exceptions::InvalidArgumentException, "Cannot build unknown reward model '" << rewardModelName << "'.");
                        STORM_LOG_THROW(this->program.getNumberOfRewardModels() == 1, storm::exceptions::InvalidArgumentException, "Reference to standard reward model is ambiguous.");
                    }
                }
                
                // If no reward model was yet added, but there was one that was given in the options, we try to build the
                // standard reward model.
                if (rewardModels.empty() && !this->options.getRewardModelNames().empty()) {
                    rewardModels.push_back(this->program.getRewardModel(0));
                }
            }
            
            // Determine whether any reward model has state action rewards.
            for (auto const& rewardModel : rewardModels) {
                hasStateActionRewards |= rewardModel.get().hasStateActionRewards();
            }
            
            // If there are terminal states we need to handle, we now need to translate all labels to expressions.
            if (this->options.hasTerminalStates()) {
                for (auto const& expressionOrLabelAndBool : this->options.getTerminalStates()) {
                    if (expressionOrLabelAndBool.first.isExpression()) {
                        this->terminalStates.push_back(std::make_pair(expressionOrLabelAndBool.first.getExpression(), expressionOrLabelAndBool.second));
                    } else {
                        if (program.hasLabel(expressionOrLabelAndBool.first.getLabel())) {
                            this->terminalStates.push_back(std::make_pair(this->program.getLabelExpression(expressionOrLabelAndBool.first.getLabel()), expressionOrLabelAndBool.second));
                        } else {
                            // If the label is not present in the program and is not a special one, we raise an error.
                            STORM_LOG_THROW(expressionOrLabelAndBool.first.getLabel() == "init" || expressionOrLabelAndBool.first.getLabel() == "deadlock", storm::exceptions::InvalidArgumentException, "Terminal states refer to illegal label '" << expressionOrLabelAndBool.first.getLabel() << "'.");
                        }
                    }
                }
            }
        }

        template<typename ValueType, typename StateType>
        void PrismNextStateGenerator<ValueType, StateType>::checkValid() const {
            // If the program still contains undefined constants and we are not in a parametric setting, assemble an appropriate error message.
#ifdef STORM_HAVE_CARL
            if (!std::is_same<ValueType, storm::RationalFunction>::value && program.hasUndefinedConstants()) {
#else
            if (program.hasUndefinedConstants()) {
#endif
                std::vector<std::reference_wrapper<storm::prism::Constant const>> undefinedConstants = program.getUndefinedConstants();
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
            else if (std::is_same<ValueType, storm::RationalFunction>::value && !program.undefinedConstantsAreGraphPreserving()) {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The program contains undefined constants that appear in some places other than update probabilities and reward value expressions, which is not admitted.");
            }
#endif
        }
        
        template<typename ValueType, typename StateType>
        ModelType PrismNextStateGenerator<ValueType, StateType>::getModelType() const {
            switch (program.getModelType()) {
                case storm::prism::Program::ModelType::DTMC: return ModelType::DTMC;
                case storm::prism::Program::ModelType::CTMC: return ModelType::CTMC;
                case storm::prism::Program::ModelType::MDP: return ModelType::MDP;
                case storm::prism::Program::ModelType::MA: return ModelType::MA;
                case storm::prism::Program::ModelType::POMDP: return ModelType::POMDP;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Invalid model type.");
            }
        }
        
        template<typename ValueType, typename StateType>
        bool PrismNextStateGenerator<ValueType, StateType>::isDeterministicModel() const {
            return program.isDeterministicModel();
        }
        
        template<typename ValueType, typename StateType>
        bool PrismNextStateGenerator<ValueType, StateType>::isDiscreteTimeModel() const {
            return program.isDiscreteTimeModel();
        }

        template<typename ValueType,typename StateType>
        bool PrismNextStateGenerator<ValueType, StateType>::isPartiallyObservable() const {
            return program.isPartiallyObservable();
        }
        
        template<typename ValueType, typename StateType>
        std::vector<StateType> PrismNextStateGenerator<ValueType, StateType>::getInitialStates(StateToIdCallback const& stateToIdCallback) {
            // Prepare an SMT solver to enumerate all initial states.
            storm::utility::solver::SmtSolverFactory factory;
            std::unique_ptr<storm::solver::SmtSolver> solver = factory.create(program.getManager());
            
            std::vector<storm::expressions::Expression> rangeExpressions = program.getAllRangeExpressions();
            for (auto const& expression : rangeExpressions) {
                solver->add(expression);
            }
            solver->add(program.getInitialStatesExpression());
            
            // Proceed ss long as the solver can still enumerate initial states.
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
                
                // Register initial state and return it.
                StateType id = stateToIdCallback(initialState);
                initialStateIndices.push_back(id);
                
                // Block the current initial state to search for the next one.
                if (!blockingExpression.isInitialized()) {
                    break;
                }
                solver->add(blockingExpression);
            }
            
            return initialStateIndices;
        }
        
        template<typename ValueType, typename StateType>
        StateBehavior<ValueType, StateType> PrismNextStateGenerator<ValueType, StateType>::expand(StateToIdCallback const& stateToIdCallback) {
            // Prepare the result, in case we return early.
            StateBehavior<ValueType, StateType> result;
            
            // First, construct the state rewards, as we may return early if there are no choices later and we already
            // need the state rewards then.
            for (auto const& rewardModel : rewardModels) {
                ValueType stateRewardValue = storm::utility::zero<ValueType>();
                if (rewardModel.get().hasStateRewards()) {
                    for (auto const& stateReward : rewardModel.get().getStateRewards()) {
                        if (this->evaluator->asBool(stateReward.getStatePredicateExpression())) {
                            stateRewardValue += ValueType(this->evaluator->asRational(stateReward.getRewardValueExpression()));
                        }
                    }
                }
                result.addStateReward(stateRewardValue);
            }
            
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
            std::vector<Choice<ValueType>> allChoices = getUnlabeledChoices(*this->state, stateToIdCallback);
            std::vector<Choice<ValueType>> allLabeledChoices = getLabeledChoices(*this->state, stateToIdCallback);
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
                    
                    if (this->options.isBuildChoiceLabelsSet() && choice.hasLabels()) {
                        globalChoice.addLabels(choice.getLabels());
                    }
                    
                    if (this->options.isBuildChoiceOriginsSet() && choice.hasOriginData()) {
                        globalChoice.addOriginData(choice.getOriginData());
                    }
                }
                
                // Now construct the state-action reward for all selected reward models.
                for (auto const& rewardModel : rewardModels) {
                    ValueType stateActionRewardValue = storm::utility::zero<ValueType>();
                    if (rewardModel.get().hasStateActionRewards()) {
                        for (auto const& stateActionReward : rewardModel.get().getStateActionRewards()) {
                            for (auto const& choice : allChoices) {
                                if (stateActionReward.getActionIndex() == choice.getActionIndex() && this->evaluator->asBool(stateActionReward.getStatePredicateExpression())) {
                                    stateActionRewardValue += ValueType(this->evaluator->asRational(stateActionReward.getRewardValueExpression())) * choice.getTotalMass();
                                }
                            }
                            
                        }
                    }
                    if (hasStateActionRewards) {
                        globalChoice.addReward(stateActionRewardValue / totalExitRate);
                    }
                }
                
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
        CompressedState PrismNextStateGenerator<ValueType, StateType>::applyUpdate(CompressedState const& state, storm::prism::Update const& update) {
            CompressedState newState(state);
            
            // NOTE: the following process assumes that the assignments of the update are ordered in such a way that the
            // assignments to boolean variables precede the assignments to all integer variables and that within the
            // types, the assignments to variables are ordered (in ascending order) by the expression variables.
            // This is guaranteed for PRISM models, by sorting the assignments as soon as an update is created.
            
            auto assignmentIt = update.getAssignments().begin();
            auto assignmentIte = update.getAssignments().end();
            
            // Iterate over all boolean assignments and carry them out.
            auto boolIt = this->variableInformation.booleanVariables.begin();
            for (; assignmentIt != assignmentIte && assignmentIt->getExpression().hasBooleanType(); ++assignmentIt) {
                while (assignmentIt->getVariable() != boolIt->variable) {
                    ++boolIt;
                }
                newState.set(boolIt->bitOffset, this->evaluator->asBool(assignmentIt->getExpression()));
            }
            
            // Iterate over all integer assignments and carry them out.
            auto integerIt = this->variableInformation.integerVariables.begin();
            for (; assignmentIt != assignmentIte && assignmentIt->getExpression().hasIntegerType(); ++assignmentIt) {
                while (assignmentIt->getVariable() != integerIt->variable) {
                    ++integerIt;
                }
                int_fast64_t assignedValue = this->evaluator->asInt(assignmentIt->getExpression());
                if (this->options.isExplorationChecksSet()) {
                    STORM_LOG_THROW(assignedValue >= integerIt->lowerBound, storm::exceptions::WrongFormatException, "The update " << update << " leads to an out-of-bounds value (" << assignedValue << ") for the variable '" << assignmentIt->getVariableName() << "'.");
                    STORM_LOG_THROW(assignedValue <= integerIt->upperBound, storm::exceptions::WrongFormatException, "The update " << update << " leads to an out-of-bounds value (" << assignedValue << ") for the variable '" << assignmentIt->getVariableName() << "'.");
                }
                newState.setFromInt(integerIt->bitOffset, integerIt->bitWidth, assignedValue - integerIt->lowerBound);
                STORM_LOG_ASSERT(static_cast<int_fast64_t>(newState.getAsInt(integerIt->bitOffset, integerIt->bitWidth)) + integerIt->lowerBound == assignedValue, "Writing to the bit vector bucket failed (read " << newState.getAsInt(integerIt->bitOffset, integerIt->bitWidth) << " but wrote " << assignedValue << ").");
            }
            
            // Check that we processed all assignments.
            STORM_LOG_ASSERT(assignmentIt == assignmentIte, "Not all assignments were consumed.");
            
            return newState;
        }
        
        template<typename ValueType, typename StateType>
        boost::optional<std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>>> PrismNextStateGenerator<ValueType, StateType>::getActiveCommandsByActionIndex(uint_fast64_t const& actionIndex) {
            boost::optional<std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>>> result((std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>>()));
                        
            // Iterate over all modules.
            for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
                storm::prism::Module const& module = program.getModule(i);
                
                // If the module has no command labeled with the given action, we can skip this module.
                if (!module.hasActionIndex(actionIndex)) {
                    continue;
                }
                
                std::set<uint_fast64_t> const& commandIndices = module.getCommandIndicesByActionIndex(actionIndex);
                
                // If the module contains the action, but there is no command in the module that is labeled with
                // this action, we don't have any feasible command combinations.
                if (commandIndices.empty()) {
                    return boost::optional<std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>>>();
                }
                
                std::vector<std::reference_wrapper<storm::prism::Command const>> commands;
                
                // Look up commands by their indices and add them if the guard evaluates to true in the given state.
                for (uint_fast64_t commandIndex : commandIndices) {
                    storm::prism::Command const& command = module.getCommand(commandIndex);
                    if (this->evaluator->asBool(command.getGuardExpression())) {
                        commands.push_back(command);
                    }
                }
                
                // If there was no enabled command although the module has some command with the required action label,
                // we must not return anything.
                if (commands.size() == 0) {
                    return boost::none;
                }
                
                result.get().push_back(std::move(commands));
            }
            
            STORM_LOG_ASSERT(!result->empty(), "Expected non-empty list.");
            return result;
        }
        
        template<typename ValueType, typename StateType>
        std::vector<Choice<ValueType>> PrismNextStateGenerator<ValueType, StateType>::getUnlabeledChoices(CompressedState const& state, StateToIdCallback stateToIdCallback) {
            std::vector<Choice<ValueType>> result;
            
            // Iterate over all modules.
            for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
                storm::prism::Module const& module = program.getModule(i);
                
                // Iterate over all commands.
                for (uint_fast64_t j = 0; j < module.getNumberOfCommands(); ++j) {
                    storm::prism::Command const& command = module.getCommand(j);
                    
                    // Only consider unlabeled commands.
                    if (command.isLabeled()) continue;
                    
                    // Skip the command, if it is not enabled.
                    if (!this->evaluator->asBool(command.getGuardExpression())) {
                        continue;
                    }
                    
                    result.push_back(Choice<ValueType>(command.getActionIndex(), command.isMarkovian()));
                    Choice<ValueType>& choice = result.back();
                    
                    // Remember the choice origin only if we were asked to.
                    if (this->options.isBuildChoiceOriginsSet()) {
                        CommandSet commandIndex { command.getGlobalIndex() };
                        choice.addOriginData(boost::any(std::move(commandIndex)));
                    }
                    
                    // Iterate over all updates of the current command.
                    ValueType probabilitySum = storm::utility::zero<ValueType>();
                    for (uint_fast64_t k = 0; k < command.getNumberOfUpdates(); ++k) {
                        storm::prism::Update const& update = command.getUpdate(k);

                        ValueType probability = this->evaluator->asRational(update.getLikelihoodExpression());
                        if (probability != storm::utility::zero<ValueType>()) {
                            // Obtain target state index and add it to the list of known states. If it has not yet been
                            // seen, we also add it to the set of states that have yet to be explored.
                            StateType stateIndex = stateToIdCallback(applyUpdate(state, update));
                            
                            // Update the choice by adding the probability/target state to it.
                            choice.addProbability(stateIndex, probability);
                            if (this->options.isExplorationChecksSet()) {
                                probabilitySum += probability;
                            }
                        }
                    }
                    
                    // Create the state-action reward for the newly created choice.
                    for (auto const& rewardModel : rewardModels) {
                        ValueType stateActionRewardValue = storm::utility::zero<ValueType>();
                        if (rewardModel.get().hasStateActionRewards()) {
                            for (auto const& stateActionReward : rewardModel.get().getStateActionRewards()) {
                                if (stateActionReward.getActionIndex() == choice.getActionIndex() && this->evaluator->asBool(stateActionReward.getStatePredicateExpression())) {
                                    stateActionRewardValue += ValueType(this->evaluator->asRational(stateActionReward.getRewardValueExpression()));
                                }
                            }
                        }
                        choice.addReward(stateActionRewardValue);
                    }
                    
                    if (this->options.isExplorationChecksSet()) {
                        // Check that the resulting distribution is in fact a distribution.
                        STORM_LOG_THROW(!program.isDiscreteTimeModel() || this->comparator.isOne(probabilitySum), storm::exceptions::WrongFormatException, "Probabilities do not sum to one for command '" << command << "' (actually sum to " << probabilitySum << ").");
                    }
                }
            }
            
            return result;
        }
        
        template<typename ValueType, typename StateType>
        std::vector<Choice<ValueType>> PrismNextStateGenerator<ValueType, StateType>::getLabeledChoices(CompressedState const& state, StateToIdCallback stateToIdCallback) {
            std::vector<Choice<ValueType>> result;

            storm::builder::jit::Distribution<CompressedState, ValueType> currentDistribution;
            storm::builder::jit::Distribution<CompressedState, ValueType> nextDistribution;

            for (uint_fast64_t actionIndex : program.getSynchronizingActionIndices()) {
                boost::optional<std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>>> optionalActiveCommandLists = getActiveCommandsByActionIndex(actionIndex);
                
                // Only process this action label, if there is at least one feasible solution.
                if (optionalActiveCommandLists) {
                    std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>> const& activeCommandList = optionalActiveCommandLists.get();
                    std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>::const_iterator> iteratorList(activeCommandList.size());
                    
                    // Initialize the list of iterators.
                    for (size_t i = 0; i < activeCommandList.size(); ++i) {
                        iteratorList[i] = activeCommandList[i].cbegin();
                    }
                    
                    // As long as there is one feasible combination of commands, keep on expanding it.
                    bool done = false;
                    while (!done) {
                        currentDistribution.clear();
                        nextDistribution.clear();

                        currentDistribution.add(state, storm::utility::one<ValueType>());
                        
                        for (uint_fast64_t i = 0; i < iteratorList.size(); ++i) {
                            storm::prism::Command const& command = *iteratorList[i];
                            for (uint_fast64_t j = 0; j < command.getNumberOfUpdates(); ++j) {
                                storm::prism::Update const& update = command.getUpdate(j);
                                
                                for (auto const& stateProbability : currentDistribution) {
                                    ValueType probability = stateProbability.getValue() * this->evaluator->asRational(update.getLikelihoodExpression());

                                    if (!storm::utility::isZero<ValueType>(probability)) {
                                        // Compute the new state under the current update and add it to the set of new target states.
                                        CompressedState newTargetState = applyUpdate(stateProbability.getState(), update);
                                        nextDistribution.add(newTargetState, probability);
                                    }
                                }
                            }
                            
                            nextDistribution.compress();
                            
                            // If there is one more command to come, shift the target states one time step back.
                            if (i < iteratorList.size() - 1) {
                                currentDistribution = std::move(nextDistribution);
                            }
                        }
                        
                        // At this point, we applied all commands of the current command combination and newTargetStates
                        // contains all target states and their respective probabilities. That means we are now ready to
                        // add the choice to the list of transitions.
                        result.push_back(Choice<ValueType>(actionIndex));
                        
                        // Now create the actual distribution.
                        Choice<ValueType>& choice = result.back();
                        
                        // Remember the choice label and origins only if we were asked to.
                        if (this->options.isBuildChoiceLabelsSet()) {
                            choice.addLabel(program.getActionName(actionIndex));
                        }
                        if (this->options.isBuildChoiceOriginsSet()) {
                            CommandSet commandIndices;
                            for (uint_fast64_t i = 0; i < iteratorList.size(); ++i) {
                                commandIndices.insert(iteratorList[i]->get().getGlobalIndex());
                            }
                            choice.addOriginData(boost::any(std::move(commandIndices)));
                        }
                        
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
                            STORM_LOG_THROW(!program.isDiscreteTimeModel() || !this->comparator.isConstant(probabilitySum) || this->comparator.isOne(probabilitySum), storm::exceptions::WrongFormatException, "Sum of update probabilities do not some to one for some command (actually sum to " << probabilitySum << ").");
                        }
                        
                        // Create the state-action reward for the newly created choice.
                        for (auto const& rewardModel : rewardModels) {
                            ValueType stateActionRewardValue = storm::utility::zero<ValueType>();
                            if (rewardModel.get().hasStateActionRewards()) {
                                for (auto const& stateActionReward : rewardModel.get().getStateActionRewards()) {
                                    if (stateActionReward.getActionIndex() == choice.getActionIndex() && this->evaluator->asBool(stateActionReward.getStatePredicateExpression())) {
                                        stateActionRewardValue += ValueType(this->evaluator->asRational(stateActionReward.getRewardValueExpression()));
                                    }
                                }
                            }
                            choice.addReward(stateActionRewardValue);
                        }
                        
                        // Now, check whether there is one more command combination to consider.
                        bool movedIterator = false;
                        for (int_fast64_t j = iteratorList.size() - 1; !movedIterator && j >= 0; --j) {
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
            
            return result;
        }
        
        template<typename ValueType, typename StateType>
        storm::models::sparse::StateLabeling PrismNextStateGenerator<ValueType, StateType>::label(storm::storage::sparse::StateStorage<StateType> const& stateStorage, std::vector<StateType> const& initialStateIndices, std::vector<StateType> const& deadlockStateIndices) {
            // Gather a vector of labels and their expressions.
            std::vector<std::pair<std::string, storm::expressions::Expression>> labels;
            if (this->options.isBuildAllLabelsSet()) {
                for (auto const& label : program.getLabels()) {
                    labels.push_back(std::make_pair(label.getName(), label.getStatePredicateExpression()));
                }
            } else {
                for (auto const& labelName : this->options.getLabelNames()) {
                    if (program.hasLabel(labelName)) {
                        labels.push_back(std::make_pair(labelName, program.getLabelExpression(labelName)));
                    } else {
                        STORM_LOG_THROW(labelName == "init" || labelName == "deadlock", storm::exceptions::InvalidArgumentException, "Cannot build labeling for unknown label '" << labelName << "'.");
                    }
                }
            }
            
            return NextStateGenerator<ValueType, StateType>::label(stateStorage, initialStateIndices, deadlockStateIndices, labels);
        }
        
        template<typename ValueType, typename StateType>
        std::size_t PrismNextStateGenerator<ValueType, StateType>::getNumberOfRewardModels() const {
            return rewardModels.size();
        }
        
        template<typename ValueType, typename StateType>
        storm::builder::RewardModelInformation PrismNextStateGenerator<ValueType, StateType>::getRewardModelInformation(uint64_t const& index) const {
            storm::prism::RewardModel const& rewardModel = rewardModels[index].get();
            return storm::builder::RewardModelInformation(rewardModel.getName(), rewardModel.hasStateRewards(), rewardModel.hasStateActionRewards(), rewardModel.hasTransitionRewards());
        }
        
        template<typename ValueType, typename StateType>
        std::shared_ptr<storm::storage::sparse::ChoiceOrigins> PrismNextStateGenerator<ValueType, StateType>::generateChoiceOrigins(std::vector<boost::any>& dataForChoiceOrigins) const {
            
            if (!this->getOptions().isBuildChoiceOriginsSet()) {
                return nullptr;
            }
            
            std::vector<uint_fast64_t> identifiers;
            identifiers.reserve(dataForChoiceOrigins.size());
            
            std::map<CommandSet, uint_fast64_t> commandSetToIdentifierMap;
            // The empty commandset (i.e., the choices without origin) always has to get identifier getIdentifierForChoicesWithNoOrigin() -- which is assumed to be 0
            STORM_LOG_ASSERT(storm::storage::sparse::ChoiceOrigins::getIdentifierForChoicesWithNoOrigin() == 0, "The no origin identifier is assumed to be zero");
            commandSetToIdentifierMap.insert(std::make_pair(CommandSet(), 0));
            uint_fast64_t currentIdentifier = 1;
            for (boost::any& originData : dataForChoiceOrigins) {
                STORM_LOG_ASSERT(originData.empty() || boost::any_cast<CommandSet>(&originData) != nullptr, "Origin data has unexpected type: " << originData.type().name() << ".");
                
                CommandSet currentCommandSet = originData.empty() ? CommandSet() : boost::any_cast<CommandSet>(std::move(originData));
                auto insertionRes = commandSetToIdentifierMap.insert(std::make_pair(std::move(currentCommandSet), currentIdentifier));
                identifiers.push_back(insertionRes.first->second);
                if (insertionRes.second) {
                    ++currentIdentifier;
                }
            }
            
            std::vector<CommandSet> identifierToCommandSetMapping(currentIdentifier);
            for (auto const& setIdPair : commandSetToIdentifierMap) {
                identifierToCommandSetMapping[setIdPair.second] = setIdPair.first;
            }
            
            return std::make_shared<storm::storage::sparse::PrismChoiceOrigins>(std::make_shared<storm::prism::Program>(program), std::move(identifiers), std::move(identifierToCommandSetMapping));
        }

                
        template class PrismNextStateGenerator<double>;

#ifdef STORM_HAVE_CARL
        template class PrismNextStateGenerator<storm::RationalNumber>;
        template class PrismNextStateGenerator<storm::RationalFunction>;
#endif
    }
}
