#include "src/builder/DdPrismModelBuilder.h"

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
            storm::dd::Dd<Type> updateDd = generationInfo.manager->getOne();
            
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
                result = result * generationInfo.manager->getRange(primedMetaVariable);
                
                updateDd *= result;
            }
            
            // FIXME: global boolean variables cause problems. They cannot be added here, because then synchronization goes wrong.
//            for (uint_fast64_t i = 0; i < generationInfo.program.getGlobalBooleanVariables().size(); ++i) {
//                storm::prism::BooleanVariable const& booleanVariable = generationInfo.program.getGlobalBooleanVariables().at(i);
//                if (update.getAssignmentMapping().find(booleanVariable.getName()) == update.getAssignmentMapping().end()) {
//                    // Multiply identity matrix
//                    updateDd = updateDd * generationInfo.variableToIdentityDecisionDiagramMap.at(booleanVariable.getName());
//                }
//            }
//            
//            // All unused global integer variables do not change
//            for (uint_fast64_t i = 0; i < generationInfo.program.getGlobalIntegerVariables().size(); ++i) {
//                storm::prism::IntegerVariable const& integerVariable = generationInfo.program.getGlobalIntegerVariables().at(i);
//                if (update.getAssignmentMapping().find(integerVariable.getName()) == update.getAssignmentMapping().end()) {
//                    // Multiply identity matrix
//                    updateDd = updateDd * generationInfo.variableToIdentityDecisionDiagramMap.at(integerVariable.getName());
//                }
//            }
            
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
            storm::dd::Dd<Type> guardDd = generationInfo.rowExpressionAdapter->translateExpression(command.getGuardExpression());
            STORM_LOG_WARN_COND(!guardDd.isZero(), "The guard '" << command.getGuardExpression() << "' is unsatisfiable.");
            
            if (!guardDd.isZero()) {
                storm::dd::Dd<Type> commandDd = generationInfo.manager->getZero();
                for (storm::prism::Update const& update : command.getUpdates()) {
                    storm::dd::Dd<Type> updateDd = createUpdateDecisionDiagram(generationInfo, module, guardDd, update);
                    
                    STORM_LOG_WARN_COND(!updateDd.isZero(), "Update '" << update << "' does not have any effect.");
                    
                    storm::dd::Dd<Type> probabilityDd = generationInfo.rowExpressionAdapter->translateExpression(update.getLikelihoodExpression());
                    updateDd *= probabilityDd;
                    
                    commandDd += updateDd;
                }
                
                guardDd.exportToDot(module.getName() + "_cmd_" + std::to_string(command.getGlobalIndex()) + "_grd.dot");
                commandDd.exportToDot(module.getName() + "_cmd_" + std::to_string(command.getGlobalIndex()) + ".dot");
                
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
                bool relevant = (!synchronizationActionIndex && !command.isLabeled()) || (synchronizationActionIndex && command.getActionIndex() == synchronizationActionIndex.get());
                
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
            storm::dd::Dd<Type> allGuards = generationInfo.manager->getZero();
            storm::dd::Dd<Type> allCommands = generationInfo.manager->getZero();
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
            storm::dd::Dd<Type> result = generationInfo.manager->getOne();
            
            std::map<storm::expressions::Variable, int_fast64_t> metaVariableNameToValueMap;
            for (uint_fast64_t i = nondeterminismVariableOffset; i < nondeterminismVariableOffset + numberOfBinaryVariables; ++i) {
                if (value & (1ull << (numberOfBinaryVariables - i - 1))) {
                    metaVariableNameToValueMap.emplace(generationInfo.nondeterminismMetaVariables[i], 1);
                }
                else {
                    metaVariableNameToValueMap.emplace(generationInfo.nondeterminismMetaVariables[i], 0);
                }
            }
            
            result.setValue(metaVariableNameToValueMap, 1);
            return result;
        }
        
        template <storm::dd::DdType Type>
        typename DdPrismModelBuilder<Type>::ActionDecisionDiagram DdPrismModelBuilder<Type>::combineCommandsToActionMDP(GenerationInformation& generationInfo, std::vector<ActionDecisionDiagram> const& commandDds, uint_fast64_t nondeterminismVariableOffset) {
            storm::dd::Dd<Type> allGuards = generationInfo.manager->getZero();
            storm::dd::Dd<Type> allCommands = generationInfo.manager->getZero();
            
            // Sum all guards, so we can read off the maximal number of nondeterministic choices in any given state.
            storm::dd::Dd<Type> sumOfGuards = generationInfo.manager->getZero();
            for (auto const& commandDd : commandDds) {
                sumOfGuards += commandDd.guardDd;
                allGuards = allGuards || commandDd.guardDd;
            }
            uint_fast64_t maxChoices = static_cast<uint_fast64_t>(sumOfGuards.getMax());

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
                
                storm::dd::Dd<Type> equalsNumberOfChoicesDd = generationInfo.manager->getZero();
                std::vector<storm::dd::Dd<Type>> choiceDds(maxChoices, generationInfo.manager->getZero());
                std::vector<storm::dd::Dd<Type>> remainingDds(maxChoices, generationInfo.manager->getZero());
                
                for (uint_fast64_t currentChoices = 1; currentChoices <= maxChoices; ++currentChoices) {
                    // Determine the set of states with exactly currentChoices choices.
                    equalsNumberOfChoicesDd = sumOfGuards.equals(generationInfo.manager->getConstant(static_cast<double>(currentChoices)));
                    
                    // If there is no such state, continue with the next possible number of choices.
                    if (equalsNumberOfChoicesDd.isZero()) {
                        continue;
                    }
                    
                    // Reset the previously used intermediate storage.
                    for (uint_fast64_t j = 0; j < currentChoices; ++j) {
                        choiceDds[j] = generationInfo.manager->getZero();
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
                            
                            // Check if we can add some overlapping parts to the current index
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
                    storm::dd::Dd<Type> nondeterminisimEncoding = generationInfo.manager->getOne();
                    
                    for (uint_fast64_t i = action2.numberOfUsedNondeterminismVariables + 1; i <= action1.numberOfUsedNondeterminismVariables; ++i) {
                        nondeterminisimEncoding *= generationInfo.manager->getEncoding(generationInfo.nondeterminismMetaVariables[i], 0);
                    }
                    action2Extended *= nondeterminisimEncoding;
                } else if (action2.numberOfUsedNondeterminismVariables > action1.numberOfUsedNondeterminismVariables) {
                    storm::dd::Dd<Type> nondeterminisimEncoding = generationInfo.manager->getOne();
                    
                    for (uint_fast64_t i = action1.numberOfUsedNondeterminismVariables + 1; i <= action2.numberOfUsedNondeterminismVariables; ++i) {
                        nondeterminisimEncoding *= generationInfo.manager->getEncoding(generationInfo.nondeterminismMetaVariables[i], 0);
                    }
                    action1Extended *= nondeterminisimEncoding;
                }
                
                // Add a new variable that resolves the nondeterminism between the two choices.
                storm::dd::Dd<Type> combinedTransitions = generationInfo.manager->getEncoding(generationInfo.nondeterminismMetaVariables[numberOfUsedNondeterminismVariables + 1], 1).ite(action2Extended, action1Extended);
                
                return ActionDecisionDiagram(action1.guardDd || action2.guardDd, combinedTransitions, numberOfUsedNondeterminismVariables + 1);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidStateException, "Illegal model type.");
            }
        }

        template <storm::dd::DdType Type>
        typename DdPrismModelBuilder<Type>::ModuleDecisionDiagram DdPrismModelBuilder<Type>::createModuleDecisionDiagram(GenerationInformation& generationInfo, storm::prism::Module const& module, std::map<uint_fast64_t, uint_fast64_t> const& synchronizingActionToOffsetMap) {
            // Start by creating the action DD for the independent action.
            ActionDecisionDiagram independentActionDd = createActionDecisionDiagram(generationInfo, module, boost::optional<uint_fast64_t>(), 0);
            
            // Create module DD for all synchronizing actions of the module.
            std::map<uint_fast64_t, ActionDecisionDiagram> actionIndexToDdMap;
            for (auto const& actionIndex : module.getActionIndices()) {
                STORM_LOG_TRACE("Creating DD for action '" << actionIndex << "'.");
                actionIndexToDdMap.emplace(actionIndex, createActionDecisionDiagram(generationInfo, module, actionIndex, synchronizingActionToOffsetMap.at(actionIndex)));
            }
            
            return ModuleDecisionDiagram(independentActionDd, actionIndexToDdMap, generationInfo.moduleToIdentityMap.at(module.getName()));
        }
        
        template <storm::dd::DdType Type>
        storm::dd::Dd<Type> DdPrismModelBuilder<Type>::createSystemFromModule(GenerationInformation& generationInfo, ModuleDecisionDiagram const& module) {
            // If the model is an MDP, we need to encode the nondeterminism using additional variables.
            if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::MDP) {
                storm::dd::Dd<Type> result = generationInfo.manager->getZero();
                
                // First, determine the maximal variable index that is used.
                uint_fast64_t numberOfUsedNondeterminismVariables = module.independentAction.numberOfUsedNondeterminismVariables;
                for (auto const& synchronizingAction : module.synchronizingActionToDecisionDiagramMap) {
                    numberOfUsedNondeterminismVariables = std::max(numberOfUsedNondeterminismVariables, synchronizingAction.second.numberOfUsedNondeterminismVariables);
                }

                // Now make all actions use the same amount of nondeterminism variables.
                
                // Add variables to independent action DD.
                storm::dd::Dd<Type> nondeterminismEncoding = generationInfo.manager->getOne();
                for (uint_fast64_t i = module.independentAction.numberOfUsedNondeterminismVariables + 1; i <= numberOfUsedNondeterminismVariables; ++i) {
                    nondeterminismEncoding *= generationInfo.manager->getEncoding(generationInfo.nondeterminismMetaVariables[i], 0);
                }
                result = module.independentAction.transitionsDd * nondeterminismEncoding;

                // Add variables to synchronized action DDs.
                std::map<uint_fast64_t, storm::dd::Dd<Type>> synchronizingActionToDdMap;
                for (auto const& synchronizingAction : module.synchronizingActionToDecisionDiagramMap) {
                    nondeterminismEncoding = generationInfo.manager->getOne();
                    for (uint_fast64_t i = synchronizingAction.second.numberOfUsedNondeterminismVariables + 1; i <= numberOfUsedNondeterminismVariables; ++i) {
                        nondeterminismEncoding *= generationInfo.manager->getEncoding(generationInfo.nondeterminismMetaVariables[i], 0);
                    }
                    synchronizingActionToDdMap.emplace(synchronizingAction.first, synchronizingAction.second.transitionsDd * nondeterminismEncoding);
                }
                
                // Add variables for synchronization.
                storm::dd::Dd<Type> synchronization = generationInfo.manager->getOne();
                for (uint_fast64_t i = 0; i < generationInfo.synchronizationMetaVariables.size(); ++i) {
                    synchronization *= generationInfo.manager->getEncoding(generationInfo.synchronizationMetaVariables[i], 0);
                }
                result *= synchronization;
                
                for (auto& synchronizingAction : synchronizingActionToDdMap) {
                    synchronization = generationInfo.manager->getOne();
                    
                    for (uint_fast64_t i = 0; i < generationInfo.synchronizationMetaVariables.size(); ++i) {
                        if (i == synchronizingAction.first) {
                            synchronization *= generationInfo.manager->getEncoding(generationInfo.synchronizationMetaVariables[i], 1);
                        } else {
                            synchronization *= generationInfo.manager->getEncoding(generationInfo.synchronizationMetaVariables[i], 0);
                        }
                    }
                    
                    synchronizingAction.second *= synchronization;
                }
                
                // Now, we can simply add all synchronizing actions to the result.
                for (auto const& synchronizingAction : module.synchronizingActionToDecisionDiagramMap) {
                    result += synchronizingAction.second.transitionsDd;
                }

                return result;
            } else {
                // Simply add all actions.
                storm::dd::Dd<Type> result = module.independentAction.transitionsDd;
                for (auto const& synchronizingAction : module.synchronizingActionToDecisionDiagramMap) {
                    result += synchronizingAction.second.transitionsDd;
                }
                return result;
            }
        }
        
        template <storm::dd::DdType Type>
        storm::dd::Dd<Type> DdPrismModelBuilder<Type>::createSystemDecisionDiagram(GenerationInformation& generationInfo) {
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
                system.independentAction = combineUnsynchronizedActions(generationInfo, system.independentAction, nextModule.independentAction, system.identity, nextModule.identity);
                
                // Combine synchronizing actions.
                for (auto const& actionIndex : currentModule.getActionIndices()) {
                    std::cout << "treating action index " << actionIndex << std::endl;
                    if (system.hasSynchronizingAction(actionIndex)) {
                        system.synchronizingActionToDecisionDiagramMap[actionIndex] = combineSynchronizingActions(generationInfo, system.synchronizingActionToDecisionDiagramMap[actionIndex], nextModule.synchronizingActionToDecisionDiagramMap[actionIndex]);
                    } else {
                        system.synchronizingActionToDecisionDiagramMap[actionIndex] = combineUnsynchronizedActions(generationInfo, ActionDecisionDiagram(*generationInfo.manager), nextModule.synchronizingActionToDecisionDiagramMap[actionIndex], system.identity, nextModule.identity);
                    }
                }
                
                // Combine identity matrices.
                system.identity = system.identity * nextModule.identity;
            }
            
            storm::dd::Dd<Type> result = createSystemFromModule(generationInfo, system);
            
            // For DTMCs, we normalize each row to 1 (to account for non-determinism).
            if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::DTMC) {
                result = result / result.sumAbstract(generationInfo.columnMetaVariables);
            }
            
            return result;
        }
        
        template <storm::dd::DdType Type>
        void DdPrismModelBuilder<Type>::translateProgram(storm::prism::Program const& program, Options const& options) {
            // There might be nondeterministic variables. In that case the program must be prepared before translating.
            storm::prism::Program preparedProgram;
            if (options.constantDefinitions) {
                preparedProgram = program.defineUndefinedConstants(options.constantDefinitions.get());
            } else {
                preparedProgram = program;
            }
            
            preparedProgram = preparedProgram.substituteConstants();
            
            std::cout << "translated prog: " << preparedProgram << std::endl;
            
            // Start by initializing the structure used for storing all information needed during the model generation.
            // In particular, this creates the meta variables used to encode the model.
            GenerationInformation generationInfo(preparedProgram);

            auto clock = std::chrono::high_resolution_clock::now();
            storm::dd::Dd<Type> transitionMatrix = createSystemDecisionDiagram(generationInfo);
            std::cout << "Built transition matrix in " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - clock).count() << "ms." << std::endl;
            transitionMatrix.exportToDot("trans.dot");
            std::cout << "num transitions: " << transitionMatrix.getNonZeroCount() << std::endl;
            
            storm::dd::Dd<Type> initialStates = createInitialStatesDecisionDiagram(generationInfo);
            std::cout << "initial states: " << initialStates.getNonZeroCount() << std::endl;
            
            storm::dd::Dd<Type> reachableStates = computeReachableStates(generationInfo, initialStates, transitionMatrix);
            std::cout << "reachable states: " << reachableStates.getNonZeroCount() << std::endl;
            exit(-1);
        }
        
        template <storm::dd::DdType Type>
        storm::dd::Dd<Type> DdPrismModelBuilder<Type>::createInitialStatesDecisionDiagram(GenerationInformation& generationInfo) {
            storm::dd::Dd<Type> initialStates = generationInfo.rowExpressionAdapter->translateExpression(generationInfo.program.getInitialConstruct().getInitialStatesExpression());
            
            for (auto const& metaVariable : generationInfo.rowMetaVariables) {
                initialStates *= generationInfo.manager->getRange(metaVariable);
            }
            
            return initialStates;
        }
        
        template <storm::dd::DdType Type>
        storm::dd::Dd<Type> DdPrismModelBuilder<Type>::computeReachableStates(GenerationInformation& generationInfo, storm::dd::Dd<Type> const& initialStates, storm::dd::Dd<Type> const& transitions) {
            storm::dd::Dd<Type> reachableStatesBdd = initialStates.notZero();
            
            // If the model is an MDP, we can abstract from the variables encoding the nondeterminism in the model.
            storm::dd::Dd<Type> transitionBdd = transitions.notZero();
            if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::MDP) {
                std::set<storm::expressions::Variable> abstractVariables;
                abstractVariables.insert(generationInfo.synchronizationMetaVariables.begin(), generationInfo.synchronizationMetaVariables.end());
                abstractVariables.insert(generationInfo.nondeterminismMetaVariables.begin(), generationInfo.nondeterminismMetaVariables.end());
                transitionBdd = transitionBdd.existsAbstract(abstractVariables);
            }
            
            transitionBdd.exportToDot("trans.dot");
            reachableStatesBdd.exportToDot("reach.dot");
            
            // Perform the BFS to discover all reachable states.
            bool changed = true;
            uint_fast64_t iteration = 0;
            do {
                STORM_LOG_TRACE("Iteration " << iteration << " of computing reachable states.");
                changed = false;
                storm::dd::Dd<Type> tmp = reachableStatesBdd * transitionBdd;
                tmp = tmp.existsAbstract(generationInfo.rowMetaVariables);
                tmp.swapVariables(generationInfo.rowColumnMetaVariablePairs);

                storm::dd::Dd<Type> newReachableStates = tmp * (!reachableStatesBdd);
                
                // Check whether new states were indeed discovered.
                if (!newReachableStates.isZero()) {
                    changed = true;
                }
                
                reachableStatesBdd += newReachableStates;
                ++iteration;
            } while (changed);
            
            return reachableStatesBdd;
        }
        
