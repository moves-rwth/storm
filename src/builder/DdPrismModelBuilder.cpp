#include "src/builder/DdPrismModelBuilder.h"

#include "src/models/symbolic/Dtmc.h"
#include "src/models/symbolic/Mdp.h"

#include "src/storage/dd/CuddDd.h"
#include "src/storage/dd/CuddDdManager.h"
#include "src/settings/SettingsManager.h"

#include "src/exceptions/InvalidStateException.h"

#include "src/utility/prism.h"
#include "src/utility/math.h"

namespace storm {
    namespace builder {
        
        template <storm::dd::DdType Type>
        DdPrismModelBuilder<Type>::Options::Options() : buildRewards(false), rewardModelName(), constantDefinitions() {
            // Intentionally left empty.
        }
        
        template <storm::dd::DdType Type>
        DdPrismModelBuilder<Type>::Options::Options(storm::logic::Formula const& formula) : buildRewards(formula.containsRewardOperator()), rewardModelName(), constantDefinitions(), labelsToBuild(std::set<std::string>()), expressionLabels(std::vector<storm::expressions::Expression>()) {
            // Extract all the labels used in the formula.
            std::vector<std::shared_ptr<storm::logic::AtomicLabelFormula const>> atomicLabelFormulas = formula.getAtomicLabelFormulas();
            for (auto const& formula : atomicLabelFormulas) {
                labelsToBuild.get().insert(formula.get()->getLabel());
            }
            
            // Extract all the expressions used in the formula.
            std::vector<std::shared_ptr<storm::logic::AtomicExpressionFormula const>> atomicExpressionFormulas = formula.getAtomicExpressionFormulas();
            for (auto const& formula : atomicExpressionFormulas) {
                expressionLabels.get().push_back(formula.get()->getExpression());
            }
        }
        
        template <storm::dd::DdType Type>
        void DdPrismModelBuilder<Type>::Options::addConstantDefinitionsFromString(storm::prism::Program const& program, std::string const& constantDefinitionString) {
            std::map<storm::expressions::Variable, storm::expressions::Expression> newConstantDefinitions = storm::utility::prism::parseConstantDefinitionString(program, constantDefinitionString);
            
            // If there is at least one constant that is defined, and the constant definition map does not yet exist,
            // we need to create it.
            if (!constantDefinitions && !newConstantDefinitions.empty()) {
                constantDefinitions = std::map<storm::expressions::Variable, storm::expressions::Expression>();
            }
            
            // Now insert all the entries that need to be defined.
            for (auto const& entry : newConstantDefinitions) {
                constantDefinitions.get().insert(entry);
            }
        }
        
        template <storm::dd::DdType Type>
        storm::dd::Dd<Type> DdPrismModelBuilder<Type>::createUpdateDecisionDiagram(GenerationInformation& generationInfo, storm::prism::Module const& module, storm::dd::Dd<Type> const& guard, storm::prism::Update const& update) {
            storm::dd::Dd<Type> updateDd = generationInfo.manager->getOne(true);
            
            STORM_LOG_TRACE("Translating update " << update);
            
            // Iterate over all assignments (boolean and integer) and build the DD for it.
            std::vector<storm::prism::Assignment> assignments = update.getAssignments();
            std::set<storm::expressions::Variable> assignedVariables;
            for (auto const& assignment : assignments) {
                // Record the variable as being written.
                STORM_LOG_TRACE("Assigning to variable " << generationInfo.variableToRowMetaVariableMap.at(assignment.getVariable()).getName());
                assignedVariables.insert(generationInfo.variableToRowMetaVariableMap.at(assignment.getVariable()));
                
                // Translate the written variable.
                auto const& primedMetaVariable = generationInfo.variableToColumnMetaVariableMap.at(assignment.getVariable());
                storm::dd::Dd<Type> writtenVariable = generationInfo.manager->getIdentity(primedMetaVariable);
                
                // Translate the expression that is being assigned.
                storm::dd::Dd<Type> updateExpression = generationInfo.rowExpressionAdapter->translateExpression(assignment.getExpression());
                
                // Combine the update expression with the guard.
                storm::dd::Dd<Type> result = updateExpression * guard;
                
                // Combine the variable and the assigned expression.
                result = result.equals(writtenVariable);
                result *= guard;
                
                // Restrict the transitions to the range of the written variable.
                result = result * generationInfo.manager->getRange(primedMetaVariable, true);
                
                updateDd *= result;
            }
            
            // This works under the assumption that global variables are only written in non-synchronzing commands, but
            // is not checked here.
            for (auto const& booleanVariable : generationInfo.program.getGlobalBooleanVariables()) {
                storm::expressions::Variable const& metaVariable = generationInfo.variableToRowMetaVariableMap.at(booleanVariable.getExpressionVariable());

                if (assignedVariables.find(metaVariable) == assignedVariables.end()) {
                    STORM_LOG_TRACE("Multiplying identity of variable " << booleanVariable.getName());
                    updateDd *= generationInfo.variableToIdentityMap.at(booleanVariable.getExpressionVariable());
                }
            }
            
            // All unused global integer variables do not change
            for (auto const& integerVariable : generationInfo.program.getGlobalIntegerVariables()) {
                storm::expressions::Variable const& metaVariable = generationInfo.variableToRowMetaVariableMap.at(integerVariable.getExpressionVariable());
                if (assignedVariables.find(metaVariable) == assignedVariables.end()) {
                    STORM_LOG_TRACE("Multiplying identity of variable " << integerVariable.getName());
                    updateDd *= generationInfo.variableToIdentityMap.at(integerVariable.getExpressionVariable());
                }
            }
            
            // All unassigned boolean variables need to keep their value.
            for (storm::prism::BooleanVariable const& booleanVariable : module.getBooleanVariables()) {
                storm::expressions::Variable const& metaVariable = generationInfo.variableToRowMetaVariableMap.at(booleanVariable.getExpressionVariable());
                if (assignedVariables.find(metaVariable) == assignedVariables.end()) {
                    STORM_LOG_TRACE("Multiplying identity of variable " << booleanVariable.getName());
                    updateDd *= generationInfo.variableToIdentityMap.at(booleanVariable.getExpressionVariable());
                }
            }
            
            // All unassigned integer variables need to keep their value.
            for (storm::prism::IntegerVariable const& integerVariable : module.getIntegerVariables()) {
                storm::expressions::Variable const& metaVariable = generationInfo.variableToRowMetaVariableMap.at(integerVariable.getExpressionVariable());
                if (assignedVariables.find(metaVariable) == assignedVariables.end()) {
                    STORM_LOG_TRACE("Multiplying identity of variable " << integerVariable.getName());
                    updateDd *= generationInfo.variableToIdentityMap.at(integerVariable.getExpressionVariable());
                }
            }
            
            return updateDd;
        }
        
        template <storm::dd::DdType Type>
        typename DdPrismModelBuilder<Type>::ActionDecisionDiagram DdPrismModelBuilder<Type>::createCommandDecisionDiagram(GenerationInformation& generationInfo, storm::prism::Module const& module, storm::prism::Command const& command) {
            STORM_LOG_TRACE("Translating guard " << command.getGuardExpression());
            storm::dd::Dd<Type> guardDd = generationInfo.rowExpressionAdapter->translateExpression(command.getGuardExpression()) * generationInfo.moduleToRangeMap[module.getName()];
            STORM_LOG_WARN_COND(!guardDd.isZero(), "The guard '" << command.getGuardExpression() << "' is unsatisfiable.");
            
            if (!guardDd.isZero()) {
                storm::dd::Dd<Type> commandDd = generationInfo.manager->getZero(true);
                for (storm::prism::Update const& update : command.getUpdates()) {
                    storm::dd::Dd<Type> updateDd = createUpdateDecisionDiagram(generationInfo, module, guardDd, update);
                    
                    STORM_LOG_WARN_COND(!updateDd.isZero(), "Update '" << update << "' does not have any effect.");
                    
                    storm::dd::Dd<Type> probabilityDd = generationInfo.rowExpressionAdapter->translateExpression(update.getLikelihoodExpression());
                    updateDd *= probabilityDd;
                    
                    commandDd += updateDd;
                }
                
                return ActionDecisionDiagram(guardDd, guardDd * commandDd);
            } else {
                return ActionDecisionDiagram(*generationInfo.manager);
            }
        }
        
        template <storm::dd::DdType Type>
        typename DdPrismModelBuilder<Type>::ActionDecisionDiagram DdPrismModelBuilder<Type>::createActionDecisionDiagram(GenerationInformation& generationInfo, storm::prism::Module const& module, boost::optional<uint_fast64_t> synchronizationActionIndex, uint_fast64_t nondeterminismVariableOffset) {
            std::vector<ActionDecisionDiagram> commandDds;
            for (storm::prism::Command const& command : module.getCommands()) {
                
                // Determine whether the command is relevant for the selected action.
                bool relevant = (!synchronizationActionIndex && !command.isLabeled()) || (synchronizationActionIndex && command.isLabeled() && command.getActionIndex() == synchronizationActionIndex.get());
                
                if (!relevant) {
                    continue;
                }
                
                // At this point, the command is known to be relevant for the action.
                commandDds.push_back(createCommandDecisionDiagram(generationInfo, module, command));
            }
            
            ActionDecisionDiagram result(*generationInfo.manager);
            if (!commandDds.empty()) {
                switch (generationInfo.program.getModelType()){
                    case storm::prism::Program::ModelType::DTMC:
                        result = combineCommandsToActionDTMC(generationInfo, commandDds);
                        break;
                    case storm::prism::Program::ModelType::MDP:
                        result = combineCommandsToActionMDP(generationInfo, commandDds, nondeterminismVariableOffset);
                        break;
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Cannot translate model of this type.");
                }
            }
            
            return result;
        }
        
        template <storm::dd::DdType Type>
        typename DdPrismModelBuilder<Type>::ActionDecisionDiagram DdPrismModelBuilder<Type>::combineCommandsToActionDTMC(GenerationInformation& generationInfo, std::vector<ActionDecisionDiagram> const& commandDds) {
            storm::dd::Dd<Type> allGuards = generationInfo.manager->getZero(true);
            storm::dd::Dd<Type> allCommands = generationInfo.manager->getZero(true);
            storm::dd::Dd<Type> temporary;
            
            for (auto const& commandDd : commandDds) {
                // Check for overlapping guards.
                temporary = commandDd.guardDd * allGuards;
                STORM_LOG_WARN_COND(temporary.isZero(), "Guard of a command overlaps with previous guards.");
                
                allGuards += commandDd.guardDd;
                allCommands += commandDd.guardDd * commandDd.transitionsDd;
            }
            
            return ActionDecisionDiagram(allGuards, allCommands);
        }
        