//        template <storm::dd::DdType Type>
//        storm::dd::Dd<Type> SymbolicModelAdapter<Type>::createSystemDecisionDiagramm(GenerationInformation & generationInfo){
//            
//            uint_fast64_t numberOfSynchronizingActions = generationInfo.allSynchronizingActions.size();
//            
//            // System DDs
//            SystemComponentDecisionDiagram<Type> systemDds(0);
//            SystemComponentDecisionDiagram<Type> systemDds1(0);
//            SystemComponentDecisionDiagram<Type> systemDds2(0);
//            
//            storm::dd::Dd<Type> temporary = generationInfo.manager->getZero();
//            
//            // Initialize usedNondetVariablesVector
//            // TODO: Formulate simpler.
//            std::vector<uint_fast64_t> usedNondetVariablesVector(numberOfSynchronizingActions);
//            for (uint_fast64_t j = 0; j < numberOfSynchronizingActions; ++j) {
//                usedNondetVariablesVector[j] = 0;
//            }
//            
//            // Create DD for first module
//            systemDds = createSystemComponentDecisionDiagramm(generationInfo, generationInfo.program.getModule(0), usedNondetVariablesVector);
//            
//            for (uint_fast64_t i = 1; i < generationInfo.program.getNumberOfModules(); ++i) {
//                
//                // Create new usedNondetVariablesVector
//                std::vector<uint_fast64_t> newUsedNondetVariablesVector(numberOfSynchronizingActions);
//                for (uint_fast64_t j = 0; j < numberOfSynchronizingActions; ++j) {
//                    // Check if systemDds contains action
//                    if (std::find(systemDds.allSynchronizingActions.begin(), systemDds.allSynchronizingActions.end(), generationInfo.allSynchronizingActions[j]) != systemDds.allSynchronizingActions.end()) {
//                        newUsedNondetVariablesVector[j] = systemDds.synchronizingActionDds[j].usedNondetVariables;
//                    }
//                    else{
//                        newUsedNondetVariablesVector[j] = usedNondetVariablesVector[j];
//                    }
//                }
//                
//                // Create DD for next module
//                systemDds2 = createSystemComponentDecisionDiagramm(generationInfo, generationInfo.program.getModule(i), newUsedNondetVariablesVector);
//                
//                // SystemDds1 stores the previous modules (already combined)
//                systemDds1 = SystemComponentDecisionDiagram<Type>(systemDds);
//                
//                // SystemDds is used to store combination of SystemDds1 and SystemDds2
//                systemDds = SystemComponentDecisionDiagram<Type>(numberOfSynchronizingActions);
//                
//                // Combine non-synchronizing/independent actions
//                systemDds.independentActionDd = combineModules(generationInfo, false, systemDds1.independentActionDd, systemDds2.independentActionDd, systemDds1.identityMatrix, systemDds2.identityMatrix);
//                
//                // Combine synchronizing actions
//                for (uint_fast64_t j = 0; j < numberOfSynchronizingActions; ++j){
//                    // Check if both modules contain the current action
//                    if (std::find(systemDds1.allSynchronizingActions.begin(), systemDds1.allSynchronizingActions.end(), generationInfo.allSynchronizingActions[j]) != systemDds1.allSynchronizingActions.end() &&
//                        std::find(systemDds2.allSynchronizingActions.begin(), systemDds2.allSynchronizingActions.end(), generationInfo.allSynchronizingActions[j]) != systemDds2.allSynchronizingActions.end()) {
//                        // Both modules contain action
//                        systemDds.synchronizingActionDds[j] = combineModules(generationInfo, true, systemDds1.synchronizingActionDds[j], systemDds2.synchronizingActionDds[j], systemDds1.identityMatrix, systemDds2.identityMatrix);
//                    }
//                    else {
//                        // Only one or no module contains current action
//                        systemDds.synchronizingActionDds[j] = combineModules(generationInfo, false, systemDds1.synchronizingActionDds[j], systemDds2.synchronizingActionDds[j], systemDds1.identityMatrix, systemDds2.identityMatrix);
//                    }
//                }
//                
//                // Combine identity matrix
//                systemDds.identityMatrix = systemDds1.identityMatrix * systemDds2.identityMatrix;
//                
//                // Combine list of synchronizing actions
//                systemDds.allSynchronizingActions.insert(systemDds1.allSynchronizingActions.begin(), systemDds1.allSynchronizingActions.end());
//                systemDds.allSynchronizingActions.insert(systemDds2.allSynchronizingActions.begin(), systemDds2.allSynchronizingActions.end());
//            }
//            
//            // Combine all DDs
//            systemDds = combineSystem(generationInfo, systemDds);
//            generationInfo.globalSystemDds = SystemComponentDecisionDiagram<Type>(systemDds);
//            
//            
//            // Build state and transition rewards
//            generationInfo.rewardDds = computeRewards(generationInfo, systemDds);
//            
//            // Create transition to action mapping for MDPs
//            if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::MDP) {
//                generationInfo.transitionActionDd = computeTransitionAction(generationInfo, systemDds);
//            }
//            
//            // Normalize each row for DTMCs
//            if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::DTMC) {
//                temporary = storm::dd::Dd<Type>(systemDds.independentActionDd.commandsDd);
//                temporary = temporary.sumAbstract(generationInfo.columnMetaVariableNames);
//                systemDds.independentActionDd.commandsDd = systemDds.independentActionDd.commandsDd / temporary;
//            }
//            
//            // Get system transition matrix
//            storm::dd::Dd<Type> systemTransitionMatrix = systemDds.independentActionDd.commandsDd;
//            
//            return systemTransitionMatrix;
//        }
//
//        template <storm::dd::DdType Type>
//        SystemComponentDecisionDiagram<Type> SymbolicModelAdapter<Type>::combineSystem(GenerationInformation const & generationInfo, SystemComponentDecisionDiagram<Type> systemDds){
//            
//            uint_fast64_t numberOfSynchronizingActions = generationInfo.allSynchronizingActions.size();
//            uint_fast64_t max = 0;
//            storm::dd::Dd<Type> temporary = generationInfo.manager->getZero();
//            storm::dd::Dd<Type> variableDd = generationInfo.manager->getZero();
//            
//            // Add non-determinism variables for MDPs
//            if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::MDP) {
//                // Check DD variables
//                
//                // Look for maximal variable index
//                max = systemDds.independentActionDd.usedNondetVariables;
//                for (uint_fast64_t i = 0; i < numberOfSynchronizingActions; ++i){
//                    if (systemDds.synchronizingActionDds[i].usedNondetVariables > max) {
//                        max = systemDds.synchronizingActionDds[i].usedNondetVariables;
//                    }
//                }
//                
//                // Add variables to independent action DD (if required)
//                if (max > systemDds.independentActionDd.usedNondetVariables) {
//                    temporary = generationInfo.manager->getOne();
//                    for (uint_fast64_t i = systemDds.independentActionDd.usedNondetVariables+1; i <= max; ++i){
//                        
//                        // Get variable and set to 0
//                        variableDd = generationInfo.manager->getEncoding("nondet" + std::to_string(i), 0);
//                        temporary = temporary * variableDd;
//                        
//                    }
//                    systemDds.independentActionDd.commandsDd = systemDds.independentActionDd.commandsDd * temporary;
//                    systemDds.independentActionDd.usedNondetVariables = max;
//                }
//                
//                // Add variables to synchronized action DDs
//                for (uint_fast64_t j = 0; j < numberOfSynchronizingActions; ++j){
//                    if (max > systemDds.synchronizingActionDds[j].usedNondetVariables) {
//                        temporary = generationInfo.manager->getOne();
//                        for (uint_fast64_t i = systemDds.synchronizingActionDds[j].usedNondetVariables+1; i <= max; ++i){
//                            
//                            // Get variable and set to 0
//                            variableDd = generationInfo.manager->getEncoding("nondet" + std::to_string(i), 0);
//                            temporary = temporary * variableDd;
//                            
//                        }
//                        systemDds.synchronizingActionDds[j].commandsDd = systemDds.synchronizingActionDds[j].commandsDd * temporary;
//                        systemDds.synchronizingActionDds[j].usedNondetVariables = max;
//                    }
//                }
//                
//                // Set variables for synchronization
//                temporary = generationInfo.manager->getOne();
//                for (uint_fast64_t i = 0; i < numberOfSynchronizingActions; ++i){
//                    // Get sync variable
//                    variableDd = generationInfo.manager->getEncoding("sync" + std::to_string(i), 0);
//                    temporary = temporary * variableDd;
//                }
//                
//                systemDds.independentActionDd.commandsDd = temporary * systemDds.independentActionDd.commandsDd;
//                
//                // Set variables for synchronized action DDs
//                for (uint_fast64_t i = 0; i < numberOfSynchronizingActions; ++i){
//                    temporary = generationInfo.manager->getOne();
//                    for (uint_fast64_t j = 0; j < numberOfSynchronizingActions; ++j){
//                        // Enable synchronizing variable j iff current action i==j
//                        if (i == j) {
//                            variableDd = generationInfo.manager->getEncoding("sync" + std::to_string(j), 1);
//                            temporary = temporary * variableDd;
//                        }
//                        else {
//                            variableDd = generationInfo.manager->getEncoding("sync" + std::to_string(j), 0);
//                            temporary = temporary * variableDd;
//                        }
//                    }
//                    
//                    systemDds.synchronizingActionDds[i].commandsDd = temporary * systemDds.synchronizingActionDds[i].commandsDd;
//                }
//                
//            }
//            
//            // Create transition matrix
//            temporary = systemDds.independentActionDd.commandsDd;
//            for (uint_fast64_t i = 0; i < numberOfSynchronizingActions; ++i){
//                temporary = temporary + systemDds.synchronizingActionDds[i].commandsDd;
//            }
//            
//            // Store transition matrix in systemDDs structure
//            systemDds.independentActionDd.commandsDd = temporary;
//            
//            return systemDds;
//        }
//        
//        template <storm::dd::DdType Type>
//        ModuleDecisionDiagram<Type> SymbolicModelAdapter<Type>::combineModules(GenerationInformation const & generationInfo, bool synchronizing, ModuleDecisionDiagram<Type> moduleDd1, ModuleDecisionDiagram<Type> moduleDd2, storm::dd::Dd<Type> const& identityDd1, storm::dd::Dd<Type> const& identityDd2){
//            
//            // Module DD
//            ModuleDecisionDiagram<Type> moduleDd = ModuleDecisionDiagram<Type>();
//            
//            if (synchronizing) {
//                // Synchronizing actions
//                
//                // Combine guards (intersection)
//                moduleDd.guardDd = moduleDd1.guardDd * moduleDd2.guardDd;
//                
//                // Combine transitions
//                moduleDd.commandsDd = moduleDd1.commandsDd * moduleDd2.commandsDd;
//                
//                // Update min/max index
//                moduleDd.usedNondetVariables = (moduleDd1.usedNondetVariables > moduleDd2.usedNondetVariables) ? moduleDd1.usedNondetVariables : moduleDd2.usedNondetVariables;
//                
//            } else {
//                // Non-synchronizing actions
//                
//                // Multiply commands with identity matrix (because of non-synchronization)
//                moduleDd1.commandsDd = moduleDd1.commandsDd * identityDd2;
//                moduleDd2.commandsDd = moduleDd2.commandsDd * identityDd1;
//                
//                // Combine modules
//                switch (generationInfo.program.getModelType()) {
//                    case storm::prism::Program::ModelType::DTMC:
//                        
//                        // No non-determinism, just sum up
//                        moduleDd.guardDd = moduleDd1.guardDd + moduleDd2.guardDd;
//                        moduleDd.commandsDd = moduleDd1.commandsDd + moduleDd2.commandsDd;
//                        moduleDd.usedNondetVariables = 0;
//                        break;
//                    case storm::prism::Program::ModelType::MDP:
//                        
//                        // Combine modules and solve non-determinism
//                        moduleDd = combineModulesMDP(generationInfo, moduleDd1, moduleDd2);
//                        break;
//                    default:
//                        STORM_LOG_ASSERT(false, "Illegal model type.");
//                }
//            }
//            
//            return moduleDd;
//        }
//        
//        template <storm::dd::DdType Type>
//        ModuleDecisionDiagram<Type> SymbolicModelAdapter<Type>::combineModulesMDP(GenerationInformation const & generationInfo, ModuleDecisionDiagram<Type> moduleDd1, ModuleDecisionDiagram<Type> moduleDd2){
//            
//            // Module DD
//            ModuleDecisionDiagram<Type> moduleDd = ModuleDecisionDiagram<Type>();
//            
//            storm::dd::Dd<Type> temporary = generationInfo.manager->getZero();
//            storm::dd::Dd<Type> variableDd = generationInfo.manager->getZero();
//            
//            // Check if one command DD equals 0
//            if (moduleDd1.commandsDd.isZero()) {
//                moduleDd.guardDd = moduleDd2.guardDd;
//                moduleDd.commandsDd = moduleDd2.commandsDd;
//                moduleDd.usedNondetVariables = moduleDd2.usedNondetVariables;
//                return moduleDd;
//            }
//            if (moduleDd2.commandsDd.isZero()) {
//                moduleDd.guardDd = moduleDd1.guardDd;
//                moduleDd.commandsDd = moduleDd1.commandsDd;
//                moduleDd.usedNondetVariables = moduleDd1.usedNondetVariables;
//                return moduleDd;
//            }
//            
//            // Solve non-determinism
//            
//            // Check index of DD variables
//            if (moduleDd1.usedNondetVariables > moduleDd2.usedNondetVariables) {
//                temporary = generationInfo.manager->getOne();
//                
//                for (uint_fast64_t i = moduleDd2.usedNondetVariables+1; i <= moduleDd1.usedNondetVariables; ++i){
//                    // Get variable and set to 0
//                    variableDd = generationInfo.manager->getEncoding("nondet" + std::to_string(i), 0);
//                    temporary = temporary * variableDd;
//                }
//                moduleDd2.commandsDd = moduleDd2.commandsDd * temporary;
//                moduleDd2.usedNondetVariables = moduleDd1.usedNondetVariables;
//            }
//            if (moduleDd2.usedNondetVariables > moduleDd1.usedNondetVariables) {
//                temporary = generationInfo.manager->getOne();
//                
//                for (uint_fast64_t i = moduleDd1.usedNondetVariables+1; i <= moduleDd2.usedNondetVariables; ++i){
//                    // Get variable and set to 0
//                    variableDd = generationInfo.manager->getEncoding("nondet" + std::to_string(i), 0);
//                    temporary = temporary * variableDd;
//                }
//                moduleDd1.commandsDd = moduleDd1.commandsDd * temporary;
//                moduleDd1.usedNondetVariables = moduleDd2.usedNondetVariables;
//            }
//            
//            // Get new nondet. variable
//            variableDd = variableDd = generationInfo.manager->getEncoding("nondet" + std::to_string(moduleDd1.usedNondetVariables + 1), 1);
//            
//            // Set nondet. variable
//            moduleDd2.commandsDd = moduleDd2.commandsDd * variableDd;
//            moduleDd1.commandsDd = moduleDd1.commandsDd * (!variableDd);
//            
//            // Combine command DDs
//            moduleDd.commandsDd = moduleDd1.commandsDd + moduleDd2.commandsDd;
//            
//            // Combine guard DDs
//            moduleDd.guardDd = moduleDd1.guardDd || moduleDd2.guardDd;
//            
//            moduleDd.usedNondetVariables = moduleDd1.usedNondetVariables + 1;
//            
//            return moduleDd;
//        }
//        
//        template <storm::dd::DdType Type>
//        SystemComponentDecisionDiagram<Type> SymbolicModelAdapter<Type>::createSystemComponentDecisionDiagramm(GenerationInformation const & generationInfo, storm::prism::Module const& module, std::vector<uint_fast64_t> usedNondetVariablesVector){
//            
//            uint_fast64_t numberOfSynchronizingActions = generationInfo.allSynchronizingActions.size();
//            
//            // System Component DD
//            SystemComponentDecisionDiagram<Type> systemComponentDd(numberOfSynchronizingActions);
//            
//            // Create module DD for independent actions
//            systemComponentDd.independentActionDd = createModuleDecisionDiagramm(generationInfo, module, "", 0);
//            
//            // Create module DD for synchronizing actions
//            for (uint_fast64_t i = 0; i < numberOfSynchronizingActions; ++i){
//                
//                if (module.hasAction(generationInfo.allSynchronizingActions[i])){
//                    systemComponentDd.synchronizingActionDds[i] = createModuleDecisionDiagramm(generationInfo, module, generationInfo.allSynchronizingActions[i], usedNondetVariablesVector[i]);
//                }else{
//                    switch (generationInfo.program.getModelType()){
//                        case storm::prism::Program::ModelType::DTMC:
//                            systemComponentDd.synchronizingActionDds[i] = ModuleDecisionDiagram<Type>(generationInfo.manager->getZero(), generationInfo.manager->getZero(), 0);
//                            break;
//                        case storm::prism::Program::ModelType::MDP:
//                            systemComponentDd.synchronizingActionDds[i] = ModuleDecisionDiagram<Type>(generationInfo.manager->getZero(), generationInfo.manager->getZero(), usedNondetVariablesVector[i]);
//                            break;
//                        default:
//                            STORM_LOG_ASSERT(false, "Illegal model type.");
//                    }
//                }
//                
//            }
//            
//            // Get module identity matrix
//            systemComponentDd.identityMatrix = generationInfo.moduleToIdentityDecisionDiagramMap.at(module.getName());
//            
//            // Store all synchronizing actions
//            systemComponentDd.allSynchronizingActions.insert(module.getActions().begin(), module.getActions().end());
//            
//            return systemComponentDd;
//        }
//        
//        // TODO
//        std::map<std::string, int_fast64_t> getMetaVariableMapping(std::vector<std::string> metaVariables, uint_fast64_t value){
//            
//            std::map<std::string, int_fast64_t> metaVariableNameToValueMap = std::map<std::string, int_fast64_t>();
//            
//            for (uint_fast64_t i = 0; i < metaVariables.size(); ++i) {
//                if (value & (1ull << (metaVariables.size() - i - 1))) {
//                    metaVariableNameToValueMap.insert(std::make_pair(metaVariables[i], 1));
//                }
//                else {
//                    metaVariableNameToValueMap.insert(std::make_pair(metaVariables[i], 0));
//                }
//            }
//
//            return metaVariableNameToValueMap;
//        }
//        
//        template <storm::dd::DdType Type>
//        ModuleDecisionDiagram<Type> SymbolicModelAdapter<Type>::combineCommandsMDP(std::shared_ptr<storm::dd::DdManager<Type>> const & manager, uint_fast64_t numberOfCommands, std::vector<storm::dd::Dd<Type>> const& commandDds, std::vector<storm::dd::Dd<Type>> const & guardDds, uint_fast64_t usedNondetVariables){
//            
//            // Module DD
//            ModuleDecisionDiagram<Type> moduleDd = ModuleDecisionDiagram<Type>();
//            
//            // Initialize DDs
//            storm::dd::Dd<Type> guardRangeDd = manager->getZero();
//            storm::dd::Dd<Type> commandsDd = manager->getZero();
//            storm::dd::Dd<Type> temporaryDd = manager->getZero();
//            
//            // Check for overlapping guards
//            storm::dd::Dd<Type> overlappingGuardDd = manager->getZero();
//            
//            for (uint_fast64_t i = 0; i < numberOfCommands; ++i) {
//                overlappingGuardDd = overlappingGuardDd + guardDds[i];
//                guardRangeDd = guardRangeDd || guardDds[i];
//            }
//            
//            uint_fast64_t maxChoices = overlappingGuardDd.getMax();
//            
//            // Check for no choice or no non-determinism
//            if (maxChoices == 0 || maxChoices == 1) {
//                
//                if (maxChoices == 1) {
//                    // Sum up all command updates
//                    for (uint_fast64_t i = 0; i < numberOfCommands; ++i) {
//                        temporaryDd = guardDds[i] * commandDds[i];
//                        commandsDd = commandsDd + temporaryDd;
//                    }
//                }
//                
//                moduleDd.guardDd = guardRangeDd;
//                moduleDd.commandsDd = commandsDd;
//                moduleDd.usedNondetVariables = usedNondetVariables;
//                
//                return moduleDd;
//            }
//            
//            // Calculate number of required variables (log2(maxChoices))
//            uint_fast64_t numberOfBinaryVariables = static_cast<uint_fast64_t>(std::ceil(log2(maxChoices)));
//            
//            // Allocate local nondet. choice variables
//            std::vector<std::string> nondetVariables(numberOfBinaryVariables);
//            for (uint_fast64_t i = 1; i <= numberOfBinaryVariables; ++i) {
//                nondetVariables[i-1] = "nondet" + std::to_string(usedNondetVariables + i);
//            }
//            
//            // Initialize more DDs
//            storm::dd::Dd<Type> equalsNumberOfChoicesDd = manager->getZero();
//            std::vector<storm::dd::Dd<Type>> choiceDds(maxChoices);
//            std::vector<storm::dd::Dd<Type>> remainingDds(maxChoices);
//            
//            storm::dd::Dd<Type> temporaryDd1 = manager->getZero();
//            storm::dd::Dd<Type> temporaryDd2 = manager->getZero();
//            
//            for (uint_fast64_t currentChoices = 1; currentChoices <= maxChoices; ++currentChoices) {
//                
//                // Check for paths with exactly i nondet. choices
//                equalsNumberOfChoicesDd = overlappingGuardDd.equals(manager->getConstant(static_cast<double> (currentChoices)));
//                
//                if (equalsNumberOfChoicesDd.isZero()) continue;
//                
//                // Reset DDs
//                for (uint_fast64_t j = 0; j < currentChoices; ++j) {
//                    choiceDds[j] = manager->getZero();
//                    remainingDds[j] = equalsNumberOfChoicesDd;
//                }
//                
//                // Check all commands
//                for (uint_fast64_t j = 0; j < numberOfCommands; ++j) {
//                    
//                    // Check if command guard overlaps with equalsNumberOfChoicesDd
//                    temporaryDd1 = guardDds[j] && equalsNumberOfChoicesDd;
//                    if (temporaryDd1.isZero()) continue;
//                    
//                    // Split nondet. choices
//                    for (uint_fast64_t k = 0; k < currentChoices; ++k) {
//                        
//                        // Calculate maximal overlapping parts of command guard and remaining DD (for current index)
//                        temporaryDd2 = temporaryDd1 && remainingDds[k];
//                        
//                        // Check if we can add some overlapping parts to the current index
//                        if (temporaryDd2 != manager->getZero()) {
//                            
//                            // Remove overlapping parts from the remaining DD
//                            remainingDds[k] = remainingDds[k] && (!temporaryDd2);
//                            
//                            // Combine guard (overlapping parts) with command updates
//                            temporaryDd = temporaryDd2 * commandDds[j];
//                            // Add command DD to the commands with current index
//                            choiceDds[k] = choiceDds[k] + temporaryDd;
//                        }
//                        
//                        // Remove overlapping parts from the command guard DD
//                        temporaryDd1 = temporaryDd1 && (!temporaryDd2);
//                        
//                        // Check if the command guard DD is already 0
//                        if (temporaryDd1.isZero()) break;
//                    }
//                }
//                
//                // Set nondet. choices for corresponding DDs
//                for (uint_fast64_t j = 0; j < currentChoices; ++j) {
//                    
//                    temporaryDd1 = manager->getZero();
//                    
//                    // Set chosen variables to value j
//                    temporaryDd1.setValue(getMetaVariableMapping(nondetVariables, j), 1);
//                    
//                    // Multiply setting of nondet. variables with corresponding commands
//                    temporaryDd = temporaryDd1 * choiceDds[j];
//                    // Sum up all DDs (no non-determinism any more)
//                    commandsDd = commandsDd + temporaryDd;
//                }
//                
//                // Delete currentChoices out of overlapping DD
//                overlappingGuardDd = overlappingGuardDd * (!equalsNumberOfChoicesDd);
//            }
//            
//            moduleDd.guardDd = guardRangeDd;
//            moduleDd.commandsDd = commandsDd;
//            moduleDd.usedNondetVariables = usedNondetVariables + numberOfBinaryVariables;
//            
//            return moduleDd;
//        }
//        
//        template <storm::dd::DdType Type>
//        ModuleDecisionDiagram<Type> SymbolicModelAdapter<Type>::createModuleDecisionDiagramm(GenerationInformation const & generationInfo, storm::prism::Module const& module, std::string const& synchronizingAction, uint_fast64_t usedNondetVariables){
//            
//            // Module DD
//            ModuleDecisionDiagram<Type> moduleDd;
//            
//            // Set up vectors
//            uint_fast64_t numberOfCommands = module.getNumberOfCommands();
//            std::vector<storm::dd::Dd<Type>> guardDds(numberOfCommands);
//            std::vector<storm::dd::Dd<Type>> commandDds(numberOfCommands);
//            
//            for (uint_fast64_t j = 0; j < numberOfCommands; ++j) {
//                
//                storm::prism::Command const& command = module.getCommand(j);
//                
//                // Check if command action matches requested synchronizing action
//                if (synchronizingAction == command.getActionName()) {
//                    
//                    // Translate guard
//                    guardDds[j] = storm::adapters::SymbolicExpressionAdapter<Type>::translateExpression(command.getGuardExpression().getBaseExpressionPointer(), generationInfo.program, generationInfo.manager);
//
//                    if (guardDds[j].isZero()){
//                        LOG4CPLUS_WARN(logger, "A guard is unsatisfiable.");
//                    }
//                    
//                    // Create command DD
//                    commandDds[j] = createCommandDecisionDiagramm(generationInfo, module, guardDds[j], command);
//                    
//                    commandDds[j] = commandDds[j] * guardDds[j];
//                    
//                    // check negative probabilities/rates
//                    if (commandDds[j].getMin() < 0){
//                        LOG4CPLUS_WARN(logger, "Negative probabilites/rates in command " << (j + 1) << ".");
//                    }
//                }
//                else {
//                    // Otherwise use zero DDs
//                    guardDds[j] = generationInfo.manager->getZero();
//                    commandDds[j] = generationInfo.manager->getZero();
//                }
//            }
//            
//            // combine command DDs with guard DDs
//            switch (generationInfo.program.getModelType()){
//                case storm::prism::Program::ModelType::DTMC:
//                    moduleDd = combineCommandsDTMC(generationInfo.manager, numberOfCommands, commandDds, guardDds);
//                    break;
//                case storm::prism::Program::ModelType::MDP:
//                    moduleDd = combineCommandsMDP(generationInfo.manager, numberOfCommands, commandDds, guardDds, usedNondetVariables);
//                    break;
//                default:
//                    STORM_LOG_ASSERT(false, "Illegal model type.");
//            }
//            
//            return moduleDd;
//        }
//        
//        template <storm::dd::DdType Type>
//        storm::dd::Dd<Type> SymbolicModelAdapter<Type>::createCommandDecisionDiagramm(GenerationInformation const & generationInfo, storm::prism::Module const& module, storm::dd::Dd<Type> const& guard, storm::prism::Command const& command){
//            
//            // Command DD
//            storm::dd::Dd<Type> commandDd = generationInfo.manager->getZero();
//            
//            for (uint_fast64_t i = 0; i < command.getNumberOfUpdates(); ++i) {
//                
//                // Create update DD
//                storm::dd::Dd<Type> updateDd = createUpdateDecisionDiagramm(generationInfo, module, guard, command.getUpdate(i));
//                
//                if (updateDd.isZero()){
//                    LOG4CPLUS_WARN(logger, "Update " << (i + 1) << " does not do anything.");
//                }
//                
//                // Multiply likelihood expression (MDP: transition probability)
//                double p = command.getUpdate(i).getLikelihoodExpression().evaluateAsDouble();
//                
//                updateDd = updateDd * generationInfo.manager->getConstant(p);
//                
//                commandDd = commandDd + updateDd;
//            }
//            
//            return commandDd;
//        }
//
//        template <storm::dd::DdType Type>
//        storm::dd::Dd<Type> SymbolicModelAdapter<Type>::createUpdateDecisionDiagramm(GenerationInformation const & generationInfo, storm::prism::Module const& module, storm::dd::Dd<Type> const& guard, storm::prism::Update const& update){
//            
//            // Update DD
//            storm::dd::Dd<Type> updateDd = generationInfo.manager->getOne();
//            
//            // Assignments (integer and boolean)
//            std::vector<storm::prism::Assignment> assignments = update.getAssignments();
//            for (auto singleAssignment : assignments) {
//                
//                // Translate first part of assignment
//                storm::dd::Dd<Type> temporary = generationInfo.manager->getZero();
//                
//                temporary = generationInfo.manager->getIdentity(singleAssignment.getVariableName() + "'");
//                
//                // Translate second part of assignment
//                storm::dd::Dd<Type> updateExpr = storm::adapters::SymbolicExpressionAdapter<Type>::translateExpression(singleAssignment.getExpression().getBaseExpressionPointer(), generationInfo.program, generationInfo.manager);
//                
//                storm::dd::Dd<Type> result = updateExpr * guard;
//                // Combine first and second part of the assignment
//                result = result.equals(temporary);
//                result = result * guard;
//                
//                // Filter range
//                result = result * generationInfo.manager->getRange(singleAssignment.getVariableName() + "'");
//                
//                updateDd = updateDd * result;
//            }
//
//            // All unused global boolean variables do not change
//            for (uint_fast64_t i = 0; i < generationInfo.program.getGlobalBooleanVariables().size(); ++i) {
//                storm::prism::BooleanVariable const& booleanVariable = generationInfo.program.getGlobalBooleanVariables().at(i);
//                if (update.getAssignmentMapping().find(booleanVariable.getName()) == update.getAssignmentMapping().end()) {
//                    // Multiply identity matrix
//                    updateDd = updateDd * generationInfo.variableToIdentityDecisionDiagramMap.at(booleanVariable.getName());
//                }
//            }
//            
//            // All unused global integer variables do not change
//            for (uint_fast64_t i = 0; i < generationInfo.program.getGlobalIntegerVariables().size(); ++i) {
//                storm::prism::IntegerVariable const& integerVariable = generationInfo.program.getGlobalIntegerVariables().at(i);
//                if (update.getAssignmentMapping().find(integerVariable.getName()) == update.getAssignmentMapping().end()) {
//                    // Multiply identity matrix
//                    updateDd = updateDd * generationInfo.variableToIdentityDecisionDiagramMap.at(integerVariable.getName());
//                }
//            }
//            
//            // All unused boolean variables do not change
//            for (uint_fast64_t i = 0; i < module.getNumberOfBooleanVariables(); ++i) {
//                storm::prism::BooleanVariable const& booleanVariable = module.getBooleanVariables().at(i);
//                if (update.getAssignmentMapping().find(booleanVariable.getName()) == update.getAssignmentMapping().end()) {
//                    // Multiply identity matrix
//                    updateDd = updateDd * generationInfo.variableToIdentityDecisionDiagramMap.at(booleanVariable.getName());
//                }
//            }
//            
//            // All unused integer variables do not change
//            for (uint_fast64_t i = 0; i < module.getNumberOfIntegerVariables(); ++i) {
//                storm::prism::IntegerVariable const& integerVariable = module.getIntegerVariables().at(i);
//                if (update.getAssignmentMapping().find(integerVariable.getName()) == update.getAssignmentMapping().end()) {
//                    // Multiply identity matrix
//                    updateDd = updateDd * generationInfo.variableToIdentityDecisionDiagramMap.at(integerVariable.getName());
//                }
//            }
//
//            return updateDd;
//        }
//        
//        template <storm::dd::DdType Type>
//        storm::dd::Dd<Type> SymbolicModelAdapter<Type>::getInitialStateDecisionDiagram(GenerationInformation const & generationInfo) {
//            storm::dd::Dd<Type> initialStates = generationInfo.manager->getOne();
//            storm::dd::Dd<Type> temporary = generationInfo.manager->getZero();
//            
//            // Global Boolean variables
//            for (uint_fast64_t j = 0; j < generationInfo.program.getGlobalBooleanVariables().size(); ++j) {
//                storm::prism::BooleanVariable const& booleanVariable = generationInfo.program.getGlobalBooleanVariables().at(j);
//                temporary = generationInfo.manager->getEncoding(booleanVariable.getName(), booleanVariable.getInitialValueExpression().evaluateAsBool());
//                initialStates = initialStates * temporary;
//            }
//            
//            // Global Integer variables
//            for (uint_fast64_t j = 0; j < generationInfo.program.getGlobalIntegerVariables().size(); ++j) {
//                storm::prism::IntegerVariable const& integerVariable = generationInfo.program.getGlobalIntegerVariables().at(j);
//                temporary = generationInfo.manager->getEncoding(integerVariable.getName(), integerVariable.getInitialValueExpression().evaluateAsInt());
//                initialStates = initialStates * temporary;
//            }
//            
//            for (uint_fast64_t i = 0; i < generationInfo.program.getNumberOfModules(); ++i) {
//                storm::prism::Module const& module = generationInfo.program.getModule(i);
//                
//                // Boolean variables
//                for (uint_fast64_t j = 0; j < module.getNumberOfBooleanVariables(); ++j) {
//                    storm::prism::BooleanVariable const& booleanVariable = module.getBooleanVariables().at(j);
//                    temporary = generationInfo.manager->getEncoding(booleanVariable.getName(), booleanVariable.getInitialValueExpression().evaluateAsBool());
//                    initialStates = initialStates * temporary;
//                }
//                
//                // Integer variables
//                for (uint_fast64_t j = 0; j < module.getNumberOfIntegerVariables(); ++j) {
//                    storm::prism::IntegerVariable const& integerVariable = module.getIntegerVariables().at(j);
//                    temporary = generationInfo.manager->getEncoding(integerVariable.getName(), integerVariable.getInitialValueExpression().evaluateAsInt());
//                    initialStates = initialStates * temporary;
//                }
//            }
//            
//            return initialStates;
//        }
//        
//        template <storm::dd::DdType Type>
//        storm::dd::Dd<Type> SymbolicModelAdapter<Type>::performReachability(GenerationInformation & generationInfo, storm::dd::Dd<Type> const& systemDd, storm::dd::Dd<Type> const& initialStateDd) {
//            
//            // Initialize the clock.
//            auto clock = std::chrono::high_resolution_clock::now();
//            
//            
//            storm::dd::Dd<Type> temporary = generationInfo.manager->getZero();
//            storm::dd::Dd<Type> S;
//            storm::dd::Dd<Type> U;
//            
//            // Get initial state
//            storm::dd::Dd<Type> reachableStates = initialStateDd;
//            
//            // Copy current state
//            storm::dd::Dd<Type> newReachableStates = reachableStates;
//            
//            std::set<std::string> abstractVariables = std::set<std::string>();
//            
//            if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::MDP) {
//                for (uint_fast64_t i = 0; i < generationInfo.allSynchronizingActions.size(); ++i) {
//                    // Synchronizing variables
//                    if (systemDd.containsMetaVariable("sync" + std::to_string(i))) {
//                        abstractVariables.insert("sync" + std::to_string(i));
//                    }
//                }
//                for (uint_fast64_t i = 1; i <= generationInfo.numberOfNondetVariables; ++i) {
//                    // Nondet. variables
//                    if (systemDd.containsMetaVariable("nondet" + std::to_string(i))) {
//                        abstractVariables.insert("nondet" + std::to_string(i));
//                    }
//                }
//            }
//            
//            // Create system BDD
//            storm::dd::Dd<Type> systemBdd = systemDd.notZero();
//            
//            // For MDPs, we need to abstract from the nondeterminism variables, but we can do so prior to the
//            // reachability analysis.
//            if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::MDP) {
//                // Abstract from synchronizing and nondet. variables (MDPs)
//                systemBdd = systemBdd.existsAbstract(abstractVariables);
//            }
//            
//            // Initialize variables and choose option
//            bool changed;
//            int iter = 0;
//            int option = storm::settings::adapterSettings().getReachabilityMethod();
//            
//            //TODO: Name reachability options.
//            std::cout << "Reachability option: " << option << std::endl;
//            
//            if (option == 3 || option == 4){
//                
//                S = storm::dd::Dd<Type>(initialStateDd);
//                U = storm::dd::Dd<Type>(initialStateDd);
//                
//                generationInfo.globalSystemDds.independentActionDd.commandsDd = generationInfo.globalSystemDds.independentActionDd.commandsDd.notZero();
//                generationInfo.globalSystemDds.independentActionDd.commandsDd = generationInfo.globalSystemDds.independentActionDd.commandsDd.existsAbstract(abstractVariables);
//                
//                for (uint_fast64_t i = 0; i < generationInfo.allSynchronizingActions.size(); ++i) {
//                    generationInfo.globalSystemDds.synchronizingActionDds[i].commandsDd = generationInfo.globalSystemDds.synchronizingActionDds[i].commandsDd.notZero();
//                    generationInfo.globalSystemDds.synchronizingActionDds[i].commandsDd = generationInfo.globalSystemDds.synchronizingActionDds[i].commandsDd.existsAbstract(abstractVariables);
//                }
//            }
//            
//            // Perform updates until nothing changes
//            do {
//                if (option == 1){
//                    iter++;
//                    changed = true;
//                    
//                    newReachableStates = newReachableStates * systemBdd;
//                    
//                    // Abstract from row meta variables
//                    newReachableStates = newReachableStates.existsAbstract(generationInfo.rowMetaVariableNames);
//                    
//                    // Swap column variables to row variables
//                    newReachableStates.swapVariables(generationInfo.metaVariablePairs);
//                    
//                    newReachableStates = newReachableStates * (!reachableStates);
//                    
//                    // Check if something has changed
//                    if (newReachableStates.isZero()) {
//                        changed = false;
//                    }
//                    
//                    // Update reachableStates DD
//                    reachableStates = reachableStates + newReachableStates;
//                    
//                } else if (option == 2) {
//                    iter++;
//                    changed = false;
//                    
//                    newReachableStates = newReachableStates * systemBdd;
//                    
//                    // Abstract from row meta variables
//                    newReachableStates = newReachableStates.existsAbstract(generationInfo.rowMetaVariableNames);
//                    
//                    // Swap column variables to row variables
//                    newReachableStates.swapVariables(generationInfo.metaVariablePairs);
//                    
//                    newReachableStates = newReachableStates || reachableStates;
//                    
//                    // Check if something has changed
//                    if (newReachableStates != reachableStates) {
//                        changed = true;
//                    }
//                    
//                    // Update reachableStates DD
//                    reachableStates = newReachableStates;
//                    
//                } else if (option == 3) {
//                    iter++;
//                    changed = true;
//                    
//                    newReachableStates = generationInfo.manager->getZero();
//                    
//                    temporary = U * generationInfo.globalSystemDds.independentActionDd.commandsDd;
//                    newReachableStates = newReachableStates.existsAbstract(generationInfo.rowMetaVariableNames);
//                    newReachableStates.swapVariables(generationInfo.metaVariablePairs);
//                    newReachableStates = temporary;
//                    
//                    for (uint_fast64_t i = 0; i < generationInfo.allSynchronizingActions.size(); ++i) {
//                        temporary = U * generationInfo.globalSystemDds.synchronizingActionDds[i].commandsDd;
//                        temporary = temporary.existsAbstract(generationInfo.rowMetaVariableNames);
//                        temporary.swapVariables(generationInfo.metaVariablePairs);
//                        newReachableStates = newReachableStates || temporary;
//                    }
//                    
//                    newReachableStates = newReachableStates.existsAbstract(generationInfo.rowMetaVariableNames);
//                    newReachableStates.swapVariables(generationInfo.metaVariablePairs);
//                    U = U || newReachableStates;
//                    
//                    if (U == S){
//                        changed = false;
//                        reachableStates = S;
//                        break;
//                    }
//                    
//                    S = S || U;
//                    
//                }
//                else if (option == 4) {
//                    iter++;
//                    changed = true;
//                    
//                    temporary = U * generationInfo.globalSystemDds.independentActionDd.commandsDd;
//                    temporary = temporary.existsAbstract(generationInfo.rowMetaVariableNames);
//                    temporary.swapVariables(generationInfo.metaVariablePairs);
//                    U = U || temporary;
//                    
//                    for (uint_fast64_t i = 0; i < generationInfo.allSynchronizingActions.size(); ++i) {
//                        temporary = U * generationInfo.globalSystemDds.synchronizingActionDds[i].commandsDd;
//                        temporary = temporary.existsAbstract(generationInfo.rowMetaVariableNames);
//                        temporary.swapVariables(generationInfo.metaVariablePairs);
//                        U = U || temporary;
//                    }
//                    
//                    U = U * (!S);
//                    
//                    S = S + U;
//                    
//                    if (U.isZero()){
//                        changed = false;
//                        reachableStates = S;
//                    }
//                    
//                }
//            } while (changed);
//            
//            std::cout << "Performed reachability (" << iter << " iterations) in " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - clock).count() << "ms." << std::endl;
//            
//            return reachableStates;
//        }
//        
//        template <storm::dd::DdType Type>
//        storm::dd::Dd<Type> SymbolicModelAdapter<Type>::findDeadlocks(GenerationInformation const & generationInfo, storm::dd::Dd<Type> systemDd, storm::dd::Dd<Type> const& reachableStatesDd) {
//            
//            // Initialize the clock.
//            auto clock = std::chrono::high_resolution_clock::now();
//            
//            storm::dd::Dd<Type> systemBdd = systemDd.notZero();
//            
//            std::set<std::string> abstractVariables = std::set<std::string>();
//            
//            if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::MDP) {
//                for (uint_fast64_t i = 0; i < generationInfo.allSynchronizingActions.size(); ++i) {
//                    // Synchronizing variables
//                    if (systemDd.containsMetaVariable("sync" + std::to_string(i))) {
//                        abstractVariables.insert("sync" + std::to_string(i));
//                    }
//                }
//                for (uint_fast64_t i = 1; i <= generationInfo.numberOfNondetVariables; ++i) {
//                    // Nondet. variables
//                    if (systemDd.containsMetaVariable("nondet" + std::to_string(i))) {
//                        abstractVariables.insert("nondet" + std::to_string(i));
//                    }
//                }
//            }
//            
//            // Find states with at least one transition
//            systemBdd = systemBdd.existsAbstract(generationInfo.columnMetaVariableNames);
//            
//            // For MDPs, we need to abstract from the nondeterminism variables
//            if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::MDP) {
//                // Abstract from synchronizing and nondet. variables (MDPs)
//                systemBdd = systemBdd.existsAbstract(abstractVariables);
//            }
//            
//            systemBdd = reachableStatesDd * (!systemBdd);
//            
//            std::cout << "Deadlocks: " << systemBdd.getNonZeroCount() << " fixed." << std::endl;
//            
//            // Check if there are deadlocks
//            if (!systemBdd.isZero()){
//                
//                storm::dd::Dd<Type> temporary = generationInfo.manager->getOne();
//                
//                // Get all variable identities
//                for (auto variable : generationInfo.rowMetaVariableNames) {
//                    temporary = temporary * generationInfo.variableToIdentityDecisionDiagramMap.at(variable);
//                }
//                
//                // Add synchronizing and nondet. variables to identity for MDPs (all set to 0)
//                if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::MDP) {
//                    for (uint_fast64_t i = 0; i < generationInfo.allSynchronizingActions.size(); ++i) {
//                        // Synchronizing variables
//                        if (systemDd.containsMetaVariable("sync" + std::to_string(i))) {
//                            temporary = temporary * generationInfo.manager->getEncoding("sync" + std::to_string(i),0);
//                        }
//                    }
//                    for (uint_fast64_t i = 1; i <= generationInfo.numberOfNondetVariables; ++i) {
//                        // Nondet. variables
//                        if (systemDd.containsMetaVariable("nondet" + std::to_string(i))) {
//                            temporary = temporary * generationInfo.manager->getEncoding("nondet" + std::to_string(i), 0);
//                        }
//                    }
//                }
//                
//                temporary = temporary * systemBdd;
//                
//                // Add self-loops to transition matrix
//                systemDd = systemDd + temporary;
//            }
//            
//            std::cout << "Fixed deadlocks in " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - clock).count() << "ms." << std::endl;
//            
//            return systemDd;
//        }
//        
//        template <storm::dd::DdType Type>
//        std::pair<std::vector<storm::dd::Dd<Type>>, std::vector<storm::dd::Dd<Type>>> SymbolicModelAdapter<Type>::computeRewards(GenerationInformation const & generationInfo, SystemComponentDecisionDiagram<Type> const& systemDds) {
//            
//            // Get number of reward modules and synchronizing actions
//            uint_fast64_t numberOfRewardModules = generationInfo.program.getRewardModels().size();
//            uint_fast64_t numberOfSynchronizingActions = generationInfo.allSynchronizingActions.size();
//            
//            // State reward DD
//            std::vector<storm::dd::Dd<Type>> stateRewardsDds = std::vector<storm::dd::Dd<Type>>(numberOfRewardModules);
//            // Transition reward DD
//            std::vector<storm::dd::Dd<Type>> transitionRewardsDds = std::vector<storm::dd::Dd<Type>>(numberOfRewardModules);
//            
//            // Initialize DDs
//            for (uint_fast64_t i = 0; i < numberOfRewardModules; ++i) {
//                stateRewardsDds[i] = generationInfo.manager->getConstant(0);
//                transitionRewardsDds[i] = generationInfo.manager->getConstant(0);
//            }
//            
//            storm::dd::Dd<Type> statesDd = generationInfo.manager->getZero();
//            storm::dd::Dd<Type> rewardsDd = generationInfo.manager->getZero();
//            storm::dd::Dd<Type> temporary = generationInfo.manager->getZero();
//            
//            // Loop through all reward models
//            for (uint_fast64_t i = 0; i < numberOfRewardModules; ++i) {
//                storm::prism::RewardModel const& currentRewardModule = generationInfo.program.getRewardModels().at(i);
//                
//                // State rewards 
//                for (auto stateReward : currentRewardModule.getStateRewards()) {
//                    
//                    // Translate state and reward expression
//                    statesDd = storm::adapters::SymbolicExpressionAdapter<Type>::translateExpression(stateReward.getStatePredicateExpression().getBaseExpressionPointer(), generationInfo.program, generationInfo.manager);
//                    rewardsDd = storm::adapters::SymbolicExpressionAdapter<Type>::translateExpression(stateReward.getRewardValueExpression().getBaseExpressionPointer(), generationInfo.program, generationInfo.manager);
//                    
//                    // Restrict rewards to states
//                    temporary = statesDd * rewardsDd;
//                    
//                    // Check for negative rewards
//                    if (temporary.getMin() < 0){
//                        LOG4CPLUS_WARN(logger, "Negative state reward in reward model " << (i + 1) << ".");
//                    }
//                    
//                    if(temporary.isZero()) {
//                        LOG4CPLUS_WARN(logger, "Only zero rewards in reward model " << (i + 1) << ".");
//                    }
//                    
//                    // Combine all rewards
//                    stateRewardsDds[i] = stateRewardsDds[i] + temporary;
//                }
//                
//                // Transition rewards 
//                for (auto transitionReward : currentRewardModule.getTransitionRewards()) {
//                    
//                    // Translate state and reward expression
//                    statesDd = storm::adapters::SymbolicExpressionAdapter<Type>::translateExpression(transitionReward.getStatePredicateExpression().getBaseExpressionPointer(), generationInfo.program, generationInfo.manager);
//                    rewardsDd = storm::adapters::SymbolicExpressionAdapter<Type>::translateExpression(transitionReward.getRewardValueExpression().getBaseExpressionPointer(), generationInfo.program, generationInfo.manager);
//                    
//                    // Get action name of the transition
//                    std::string const& rewardAction = transitionReward.getActionName();
//                    
//                    if (rewardAction == "") {
//                        // Take independent action module
//                        temporary = systemDds.independentActionDd.commandsDd;
//                    }else {
//                        // Get module corresponding to the reward action
//                        for (uint_fast64_t j = 0; j < numberOfSynchronizingActions; ++j) {
//                            if (generationInfo.allSynchronizingActions[j] == rewardAction) {
//                                temporary = systemDds.synchronizingActionDds[j].commandsDd;
//                                break;
//                            }
//                        }
//                    }
//                    
//                    // Convert to BDD for MDPs (DTMC need exact values for scaling)
//                    if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::MDP) {
//                        temporary = temporary.notZero();
//                    }
//                    
//                    // Restrict to states and multiply reward values
//                    temporary = temporary * statesDd;
//                    temporary = temporary * rewardsDd;
//                    
//                    // Check for negative rewards
//                    if (temporary.getMin() < 0){
//                        LOG4CPLUS_WARN(logger, "Negative transition reward in reward model " << (i + 1) << ".");
//                    }
//                    
//                    // Combine all rewards
//                    transitionRewardsDds[i] = transitionRewardsDds[i] + temporary;
//                }
//            }
//            
//            // Scale transition rewards for DTMCs
//            if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::DTMC) {
//                for (uint_fast64_t i = 0; i < generationInfo.program.getRewardModels().size(); ++i){
//                    // Divide transition rewards through transition matrix
//                    transitionRewardsDds[i] = transitionRewardsDds[i] / systemDds.independentActionDd.commandsDd;
//                }
//            }
//            
//            // Pair to store state and transition rewards
//            return std::make_pair(stateRewardsDds, transitionRewardsDds);
//        }
//        
//        template <storm::dd::DdType Type>
//        storm::dd::Dd<Type> SymbolicModelAdapter<Type>::computeTransitionAction(GenerationInformation const & generationInfo, SystemComponentDecisionDiagram<Type> const& systemDds){
//            
//            // Transition actions DD
//            storm::dd::Dd<Type> transitionActionDd = generationInfo.manager->getZero();
//            storm::dd::Dd<Type> temporary = generationInfo.manager->getZero();
//            
//            // Store action labels for each transition (0 iff no action/tau/epsilon)
//            storm::dd::Dd<Type> commandsBdd = generationInfo.manager->getZero();
//            for (uint_fast64_t i = 0; i < generationInfo.allSynchronizingActions.size(); ++i){
//                // Get action transition matrix as BDD
//                commandsBdd = systemDds.synchronizingActionDds[i].commandsDd.notZero();
//                commandsBdd = commandsBdd.existsAbstract(generationInfo.columnMetaVariableNames);
//                
//                // Add action index
//                temporary = commandsBdd * generationInfo.manager->getConstant(i + 1);
//                transitionActionDd = transitionActionDd + temporary;
//            }
//            
//            return transitionActionDd;
//        }
        
        // Explicitly instantiate the symbolic expression adapter
        template class DdPrismModelBuilder<storm::dd::DdType::CUDD>;
        
    } // namespace adapters
} // namespace storm