        template <storm::dd::DdType Type>
        storm::dd::Dd<Type> DdPrismModelBuilder<Type>::encodeChoice(GenerationInformation& generationInfo, uint_fast64_t nondeterminismVariableOffset, uint_fast64_t numberOfBinaryVariables, int_fast64_t value) {
            storm::dd::Dd<Type> result = generationInfo.manager->getZero(true);
            
            STORM_LOG_TRACE("Encoding " << value << " with " << numberOfBinaryVariables << " binary variable(s) starting from offset " << nondeterminismVariableOffset << ".");
            
            std::map<storm::expressions::Variable, int_fast64_t> metaVariableNameToValueMap;
            for (uint_fast64_t i = nondeterminismVariableOffset; i < nondeterminismVariableOffset + numberOfBinaryVariables; ++i) {
                if (value & (1ull << (numberOfBinaryVariables - i - 1))) {
                    metaVariableNameToValueMap.emplace(generationInfo.nondeterminismMetaVariables[i], 1);
                } else {
                    metaVariableNameToValueMap.emplace(generationInfo.nondeterminismMetaVariables[i], 0);
                }
            }
            
            result.setValue(metaVariableNameToValueMap, 1);
            return result;
        }
        
        template <storm::dd::DdType Type>
        typename DdPrismModelBuilder<Type>::ActionDecisionDiagram DdPrismModelBuilder<Type>::combineCommandsToActionMDP(GenerationInformation& generationInfo, std::vector<ActionDecisionDiagram> const& commandDds, uint_fast64_t nondeterminismVariableOffset) {
            storm::dd::Dd<Type> allGuards = generationInfo.manager->getZero(true);
            storm::dd::Dd<Type> allCommands = generationInfo.manager->getZero(true);
            
            // Sum all guards, so we can read off the maximal number of nondeterministic choices in any given state.
            storm::dd::Dd<Type> sumOfGuards = generationInfo.manager->getZero(true);
            for (auto const& commandDd : commandDds) {
                sumOfGuards += commandDd.guardDd;
                allGuards = allGuards || commandDd.guardDd;
            }
            uint_fast64_t maxChoices = static_cast<uint_fast64_t>(sumOfGuards.getMax());

            STORM_LOG_TRACE("Found " << maxChoices << " local choices.");
            
            // Depending on the maximal number of nondeterminstic choices, we need to use some variables to encode the nondeterminism.
            if (maxChoices == 0) {
                return ActionDecisionDiagram(*generationInfo.manager);
            } else if (maxChoices == 1) {
                // Sum up all commands.
                for (auto const& commandDd : commandDds) {
                    // FIXME: necessary to multiply with guard again?
                    allCommands += commandDd.guardDd * commandDd.transitionsDd;
                }
                return ActionDecisionDiagram(sumOfGuards, allCommands);
            } else {
                // Calculate number of required variables to encode the nondeterminism.
                uint_fast64_t numberOfBinaryVariables = static_cast<uint_fast64_t>(std::ceil(storm::utility::math::log2(maxChoices)));
                
                storm::dd::Dd<Type> equalsNumberOfChoicesDd = generationInfo.manager->getZero(true);
                std::vector<storm::dd::Dd<Type>> choiceDds(maxChoices, generationInfo.manager->getZero(true));
                std::vector<storm::dd::Dd<Type>> remainingDds(maxChoices, generationInfo.manager->getZero(true));
                
                for (uint_fast64_t currentChoices = 1; currentChoices <= maxChoices; ++currentChoices) {
                    // Determine the set of states with exactly currentChoices choices.
                    equalsNumberOfChoicesDd = sumOfGuards.equals(generationInfo.manager->getConstant(static_cast<double>(currentChoices)));
                    
                    // If there is no such state, continue with the next possible number of choices.
                    if (equalsNumberOfChoicesDd.isZero()) {
                        continue;
                    }
                    
                    // Reset the previously used intermediate storage.
                    for (uint_fast64_t j = 0; j < currentChoices; ++j) {
                        choiceDds[j] = generationInfo.manager->getZero(true);
                        remainingDds[j] = equalsNumberOfChoicesDd;
                    }
                    
                    for (std::size_t j = 0; j < commandDds.size(); ++j) {
                        // Check if command guard overlaps with equalsNumberOfChoicesDd. That is, there are states with exactly currentChoices
                        // choices such that one outgoing choice is given by the j-th command.
                        storm::dd::Dd<Type> guardChoicesIntersection = commandDds[j].guardDd && equalsNumberOfChoicesDd;
                        
                        // If there is no such state, continue with the next command.
                        if (guardChoicesIntersection.isZero()) {
                            continue;
                        }
                        
                        // Split the currentChoices nondeterministic choices.
                        for (uint_fast64_t k = 0; k < currentChoices; ++k) {
                            // Calculate the overlapping part of command guard and the remaining DD.
                            storm::dd::Dd<Type> remainingGuardChoicesIntersection = guardChoicesIntersection && remainingDds[k];
                            
                            // Check if we can add some overlapping parts to the current index.
                            if (!remainingGuardChoicesIntersection.isZero()) {
                                // Remove overlapping parts from the remaining DD.
                                remainingDds[k] = remainingDds[k] && !remainingGuardChoicesIntersection;
                                
                                // Combine the overlapping part of the guard with command updates and add it to the resulting DD.
                                choiceDds[k] += remainingGuardChoicesIntersection * commandDds[j].transitionsDd;
                            }
                            
                            // Remove overlapping parts from the command guard DD
                            guardChoicesIntersection = guardChoicesIntersection && !remainingGuardChoicesIntersection;
                            
                            // If the guard DD has become equivalent to false, we can stop here.
                            if (guardChoicesIntersection.isZero()) {
                                break;
                            }
                        }
                    }
                    
                    // Add the meta variables that encode the nondeterminisim to the different choices.
                    for (uint_fast64_t j = 0; j < currentChoices; ++j) {
                        allCommands += encodeChoice(generationInfo, nondeterminismVariableOffset, numberOfBinaryVariables, j) * choiceDds[j];
                    }
                    
                    // Delete currentChoices out of overlapping DD
                    sumOfGuards = sumOfGuards * (!equalsNumberOfChoicesDd);
                }
                
                return ActionDecisionDiagram(allGuards, allCommands, nondeterminismVariableOffset + numberOfBinaryVariables);
            }
        }

        template <storm::dd::DdType Type>
        typename DdPrismModelBuilder<Type>::ActionDecisionDiagram DdPrismModelBuilder<Type>::combineSynchronizingActions(GenerationInformation const& generationInfo, ActionDecisionDiagram const& action1, ActionDecisionDiagram const& action2) {
            return ActionDecisionDiagram(action1.guardDd * action2.guardDd, action1.transitionsDd * action2.transitionsDd, std::max(action1.numberOfUsedNondeterminismVariables, action2.numberOfUsedNondeterminismVariables));
        }
        
        template <storm::dd::DdType Type>
        typename DdPrismModelBuilder<Type>::ActionDecisionDiagram DdPrismModelBuilder<Type>::combineUnsynchronizedActions(GenerationInformation const& generationInfo, ActionDecisionDiagram const& action1, ActionDecisionDiagram const& action2, storm::dd::Dd<Type> const& identityDd1, storm::dd::Dd<Type> const& identityDd2) {
            storm::dd::Dd<Type> action1Extended = action1.transitionsDd * identityDd2;
            storm::dd::Dd<Type> action2Extended = action2.transitionsDd * identityDd1;

            if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::DTMC) {
                return ActionDecisionDiagram(action1.guardDd + action2.guardDd, action1Extended + action2Extended, 0);
            } else if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::MDP) {
                if (action1.transitionsDd.isZero()) {
                    return ActionDecisionDiagram(action2.guardDd, action2Extended, action2.numberOfUsedNondeterminismVariables);
                } else if (action2.transitionsDd.isZero()) {
                    return ActionDecisionDiagram(action1.guardDd, action1Extended, action1.numberOfUsedNondeterminismVariables);
                }
                
                // Bring both choices to the same number of variables that encode the nondeterminism.
                uint_fast64_t numberOfUsedNondeterminismVariables = std::max(action1.numberOfUsedNondeterminismVariables, action2.numberOfUsedNondeterminismVariables);
                if (action1.numberOfUsedNondeterminismVariables > action2.numberOfUsedNondeterminismVariables) {
                    storm::dd::Dd<Type> nondeterminisimEncoding = generationInfo.manager->getOne(true);
                    
                    for (uint_fast64_t i = action2.numberOfUsedNondeterminismVariables; i < action1.numberOfUsedNondeterminismVariables; ++i) {
                        nondeterminisimEncoding *= generationInfo.manager->getEncoding(generationInfo.nondeterminismMetaVariables[i], 0, true);
                    }
                    action2Extended *= nondeterminisimEncoding;
                } else if (action2.numberOfUsedNondeterminismVariables > action1.numberOfUsedNondeterminismVariables) {
                    storm::dd::Dd<Type> nondeterminisimEncoding = generationInfo.manager->getOne(true);
                    
                    for (uint_fast64_t i = action1.numberOfUsedNondeterminismVariables; i < action2.numberOfUsedNondeterminismVariables; ++i) {
                        nondeterminisimEncoding *= generationInfo.manager->getEncoding(generationInfo.nondeterminismMetaVariables[i], 0, true);
                    }
                    action1Extended *= nondeterminisimEncoding;
                }
                
                // Add a new variable that resolves the nondeterminism between the two choices.
                storm::dd::Dd<Type> combinedTransitions = generationInfo.manager->getEncoding(generationInfo.nondeterminismMetaVariables[numberOfUsedNondeterminismVariables], 1, true).ite(action2Extended, action1Extended);
                
                return ActionDecisionDiagram(action1.guardDd || action2.guardDd, combinedTransitions, numberOfUsedNondeterminismVariables + 1);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidStateException, "Illegal model type.");
            }
        }

        template <storm::dd::DdType Type>
        typename DdPrismModelBuilder<Type>::ModuleDecisionDiagram DdPrismModelBuilder<Type>::createModuleDecisionDiagram(GenerationInformation& generationInfo, storm::prism::Module const& module, std::map<uint_fast64_t, uint_fast64_t> const& synchronizingActionToOffsetMap) {
            // Start by creating the action DD for the independent action.
            ActionDecisionDiagram independentActionDd = createActionDecisionDiagram(generationInfo, module, boost::optional<uint_fast64_t>(), 0);
            uint_fast64_t numberOfUsedNondeterminismVariables = independentActionDd.numberOfUsedNondeterminismVariables;
            
            // Create module DD for all synchronizing actions of the module.
            std::map<uint_fast64_t, ActionDecisionDiagram> actionIndexToDdMap;
            for (auto const& actionIndex : module.getActionIndices()) {
                STORM_LOG_TRACE("Creating DD for action '" << actionIndex << "'.");
                ActionDecisionDiagram tmp = createActionDecisionDiagram(generationInfo, module, actionIndex, synchronizingActionToOffsetMap.at(actionIndex));
                numberOfUsedNondeterminismVariables = std::max(numberOfUsedNondeterminismVariables, tmp.numberOfUsedNondeterminismVariables);
                actionIndexToDdMap.emplace(actionIndex, tmp);
            }
            
            return ModuleDecisionDiagram(independentActionDd, actionIndexToDdMap, generationInfo.moduleToIdentityMap.at(module.getName()), numberOfUsedNondeterminismVariables);
        }
        
        template <storm::dd::DdType Type>
        storm::dd::Dd<Type> DdPrismModelBuilder<Type>::getSynchronizationDecisionDiagram(GenerationInformation& generationInfo, boost::optional<uint_fast64_t> const& synchronizationAction) {
            storm::dd::Dd<Type> synchronization = generationInfo.manager->getOne(true);
            for (uint_fast64_t i = 0; i < generationInfo.synchronizationMetaVariables.size(); ++i) {
                if (synchronizationAction && synchronizationAction.get() == i) {
                    synchronization *= generationInfo.manager->getEncoding(generationInfo.synchronizationMetaVariables[i], 1, true);
                } else {
                    synchronization *= generationInfo.manager->getEncoding(generationInfo.synchronizationMetaVariables[i], 0, true);
                }
            }
            return synchronization;
        }
        
        template <storm::dd::DdType Type>
        storm::dd::Dd<Type> DdPrismModelBuilder<Type>::createSystemFromModule(GenerationInformation& generationInfo, ModuleDecisionDiagram const& module) {
            // If the model is an MDP, we need to encode the nondeterminism using additional variables.
            if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::MDP) {
                storm::dd::Dd<Type> result = generationInfo.manager->getZero(true);
                
                // First, determine the highest number of nondeterminism variables that is used in any action and make
                // all actions use the same amout of nondeterminism variables.
                uint_fast64_t numberOfUsedNondeterminismVariables = module.numberOfUsedNondeterminismVariables;

                // Add variables to independent action DD.
                storm::dd::Dd<Type> nondeterminismEncoding = generationInfo.manager->getOne(true);
                for (uint_fast64_t i = module.independentAction.numberOfUsedNondeterminismVariables; i < numberOfUsedNondeterminismVariables; ++i) {
                    nondeterminismEncoding *= generationInfo.manager->getEncoding(generationInfo.nondeterminismMetaVariables[i], 0, true);
                }
                result = module.independentAction.transitionsDd * nondeterminismEncoding;

                // Add variables to synchronized action DDs.
                std::map<uint_fast64_t, storm::dd::Dd<Type>> synchronizingActionToDdMap;
                for (auto const& synchronizingAction : module.synchronizingActionToDecisionDiagramMap) {
                    nondeterminismEncoding = generationInfo.manager->getOne(true);
                    for (uint_fast64_t i = synchronizingAction.second.numberOfUsedNondeterminismVariables; i < numberOfUsedNondeterminismVariables; ++i) {
                        nondeterminismEncoding *= generationInfo.manager->getEncoding(generationInfo.nondeterminismMetaVariables[i], 0, true);
                    }
                    synchronizingActionToDdMap.emplace(synchronizingAction.first, synchronizingAction.second.transitionsDd * nondeterminismEncoding);
                }
                
                // Add variables for synchronization.
                result *= getSynchronizationDecisionDiagram(generationInfo);
                
                for (auto& synchronizingAction : synchronizingActionToDdMap) {
                    synchronizingAction.second *= getSynchronizationDecisionDiagram(generationInfo, synchronizingAction.first);
                }
                
                // Now, we can simply add all synchronizing actions to the result.
                for (auto const& synchronizingAction : synchronizingActionToDdMap) {
                    result += synchronizingAction.second;
                }

                return result;
            } else if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::DTMC) {
                // Simply add all actions.
                storm::dd::Dd<Type> result = module.independentAction.transitionsDd;
                for (auto const& synchronizingAction : module.synchronizingActionToDecisionDiagramMap) {
                    result += synchronizingAction.second.transitionsDd;
                }
                return result;
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Illegal model type.");
            }
        }
        
        template <storm::dd::DdType Type>
        std::pair<storm::dd::Dd<Type>, typename DdPrismModelBuilder<Type>::ModuleDecisionDiagram> DdPrismModelBuilder<Type>::createSystemDecisionDiagram(GenerationInformation& generationInfo) {
            // Create the initial offset mapping.
            std::map<uint_fast64_t, uint_fast64_t> synchronizingActionToOffsetMap;
            for (auto const& actionIndex : generationInfo.program.getActionIndices()) {
                synchronizingActionToOffsetMap[actionIndex] = 0;
            }

            // Start by creating DDs for the first module.
            STORM_LOG_TRACE("Translating (first) module '" << generationInfo.program.getModule(0).getName() << "'.");
            ModuleDecisionDiagram system = createModuleDecisionDiagram(generationInfo, generationInfo.program.getModule(0), synchronizingActionToOffsetMap);

            // No translate module by module and combine it with the system created thus far.
            for (uint_fast64_t i = 1; i < generationInfo.program.getNumberOfModules(); ++i) {
                storm::prism::Module const& currentModule = generationInfo.program.getModule(i);
                STORM_LOG_TRACE("Translating module '" << currentModule.getName() << "'.");
                
                // Update the offset index.
                for (auto const& actionIndex : generationInfo.program.getActionIndices()) {
                    if (system.hasSynchronizingAction(actionIndex)) {
                        synchronizingActionToOffsetMap[actionIndex] = system.synchronizingActionToDecisionDiagramMap[actionIndex].numberOfUsedNondeterminismVariables;
                    }
                }
                
                ModuleDecisionDiagram nextModule = createModuleDecisionDiagram(generationInfo, currentModule, synchronizingActionToOffsetMap);
                
                // Combine the non-synchronizing action.
                uint_fast64_t numberOfUsedNondeterminismVariables = nextModule.numberOfUsedNondeterminismVariables;
                system.independentAction = combineUnsynchronizedActions(generationInfo, system.independentAction, nextModule.independentAction, system.identity, nextModule.identity);
                numberOfUsedNondeterminismVariables = std::max(numberOfUsedNondeterminismVariables, system.independentAction.numberOfUsedNondeterminismVariables);
                
                // For all synchronizing actions that the next module does not have, we need to multiply the identity of the next module.
                for (auto& action : system.synchronizingActionToDecisionDiagramMap) {
                    if (!nextModule.hasSynchronizingAction(action.first)) {
                        action.second = combineUnsynchronizedActions(generationInfo, action.second, ActionDecisionDiagram(*generationInfo.manager), system.identity, nextModule.identity);
                    }
                }
                
                // Combine synchronizing actions.
                for (auto const& actionIndex : currentModule.getActionIndices()) {
                    if (system.hasSynchronizingAction(actionIndex)) {
                        system.synchronizingActionToDecisionDiagramMap[actionIndex] = combineSynchronizingActions(generationInfo, system.synchronizingActionToDecisionDiagramMap[actionIndex], nextModule.synchronizingActionToDecisionDiagramMap[actionIndex]);
                        numberOfUsedNondeterminismVariables = std::max(numberOfUsedNondeterminismVariables, system.synchronizingActionToDecisionDiagramMap[actionIndex].numberOfUsedNondeterminismVariables);
                    } else {
                        system.synchronizingActionToDecisionDiagramMap[actionIndex] = combineUnsynchronizedActions(generationInfo, ActionDecisionDiagram(*generationInfo.manager), nextModule.synchronizingActionToDecisionDiagramMap[actionIndex], system.identity, nextModule.identity);
                        numberOfUsedNondeterminismVariables = std::max(numberOfUsedNondeterminismVariables, system.synchronizingActionToDecisionDiagramMap[actionIndex].numberOfUsedNondeterminismVariables);
                    }
                }
                
                // Combine identity matrices.
                system.identity = system.identity * nextModule.identity;
                
                // Keep track of the number of nondeterminism variables used.
                system.numberOfUsedNondeterminismVariables = std::max(system.numberOfUsedNondeterminismVariables, numberOfUsedNondeterminismVariables);
            }
            
            storm::dd::Dd<Type> result = createSystemFromModule(generationInfo, system);
            
            // For DTMCs, we normalize each row to 1 (to account for non-determinism).
            if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::DTMC) {
                result = result / result.sumAbstract(generationInfo.columnMetaVariables);
            } else if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::MDP) {
                // For MDPs, we need to throw away the nondeterminism variables from the generation information that
                // were never used.
                for (uint_fast64_t index = system.numberOfUsedNondeterminismVariables; index < generationInfo.nondeterminismMetaVariables.size(); ++index) {
                    generationInfo.allNondeterminismVariables.erase(generationInfo.nondeterminismMetaVariables[index]);
                }
                generationInfo.nondeterminismMetaVariables.resize(system.numberOfUsedNondeterminismVariables);
            }
            
            return std::make_pair(result, system);
        }
        
        template <storm::dd::DdType Type>
        std::pair<storm::dd::Dd<Type>, storm::dd::Dd<Type>> DdPrismModelBuilder<Type>::createRewardDecisionDiagrams(GenerationInformation& generationInfo, storm::prism::RewardModel const& rewardModel, ModuleDecisionDiagram const& globalModule, storm::dd::Dd<Type> const& fullTransitionMatrix) {
            // Start by creating the state reward vector.
            storm::dd::Dd<Type> stateRewards = generationInfo.manager->getZero();
            for (auto const& stateReward : rewardModel.getStateRewards()) {
                storm::dd::Dd<Type> states = generationInfo.rowExpressionAdapter->translateExpression(stateReward.getStatePredicateExpression());
                storm::dd::Dd<Type> rewards = generationInfo.rowExpressionAdapter->translateExpression(stateReward.getRewardValueExpression());
                
                // Restrict the rewards to those states that satisfy the condition.
                rewards = states * rewards;
                
                // Perform some sanity checks.
                STORM_LOG_WARN_COND(rewards.getMin() >= 0, "The reward model assigns negative rewards to some states.");
                STORM_LOG_WARN_COND(!rewards.isZero(), "The reward model does not assign any non-zero rewards.");
                
                // Add the rewards to the global state reward vector.
                stateRewards += rewards;
            }
            
            // Then build the transition reward matrix.
            storm::dd::Dd<Type> transitionRewards = generationInfo.manager->getZero();
            for (auto const& transitionReward : rewardModel.getTransitionRewards()) {
                storm::dd::Dd<Type> states = generationInfo.rowExpressionAdapter->translateExpression(transitionReward.getStatePredicateExpression());
                storm::dd::Dd<Type> rewards = generationInfo.rowExpressionAdapter->translateExpression(transitionReward.getRewardValueExpression());
                
                storm::dd::Dd<Type> synchronization;
                storm::dd::Dd<Type> transitions;
                if (transitionReward.isLabeled()) {
                    synchronization = getSynchronizationDecisionDiagram(generationInfo, transitionReward.getActionIndex());
                    transitions = globalModule.synchronizingActionToDecisionDiagramMap.at(transitionReward.getActionIndex()).transitionsDd;
                } else {
                    synchronization = getSynchronizationDecisionDiagram(generationInfo);
                    transitions = globalModule.independentAction.transitionsDd;
                }
                
                storm::dd::Dd<Type> transitionRewardDd = synchronization * states * rewards;
                if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::MDP) {
                    transitionRewardDd += transitions.notZero() * transitionRewardDd;
                } else {
                    transitionRewardDd += transitions * transitionRewardDd;
                }
                
                // Perform some sanity checks.
                STORM_LOG_WARN_COND(transitionRewardDd.getMin() >= 0, "The reward model assigns negative rewards to some states.");
                STORM_LOG_WARN_COND(!transitionRewardDd.isZero(), "The reward model does not assign any non-zero rewards.");
                
                // Add the rewards to the global transition reward matrix.
                transitionRewards += transitionRewardDd;
            }
            
            // Scale transition rewards for DTMCs.
            if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::DTMC) {
                transitionRewards /= fullTransitionMatrix;
            }
            
            return std::make_pair(stateRewards, transitionRewards);
        }
    
        template <storm::dd::DdType Type>
        std::shared_ptr<storm::models::symbolic::Model<Type>> DdPrismModelBuilder<Type>::translateProgram(storm::prism::Program const& program, Options const& options) {
            // There might be nondeterministic variables. In that case the program must be prepared before translating.
            storm::prism::Program preparedProgram;
            if (options.constantDefinitions) {
                preparedProgram = program.defineUndefinedConstants(options.constantDefinitions.get());
            } else {
                preparedProgram = program;
            }
            
            if (preparedProgram.hasUndefinedConstants()) {
                std::vector<std::reference_wrapper<storm::prism::Constant const>> undefinedConstants = preparedProgram.getUndefinedConstants();
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
            
            preparedProgram = preparedProgram.substituteConstants();
            
            // Start by initializing the structure used for storing all information needed during the model generation.
            // In particular, this creates the meta variables used to encode the model.
            GenerationInformation generationInfo(preparedProgram);

            std::pair<storm::dd::Dd<Type>, ModuleDecisionDiagram> transitionMatrixModulePair = createSystemDecisionDiagram(generationInfo);
            storm::dd::Dd<Type> transitionMatrix = transitionMatrixModulePair.first;
            ModuleDecisionDiagram const& globalModule = transitionMatrixModulePair.second;
            
            // Finally, we build the DDs for a reward structure, if requested. It is important to do this now, because
            // we still have the uncut transition matrix, which is needed for the reward computation. This is because
            // the reward computation might divide by the transition probabilities, which must therefore never be 0.
            // However, cutting it to the reachable fragment, there might be zero probability transitions.
            boost::optional<std::pair<storm::dd::Dd<Type>, storm::dd::Dd<Type>>> stateAndTransitionRewards;
            if (options.buildRewards) {
                // If a specific reward model was selected or one with the empty name exists, select it.
                storm::prism::RewardModel rewardModel = storm::prism::RewardModel();
                if (options.rewardModelName) {
                    rewardModel = preparedProgram.getRewardModel(options.rewardModelName.get());
                } else if (preparedProgram.hasRewardModel("")) {
                    rewardModel = preparedProgram.getRewardModel("");
                } else if (preparedProgram.hasRewardModel()) {
                    // Otherwise, we select the first one.
                    rewardModel = preparedProgram.getRewardModel(0);
                }
                
                STORM_LOG_TRACE("Building reward structure.");
                stateAndTransitionRewards = createRewardDecisionDiagrams(generationInfo, rewardModel, globalModule, transitionMatrix);
            }
            
            // Cut the transition matrix to the reachable fragment of the state space.
            storm::dd::Dd<Type> initialStates = createInitialStatesDecisionDiagram(generationInfo);
            storm::dd::Dd<Type> reachableStates = computeReachableStates(generationInfo, initialStates, transitionMatrix);
            transitionMatrix *= reachableStates.toMtbdd();

            // Detect deadlocks and 1) fix them if requested 2) throw an error otherwise.
            storm::dd::Dd<Type> statesWithTransition = transitionMatrix.notZero();
            if (program.getModelType() == storm::prism::Program::ModelType::MDP) {
                statesWithTransition = statesWithTransition.existsAbstract(generationInfo.allNondeterminismVariables);
            }
            statesWithTransition = statesWithTransition.existsAbstract(generationInfo.columnMetaVariables);
            storm::dd::Dd<Type> deadlockStates = (reachableStates && !statesWithTransition).toMtbdd();

            if (!deadlockStates.isZero()) {
                // If we need to fix deadlocks, we do so now.
                if (!storm::settings::generalSettings().isDontFixDeadlocksSet()) {
                    STORM_LOG_WARN("Fixing deadlocks in " << deadlockStates.getNonZeroCount() << " states.");

                    if (program.getModelType() == storm::prism::Program::ModelType::DTMC) {
                        // For DTMCs, we can simply add the identity of the global module for all deadlock states.
                        transitionMatrix += deadlockStates * globalModule.identity;
                    } else if (program.getModelType() == storm::prism::Program::ModelType::MDP) {
                        // For MDPs, however, we need to select an action associated with the self-loop, if we do not
                        // want to attach a lot of self-loops to the deadlock states.
                        storm::dd::Dd<Type> action = generationInfo.manager->getOne(true);
                        std::for_each(generationInfo.allNondeterminismVariables.begin(), generationInfo.allNondeterminismVariables.end(), [&action,&generationInfo] (storm::expressions::Variable const& metaVariable) { action *= !generationInfo.manager->getIdentity(metaVariable); } );
                        transitionMatrix += deadlockStates * globalModule.identity * action;
                    }
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The model contains " << deadlockStates.getNonZeroCount() << " deadlock states. Please unset the option to not fix deadlocks, if you want to fix them automatically.");
                }
            }
            
            // Build the labels that can be accessed as a shortcut.
            std::map<std::string, storm::expressions::Expression> labelToExpressionMapping;
            for (auto const& label : program.getLabels()) {
                labelToExpressionMapping.emplace(label.getName(), label.getStatePredicateExpression());
            }
            
            if (program.getModelType() == storm::prism::Program::ModelType::DTMC) {
                return std::shared_ptr<storm::models::symbolic::Model<Type>>(new storm::models::symbolic::Dtmc<Type>(generationInfo.manager, reachableStates, initialStates, transitionMatrix, generationInfo.rowMetaVariables, generationInfo.rowExpressionAdapter, generationInfo.columnMetaVariables, generationInfo.columnExpressionAdapter, generationInfo.rowColumnMetaVariablePairs, labelToExpressionMapping, stateAndTransitionRewards ? stateAndTransitionRewards.get().first : boost::optional<storm::dd::Dd<Type>>(), stateAndTransitionRewards ? stateAndTransitionRewards.get().second : boost::optional<storm::dd::Dd<Type>>()));
            } else if (program.getModelType() == storm::prism::Program::ModelType::MDP) {
                return std::shared_ptr<storm::models::symbolic::Model<Type>>(new storm::models::symbolic::Mdp<Type>(generationInfo.manager, reachableStates, initialStates, transitionMatrix, generationInfo.rowMetaVariables, generationInfo.rowExpressionAdapter, generationInfo.columnMetaVariables, generationInfo.columnExpressionAdapter, generationInfo.rowColumnMetaVariablePairs, generationInfo.allNondeterminismVariables, labelToExpressionMapping, stateAndTransitionRewards ? stateAndTransitionRewards.get().first : boost::optional<storm::dd::Dd<Type>>(), stateAndTransitionRewards ? stateAndTransitionRewards.get().second : boost::optional<storm::dd::Dd<Type>>()));
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Invalid model type.");
            }
        }
        
        template <storm::dd::DdType Type>
        storm::dd::Dd<Type> DdPrismModelBuilder<Type>::createInitialStatesDecisionDiagram(GenerationInformation& generationInfo) {
            storm::dd::Dd<Type> initialStates = generationInfo.rowExpressionAdapter->translateExpression(generationInfo.program.getInitialConstruct().getInitialStatesExpression()).toBdd();
            
            for (auto const& metaVariable : generationInfo.rowMetaVariables) {
                initialStates &= generationInfo.manager->getRange(metaVariable);
            }
            
            return initialStates;
        }
        
        template <storm::dd::DdType Type>
        storm::dd::Dd<Type> DdPrismModelBuilder<Type>::computeReachableStates(GenerationInformation& generationInfo, storm::dd::Dd<Type> const& initialStates, storm::dd::Dd<Type> const& transitions) {
            storm::dd::Dd<Type> reachableStatesBdd = initialStates.toBdd();
            
            // If the model is an MDP, we can abstract from the variables encoding the nondeterminism in the model.
            storm::dd::Dd<Type> transitionBdd = transitions.notZero();
            if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::MDP) {
                transitionBdd = transitionBdd.existsAbstract(generationInfo.allNondeterminismVariables);
            }
            
            // Perform the BFS to discover all reachable states.
            bool changed = true;
            uint_fast64_t iteration = 0;
            do {
                STORM_LOG_TRACE("Iteration " << iteration << " of computing reachable states.");
                changed = false;
                storm::dd::Dd<Type> tmp = reachableStatesBdd.andExists(transitionBdd, generationInfo.rowMetaVariables);
                tmp.swapVariables(generationInfo.rowColumnMetaVariablePairs);

                storm::dd::Dd<Type> newReachableStates = tmp && (!reachableStatesBdd);
                
                // Check whether new states were indeed discovered.
                if (!newReachableStates.isZero()) {
                    changed = true;
                }
                
                reachableStatesBdd |= newReachableStates;
                ++iteration;
            } while (changed);
            
            return reachableStatesBdd;
        }
        
        // Explicitly instantiate the symbolic expression adapter
        template class DdPrismModelBuilder<storm::dd::DdType::CUDD>;
        
    } // namespace adapters
} // namespace storm


