#include "src/builder/DdPrismModelBuilder.h"

#include "src/models/symbolic/Dtmc.h"
#include "src/models/symbolic/Ctmc.h"
#include "src/models/symbolic/Mdp.h"
#include "src/models/symbolic/StandardRewardModel.h"

#include "src/storage/dd/CuddDd.h"
#include "src/storage/dd/CuddDdManager.h"
#include "src/settings/SettingsManager.h"

#include "src/exceptions/InvalidStateException.h"

#include "src/exceptions/InvalidArgumentException.h"

#include "src/utility/prism.h"
#include "src/utility/math.h"
#include "src/storage/prism/Program.h"

#include "src/storage/dd/CuddAdd.h"
#include "src/storage/dd/CuddBdd.h"

#include "src/settings/modules/GeneralSettings.h"

namespace storm {
    namespace builder {
        
        template <storm::dd::DdType Type>
        class DdPrismModelBuilder<Type>::GenerationInformation {
            public:
                GenerationInformation(storm::prism::Program const& program) : program(program), manager(std::make_shared<storm::dd::DdManager<Type>>()), rowMetaVariables(), variableToRowMetaVariableMap(), rowExpressionAdapter(nullptr), columnMetaVariables(), variableToColumnMetaVariableMap(), columnExpressionAdapter(nullptr), rowColumnMetaVariablePairs(), nondeterminismMetaVariables(), variableToIdentityMap(), allGlobalVariables(), moduleToIdentityMap() {
                    // Initializes variables and identity DDs.
                    createMetaVariablesAndIdentities();
                    
                    rowExpressionAdapter = std::shared_ptr<storm::adapters::AddExpressionAdapter<Type>>(new storm::adapters::AddExpressionAdapter<Type>(manager, variableToRowMetaVariableMap));
                    columnExpressionAdapter = std::shared_ptr<storm::adapters::AddExpressionAdapter<Type>>(new storm::adapters::AddExpressionAdapter<Type>(manager, variableToColumnMetaVariableMap));
                }
                
                // The program that is currently translated.
                storm::prism::Program const& program;
                
                // The manager used to build the decision diagrams.
                std::shared_ptr<storm::dd::DdManager<Type>> manager;

                // The meta variables for the row encoding.
                std::set<storm::expressions::Variable> rowMetaVariables;
                std::map<storm::expressions::Variable, storm::expressions::Variable> variableToRowMetaVariableMap;
                std::shared_ptr<storm::adapters::AddExpressionAdapter<Type>> rowExpressionAdapter;
                
                // The meta variables for the column encoding.
                std::set<storm::expressions::Variable> columnMetaVariables;
                std::map<storm::expressions::Variable, storm::expressions::Variable> variableToColumnMetaVariableMap;
                std::shared_ptr<storm::adapters::AddExpressionAdapter<Type>> columnExpressionAdapter;
                
                // All pairs of row/column meta variables.
                std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> rowColumnMetaVariablePairs;
                
                // The meta variables used to encode the nondeterminism.
                std::vector<storm::expressions::Variable> nondeterminismMetaVariables;
                
                // The meta variables used to encode the synchronization.
                std::vector<storm::expressions::Variable> synchronizationMetaVariables;
                
                // A set of all variables used for encoding the nondeterminism (i.e. nondetermism + synchronization
                // variables). This is handy to abstract from this variable set.
                std::set<storm::expressions::Variable> allNondeterminismVariables;
            
                // As set of all variables used for encoding the synchronization.
                std::set<storm::expressions::Variable> allSynchronizationMetaVariables;
            
                // DDs representing the identity for each variable.
                std::map<storm::expressions::Variable, storm::dd::Add<Type>> variableToIdentityMap;
            
                // A set of all meta variables that correspond to global variables.
                std::set<storm::expressions::Variable> allGlobalVariables;
            
                // DDs representing the identity for each module.
                std::map<std::string, storm::dd::Add<Type>> moduleToIdentityMap;
                
                // DDs representing the valid ranges of the variables of each module.
                std::map<std::string, storm::dd::Add<Type>> moduleToRangeMap;
                
            private:
                /*!
                 * Creates the required meta variables and variable/module identities.
                 */
                void createMetaVariablesAndIdentities() {
                    // Add synchronization variables.
                    for (auto const& actionIndex : program.getSynchronizingActionIndices()) {
                        std::pair<storm::expressions::Variable, storm::expressions::Variable> variablePair = manager->addMetaVariable(program.getActionName(actionIndex));
                        synchronizationMetaVariables.push_back(variablePair.first);
                        allSynchronizationMetaVariables.insert(variablePair.first);
                        allNondeterminismVariables.insert(variablePair.first);
                    }
                    
                    // Add nondeterminism variables (number of modules + number of commands).
                    uint_fast64_t numberOfNondeterminismVariables = program.getModules().size();
                    for (auto const& module : program.getModules()) {
                        numberOfNondeterminismVariables += module.getNumberOfCommands();
                    }
                    for (uint_fast64_t i = 0; i < numberOfNondeterminismVariables; ++i) {
                        std::pair<storm::expressions::Variable, storm::expressions::Variable> variablePair = manager->addMetaVariable("nondet" + std::to_string(i));
                        nondeterminismMetaVariables.push_back(variablePair.first);
                        allNondeterminismVariables.insert(variablePair.first);
                    }
                    
                    // Create meta variables for global program variables.
                    for (storm::prism::IntegerVariable const& integerVariable : program.getGlobalIntegerVariables()) {
                        int_fast64_t low = integerVariable.getLowerBoundExpression().evaluateAsInt();
                        int_fast64_t high = integerVariable.getUpperBoundExpression().evaluateAsInt();
                        std::pair<storm::expressions::Variable, storm::expressions::Variable> variablePair = manager->addMetaVariable(integerVariable.getName(), low, high);
                        
                        STORM_LOG_TRACE("Created meta variables for global integer variable: " << variablePair.first.getName() << "[" << variablePair.first.getIndex() << "] and " << variablePair.second.getName() << "[" << variablePair.second.getIndex() << "]");

                        rowMetaVariables.insert(variablePair.first);
                        variableToRowMetaVariableMap.emplace(integerVariable.getExpressionVariable(), variablePair.first);
                        
                        columnMetaVariables.insert(variablePair.second);
                        variableToColumnMetaVariableMap.emplace(integerVariable.getExpressionVariable(), variablePair.second);
                        
                        storm::dd::Add<Type> variableIdentity = manager->getIdentity(variablePair.first).equals(manager->getIdentity(variablePair.second)) * manager->getRange(variablePair.first).toAdd() * manager->getRange(variablePair.second).toAdd();
                        variableToIdentityMap.emplace(integerVariable.getExpressionVariable(), variableIdentity);
                        rowColumnMetaVariablePairs.push_back(variablePair);
                        
                        allGlobalVariables.insert(integerVariable.getExpressionVariable());
                    }
                    for (storm::prism::BooleanVariable const& booleanVariable : program.getGlobalBooleanVariables()) {
                        std::pair<storm::expressions::Variable, storm::expressions::Variable> variablePair = manager->addMetaVariable(booleanVariable.getName());
                        
                        STORM_LOG_TRACE("Created meta variables for global boolean variable: " << variablePair.first.getName() << "[" << variablePair.first.getIndex() << "] and " << variablePair.second.getName() << "[" << variablePair.second.getIndex() << "]");
                        
                        rowMetaVariables.insert(variablePair.first);
                        variableToRowMetaVariableMap.emplace(booleanVariable.getExpressionVariable(), variablePair.first);
                        
                        columnMetaVariables.insert(variablePair.second);
                        variableToColumnMetaVariableMap.emplace(booleanVariable.getExpressionVariable(), variablePair.second);
                        
                        storm::dd::Add<Type> variableIdentity = manager->getIdentity(variablePair.first).equals(manager->getIdentity(variablePair.second));
                        variableToIdentityMap.emplace(booleanVariable.getExpressionVariable(), variableIdentity);
                        
                        rowColumnMetaVariablePairs.push_back(variablePair);
                        allGlobalVariables.insert(booleanVariable.getExpressionVariable());
                    }

                    // Create meta variables for each of the modules' variables.
                    for (storm::prism::Module const& module : program.getModules()) {
                        storm::dd::Add<Type> moduleIdentity = manager->getAddOne();
                        storm::dd::Add<Type> moduleRange = manager->getAddOne();
                        
                        for (storm::prism::IntegerVariable const& integerVariable : module.getIntegerVariables()) {
                            int_fast64_t low = integerVariable.getLowerBoundExpression().evaluateAsInt();
                            int_fast64_t high = integerVariable.getUpperBoundExpression().evaluateAsInt();
                            std::pair<storm::expressions::Variable, storm::expressions::Variable> variablePair = manager->addMetaVariable(integerVariable.getName(), low, high);
                            STORM_LOG_TRACE("Created meta variables for integer variable: " << variablePair.first.getName() << "[" << variablePair.first.getIndex() << "] and " << variablePair.second.getName() << "[" << variablePair.second.getIndex() << "]");
                            
                            rowMetaVariables.insert(variablePair.first);
                            variableToRowMetaVariableMap.emplace(integerVariable.getExpressionVariable(), variablePair.first);
                            
                            columnMetaVariables.insert(variablePair.second);
                            variableToColumnMetaVariableMap.emplace(integerVariable.getExpressionVariable(), variablePair.second);
                            
                            storm::dd::Add<Type> variableIdentity = manager->getIdentity(variablePair.first).equals(manager->getIdentity(variablePair.second)) * manager->getRange(variablePair.first).toAdd() * manager->getRange(variablePair.second).toAdd();
                            variableToIdentityMap.emplace(integerVariable.getExpressionVariable(), variableIdentity);
                            moduleIdentity *= variableIdentity;
                            moduleRange *= manager->getRange(variablePair.first).toAdd();
                            
                            rowColumnMetaVariablePairs.push_back(variablePair);
                        }
                        for (storm::prism::BooleanVariable const& booleanVariable : module.getBooleanVariables()) {
                            std::pair<storm::expressions::Variable, storm::expressions::Variable> variablePair = manager->addMetaVariable(booleanVariable.getName());
                            STORM_LOG_TRACE("Created meta variables for boolean variable: " << variablePair.first.getName() << "[" << variablePair.first.getIndex() << "] and " << variablePair.second.getName() << "[" << variablePair.second.getIndex() << "]");

                            rowMetaVariables.insert(variablePair.first);
                            variableToRowMetaVariableMap.emplace(booleanVariable.getExpressionVariable(), variablePair.first);
                            
                            columnMetaVariables.insert(variablePair.second);
                            variableToColumnMetaVariableMap.emplace(booleanVariable.getExpressionVariable(), variablePair.second);
                            
                            storm::dd::Add<Type> variableIdentity = manager->getIdentity(variablePair.first).equals(manager->getIdentity(variablePair.second)) * manager->getRange(variablePair.first).toAdd() * manager->getRange(variablePair.second).toAdd();
                            variableToIdentityMap.emplace(booleanVariable.getExpressionVariable(), variableIdentity);
                            moduleIdentity *= variableIdentity;
                            moduleRange *= manager->getRange(variablePair.first).toAdd();

                            rowColumnMetaVariablePairs.push_back(variablePair);
                        }
                        moduleToIdentityMap[module.getName()] = moduleIdentity;
                        moduleToRangeMap[module.getName()] = moduleRange;
                    }
                }
            };
            
        template <storm::dd::DdType Type>
        DdPrismModelBuilder<Type>::Options::Options() : buildAllRewardModels(true), rewardModelsToBuild(), constantDefinitions(), buildAllLabels(true), labelsToBuild(), expressionLabels(), terminalStates(), negatedTerminalStates() {
            // Intentionally left empty.
        }
        
        template <storm::dd::DdType Type>
        DdPrismModelBuilder<Type>::Options::Options(storm::logic::Formula const& formula) : buildAllRewardModels(false), rewardModelsToBuild(), constantDefinitions(), buildAllLabels(false), labelsToBuild(std::set<std::string>()), expressionLabels(std::vector<storm::expressions::Expression>()), terminalStates(), negatedTerminalStates() {
            this->preserveFormula(formula);
            this->setTerminalStatesFromFormula(formula);
        }
        
        template <storm::dd::DdType Type>
        DdPrismModelBuilder<Type>::Options::Options(std::vector<std::shared_ptr<storm::logic::Formula>> const& formulas) : buildAllRewardModels(false), rewardModelsToBuild(), constantDefinitions(), buildAllLabels(false), labelsToBuild(), expressionLabels(), terminalStates(), negatedTerminalStates() {
            if (formulas.empty()) {
                this->buildAllRewardModels = true;
                this->buildAllLabels = true;
            } else {
                for (auto const& formula : formulas) {
                    this->preserveFormula(*formula);
                }
                if (formulas.size() == 1) {
                    this->setTerminalStatesFromFormula(*formulas.front());
                }
            }
        }
        
        template <storm::dd::DdType Type>
        void DdPrismModelBuilder<Type>::Options::preserveFormula(storm::logic::Formula const& formula) {
            // If we already had terminal states, we need to erase them.
            if (terminalStates) {
                terminalStates.reset();
            }
            if (negatedTerminalStates) {
                negatedTerminalStates.reset();
            }

            // If we are not required to build all reward models, we determine the reward models we need to build.
            if (!buildAllRewardModels) {
                if (formula.containsRewardOperator()) {
                    std::set<std::string> referencedRewardModels = formula.getReferencedRewardModels();
                    rewardModelsToBuild.insert(referencedRewardModels.begin(), referencedRewardModels.end());
                }
            }
            
            // Extract all the labels used in the formula.
            std::vector<std::shared_ptr<storm::logic::AtomicLabelFormula const>> atomicLabelFormulas = formula.getAtomicLabelFormulas();
            for (auto const& formula : atomicLabelFormulas) {
                if (!labelsToBuild) {
                    labelsToBuild = std::set<std::string>();
                }
                labelsToBuild.get().insert(formula.get()->getLabel());
            }
            
            // Extract all the expressions used in the formula.
            std::vector<std::shared_ptr<storm::logic::AtomicExpressionFormula const>> atomicExpressionFormulas = formula.getAtomicExpressionFormulas();
            for (auto const& formula : atomicExpressionFormulas) {
                if (!expressionLabels) {
                    expressionLabels = std::vector<storm::expressions::Expression>();
                }
                expressionLabels.get().push_back(formula.get()->getExpression());
            }
        }
        
        template <storm::dd::DdType Type>
        void DdPrismModelBuilder<Type>::Options::setTerminalStatesFromFormula(storm::logic::Formula const& formula) {
            // If cutting the model was disabled, we do not set anything.
            if (storm::settings::generalSettings().isNoCutsSet()) {
                return;
            }

            if (formula.isAtomicExpressionFormula()) {
                terminalStates = formula.asAtomicExpressionFormula().getExpression();
            } else if (formula.isAtomicLabelFormula()) {
                terminalStates = formula.asAtomicLabelFormula().getLabel();
            } else if (formula.isEventuallyFormula()) {
                storm::logic::Formula const& sub = formula.asEventuallyFormula().getSubformula();
                if (sub.isAtomicExpressionFormula() || sub.isAtomicLabelFormula()) {
                    this->setTerminalStatesFromFormula(sub);
                }
            } else if (formula.isUntilFormula()) {
                storm::logic::Formula const& right = formula.asUntilFormula().getRightSubformula();
                if (right.isAtomicExpressionFormula() || right.isAtomicLabelFormula()) {
                    this->setTerminalStatesFromFormula(right);
                }
                storm::logic::Formula const& left = formula.asUntilFormula().getLeftSubformula();
                if (left.isAtomicExpressionFormula()) {
                    negatedTerminalStates = left.asAtomicExpressionFormula().getExpression();
                } else if (left.isAtomicLabelFormula()) {
                    negatedTerminalStates = left.asAtomicLabelFormula().getLabel();
                }
            } else if (formula.isProbabilityOperatorFormula()) {
                storm::logic::Formula const& sub = formula.asProbabilityOperatorFormula().getSubformula();
                if (sub.isEventuallyFormula() || sub.isUntilFormula()) {
                    this->setTerminalStatesFromFormula(sub);
                }
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
        struct DdPrismModelBuilder<Type>::SystemResult {
            SystemResult(storm::dd::Add<Type> const& allTransitionsDd, DdPrismModelBuilder<Type>::ModuleDecisionDiagram const& globalModule, storm::dd::Add<Type> const& stateActionDd) : allTransitionsDd(allTransitionsDd), globalModule(globalModule), stateActionDd(stateActionDd) {
                // Intentionally left empty.
            }
            
            storm::dd::Add<Type> allTransitionsDd;
            typename DdPrismModelBuilder<Type>::ModuleDecisionDiagram globalModule;
            storm::dd::Add<Type> stateActionDd;
        };
        
        template <storm::dd::DdType Type>
        typename DdPrismModelBuilder<Type>::UpdateDecisionDiagram DdPrismModelBuilder<Type>::createUpdateDecisionDiagram(GenerationInformation& generationInfo, storm::prism::Module const& module, storm::dd::Add<Type> const& guard, storm::prism::Update const& update) {
            storm::dd::Add<Type> updateDd = generationInfo.manager->getAddOne();
            
            STORM_LOG_TRACE("Translating update " << update);
            
            // Iterate over all assignments (boolean and integer) and build the DD for it.
            std::vector<storm::prism::Assignment> assignments = update.getAssignments();
            std::set<storm::expressions::Variable> assignedVariables;
            for (auto const& assignment : assignments) {
                // Record the variable as being written.
                STORM_LOG_TRACE("Assigning to variable " << generationInfo.variableToRowMetaVariableMap.at(assignment.getVariable()).getName());
                assignedVariables.insert(assignment.getVariable());
                
                // Translate the written variable.
                auto const& primedMetaVariable = generationInfo.variableToColumnMetaVariableMap.at(assignment.getVariable());
                storm::dd::Add<Type> writtenVariable = generationInfo.manager->getIdentity(primedMetaVariable);
                
                // Translate the expression that is being assigned.
                storm::dd::Add<Type> updateExpression = generationInfo.rowExpressionAdapter->translateExpression(assignment.getExpression());
                
                // Combine the update expression with the guard.
                storm::dd::Add<Type> result = updateExpression * guard;
                
                // Combine the variable and the assigned expression.
                result = result.equals(writtenVariable);
                result *= guard;
                
                // Restrict the transitions to the range of the written variable.
                result = result * generationInfo.manager->getRange(primedMetaVariable).toAdd();
                
                updateDd *= result;
            }
            
            // Compute the set of assigned global variables.
            std::set<storm::expressions::Variable> assignedGlobalVariables;
            std::set_intersection(assignedVariables.begin(), assignedVariables.end(), generationInfo.allGlobalVariables.begin(), generationInfo.allGlobalVariables.end(), std::inserter(assignedGlobalVariables, assignedGlobalVariables.begin()));
            
            // All unassigned boolean variables need to keep their value.
            for (storm::prism::BooleanVariable const& booleanVariable : module.getBooleanVariables()) {
                if (assignedVariables.find(booleanVariable.getExpressionVariable()) == assignedVariables.end()) {
                    STORM_LOG_TRACE("Multiplying identity of variable " << booleanVariable.getName());
                    updateDd *= generationInfo.variableToIdentityMap.at(booleanVariable.getExpressionVariable());
                }
            }
            
            // All unassigned integer variables need to keep their value.
            for (storm::prism::IntegerVariable const& integerVariable : module.getIntegerVariables()) {
                if (assignedVariables.find(integerVariable.getExpressionVariable()) == assignedVariables.end()) {
                    STORM_LOG_TRACE("Multiplying identity of variable " << integerVariable.getName());
                    updateDd *= generationInfo.variableToIdentityMap.at(integerVariable.getExpressionVariable());
                }
            }
            
            return UpdateDecisionDiagram(updateDd, assignedGlobalVariables);
        }
        
        template <storm::dd::DdType Type>
        typename DdPrismModelBuilder<Type>::ActionDecisionDiagram DdPrismModelBuilder<Type>::createCommandDecisionDiagram(GenerationInformation& generationInfo, storm::prism::Module const& module, storm::prism::Command const& command) {
            STORM_LOG_TRACE("Translating guard " << command.getGuardExpression());
            storm::dd::Add<Type> guardDd = generationInfo.rowExpressionAdapter->translateExpression(command.getGuardExpression()) * generationInfo.moduleToRangeMap[module.getName()];
            STORM_LOG_WARN_COND(!guardDd.isZero(), "The guard '" << command.getGuardExpression() << "' is unsatisfiable.");
            
            if (!guardDd.isZero()) {
                // Create the DDs representing the individual updates.
                std::vector<UpdateDecisionDiagram> updateResults;
                for (storm::prism::Update const& update : command.getUpdates()) {
                    updateResults.push_back(createUpdateDecisionDiagram(generationInfo, module, guardDd, update));
                    
                    STORM_LOG_WARN_COND(!updateResults.back().updateDd.isZero(), "Update '" << update << "' does not have any effect.");
                }

                // Start by gathering all variables that were written in at least one update.
                std::set<storm::expressions::Variable> globalVariablesInSomeUpdate;
                
                // If the command is labeled, we have to analyze which portion of the global variables was written by
                // any of the updates and make all update results equal w.r.t. this set. If the command is not labeled,
                // we can already multiply the identities of all global variables.
                if (command.isLabeled()) {
                    std::for_each(updateResults.begin(), updateResults.end(), [&globalVariablesInSomeUpdate] (UpdateDecisionDiagram const& update) { globalVariablesInSomeUpdate.insert(update.assignedGlobalVariables.begin(), update.assignedGlobalVariables.end()); } );
                } else {
                    globalVariablesInSomeUpdate = generationInfo.allGlobalVariables;
                }
                
                // Then, multiply the missing identities.
                for (auto& updateResult : updateResults) {
                    std::set<storm::expressions::Variable> missingIdentities;
                    std::set_difference(globalVariablesInSomeUpdate.begin(), globalVariablesInSomeUpdate.end(), updateResult.assignedGlobalVariables.begin(), updateResult.assignedGlobalVariables.end(), std::inserter(missingIdentities, missingIdentities.begin()));

                    for (auto const& variable : missingIdentities) {
                        STORM_LOG_TRACE("Multiplying identity for variable " << variable.getName() << "[" << variable.getIndex() << "] to update.");
                        updateResult.updateDd *= generationInfo.variableToIdentityMap.at(variable);
                    }
                }
                
                // Now combine the update DDs to the command DD.
                storm::dd::Add<Type> commandDd = generationInfo.manager->getAddZero();
                auto updateResultsIt = updateResults.begin();
                for (auto updateIt = command.getUpdates().begin(), updateIte = command.getUpdates().end(); updateIt != updateIte; ++updateIt, ++updateResultsIt) {
                    storm::dd::Add<Type> probabilityDd = generationInfo.rowExpressionAdapter->translateExpression(updateIt->getLikelihoodExpression());
                    commandDd += updateResultsIt->updateDd * probabilityDd;
                }
                
                return ActionDecisionDiagram(guardDd, guardDd * commandDd, globalVariablesInSomeUpdate);
            } else {
                return ActionDecisionDiagram(*generationInfo.manager);
            }
        }
        
        template <storm::dd::DdType Type>
        typename DdPrismModelBuilder<Type>::ActionDecisionDiagram DdPrismModelBuilder<Type>::createActionDecisionDiagram(GenerationInformation& generationInfo, storm::prism::Module const& module, uint_fast64_t synchronizationActionIndex, uint_fast64_t nondeterminismVariableOffset) {
            std::vector<ActionDecisionDiagram> commandDds;
            for (storm::prism::Command const& command : module.getCommands()) {
                
                // Determine whether the command is relevant for the selected action.
                bool relevant = (synchronizationActionIndex == 0 && !command.isLabeled()) || (synchronizationActionIndex && command.isLabeled() && command.getActionIndex() == synchronizationActionIndex);
                
                if (!relevant) {
                    continue;
                }
                
                STORM_LOG_TRACE("Translating command " << command);
                
                // At this point, the command is known to be relevant for the action.
                commandDds.push_back(createCommandDecisionDiagram(generationInfo, module, command));
            }
            
            ActionDecisionDiagram result(*generationInfo.manager);
            if (!commandDds.empty()) {
                switch (generationInfo.program.getModelType()){
                    case storm::prism::Program::ModelType::DTMC:
                    case storm::prism::Program::ModelType::CTMC:
                        result = combineCommandsToActionMarkovChain(generationInfo, commandDds);
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
        std::set<storm::expressions::Variable> DdPrismModelBuilder<Type>::equalizeAssignedGlobalVariables(GenerationInformation const& generationInfo, ActionDecisionDiagram& action1, ActionDecisionDiagram& action2) {
            // Start by gathering all variables that were written in at least one action DD.
            std::set<storm::expressions::Variable> globalVariablesInActionDd;
            std::set_union(action1.assignedGlobalVariables.begin(), action1.assignedGlobalVariables.end(), action2.assignedGlobalVariables.begin(), action2.assignedGlobalVariables.end(), std::inserter(globalVariablesInActionDd, globalVariablesInActionDd.begin()));

            std::set<storm::expressions::Variable> missingIdentitiesInAction1;
            std::set_difference(globalVariablesInActionDd.begin(), globalVariablesInActionDd.end(), action1.assignedGlobalVariables.begin(), action1.assignedGlobalVariables.end(), std::inserter(missingIdentitiesInAction1, missingIdentitiesInAction1.begin()));
            for (auto const& variable : missingIdentitiesInAction1) {
                action1.transitionsDd *= generationInfo.variableToIdentityMap.at(variable);
            }
            
            std::set<storm::expressions::Variable> missingIdentitiesInAction2;
            std::set_difference(globalVariablesInActionDd.begin(), globalVariablesInActionDd.end(), action1.assignedGlobalVariables.begin(), action1.assignedGlobalVariables.end(), std::inserter(missingIdentitiesInAction2, missingIdentitiesInAction2.begin()));
            for (auto const& variable : missingIdentitiesInAction2) {
                action2.transitionsDd *= generationInfo.variableToIdentityMap.at(variable);
            }

            return globalVariablesInActionDd;
        }
        
        template <storm::dd::DdType Type>
        std::set<storm::expressions::Variable> DdPrismModelBuilder<Type>::equalizeAssignedGlobalVariables(GenerationInformation const& generationInfo, std::vector<ActionDecisionDiagram>& actionDds) {
            // Start by gathering all variables that were written in at least one action DD.
            std::set<storm::expressions::Variable> globalVariablesInActionDd;
            for (auto const& commandDd : actionDds) {
                globalVariablesInActionDd.insert(commandDd.assignedGlobalVariables.begin(), commandDd.assignedGlobalVariables.end());
            }
            
            STORM_LOG_TRACE("Equalizing assigned global variables.");

            // Then multiply the transitions of each action with the missing identities.
            for (auto& actionDd : actionDds) {
                STORM_LOG_TRACE("Equalizing next action.");
                std::set<storm::expressions::Variable> missingIdentities;
                std::set_difference(globalVariablesInActionDd.begin(), globalVariablesInActionDd.end(), actionDd.assignedGlobalVariables.begin(), actionDd.assignedGlobalVariables.end(), std::inserter(missingIdentities, missingIdentities.begin()));
                for (auto const& variable : missingIdentities) {
                    STORM_LOG_TRACE("Multiplying identity of variable " << variable.getName() << ".");
                    actionDd.transitionsDd *= generationInfo.variableToIdentityMap.at(variable);
                }
            }
            return globalVariablesInActionDd;
        }
        
        template <storm::dd::DdType Type>
        typename DdPrismModelBuilder<Type>::ActionDecisionDiagram DdPrismModelBuilder<Type>::combineCommandsToActionMarkovChain(GenerationInformation& generationInfo, std::vector<ActionDecisionDiagram>& commandDds) {
            storm::dd::Add<Type> allGuards = generationInfo.manager->getAddZero();
            storm::dd::Add<Type> allCommands = generationInfo.manager->getAddZero();
            storm::dd::Add<Type> temporary;
            
            // Make all command DDs assign to the same global variables.
            std::set<storm::expressions::Variable> assignedGlobalVariables = equalizeAssignedGlobalVariables(generationInfo, commandDds);
            
            // Then combine the commands to the full action DD and multiply missing identities along the way.
            for (auto& commandDd : commandDds) {
                // Check for overlapping guards.
                temporary = commandDd.guardDd * allGuards;
                
                // Issue a warning if there are overlapping guards in a non-CTMC model.
                STORM_LOG_WARN_COND(temporary.isZero() || generationInfo.program.getModelType() == storm::prism::Program::ModelType::CTMC, "Guard of a command overlaps with previous guards.");
                
                allGuards += commandDd.guardDd;
                allCommands += commandDd.guardDd * commandDd.transitionsDd ;
            }
            
            return ActionDecisionDiagram(allGuards, allCommands, assignedGlobalVariables);
        }
        
        template <storm::dd::DdType Type>
        storm::dd::Add<Type> DdPrismModelBuilder<Type>::encodeChoice(GenerationInformation& generationInfo, uint_fast64_t nondeterminismVariableOffset, uint_fast64_t numberOfBinaryVariables, int_fast64_t value) {
            storm::dd::Add<Type> result = generationInfo.manager->getAddZero();
            
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
        typename DdPrismModelBuilder<Type>::ActionDecisionDiagram DdPrismModelBuilder<Type>::combineCommandsToActionMDP(GenerationInformation& generationInfo, std::vector<ActionDecisionDiagram>& commandDds, uint_fast64_t nondeterminismVariableOffset) {
            storm::dd::Add<Type> allGuards = generationInfo.manager->getAddZero();
            storm::dd::Add<Type> allCommands = generationInfo.manager->getAddZero();
            
            // Make all command DDs assign to the same global variables.
            std::set<storm::expressions::Variable> assignedGlobalVariables = equalizeAssignedGlobalVariables(generationInfo, commandDds);
            
            // Sum all guards, so we can read off the maximal number of nondeterministic choices in any given state.
            storm::dd::Add<Type> sumOfGuards = generationInfo.manager->getAddZero();
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
                    allCommands += commandDd.transitionsDd;
                }
                return ActionDecisionDiagram(sumOfGuards, allCommands, assignedGlobalVariables);
            } else {
                // Calculate number of required variables to encode the nondeterminism.
                uint_fast64_t numberOfBinaryVariables = static_cast<uint_fast64_t>(std::ceil(storm::utility::math::log2(maxChoices)));
                
                storm::dd::Add<Type> equalsNumberOfChoicesDd = generationInfo.manager->getAddZero();
                std::vector<storm::dd::Add<Type>> choiceDds(maxChoices, generationInfo.manager->getAddZero());
                std::vector<storm::dd::Add<Type>> remainingDds(maxChoices, generationInfo.manager->getAddZero());
                
                for (uint_fast64_t currentChoices = 1; currentChoices <= maxChoices; ++currentChoices) {
                    // Determine the set of states with exactly currentChoices choices.
                    equalsNumberOfChoicesDd = sumOfGuards.equals(generationInfo.manager->getConstant(static_cast<double>(currentChoices)));
                    
                    // If there is no such state, continue with the next possible number of choices.
                    if (equalsNumberOfChoicesDd.isZero()) {
                        continue;
                    }
                    
                    // Reset the previously used intermediate storage.
                    for (uint_fast64_t j = 0; j < currentChoices; ++j) {
                        choiceDds[j] = generationInfo.manager->getAddZero();
                        remainingDds[j] = equalsNumberOfChoicesDd;
                    }
                    
                    for (std::size_t j = 0; j < commandDds.size(); ++j) {
                        // Check if command guard overlaps with equalsNumberOfChoicesDd. That is, there are states with exactly currentChoices
                        // choices such that one outgoing choice is given by the j-th command.
                        storm::dd::Add<Type> guardChoicesIntersection = commandDds[j].guardDd * equalsNumberOfChoicesDd;
                        
                        // If there is no such state, continue with the next command.
                        if (guardChoicesIntersection.isZero()) {
                            continue;
                        }
                        
                        // Split the currentChoices nondeterministic choices.
                        for (uint_fast64_t k = 0; k < currentChoices; ++k) {
                            // Calculate the overlapping part of command guard and the remaining DD.
                            storm::dd::Add<Type> remainingGuardChoicesIntersection = guardChoicesIntersection * remainingDds[k];
                            
                            // Check if we can add some overlapping parts to the current index.
                            if (!remainingGuardChoicesIntersection.isZero()) {
                                // Remove overlapping parts from the remaining DD.
                                remainingDds[k] = remainingDds[k] * !remainingGuardChoicesIntersection;
                                
                                // Combine the overlapping part of the guard with command updates and add it to the resulting DD.
                                choiceDds[k] += remainingGuardChoicesIntersection * commandDds[j].transitionsDd;
                            }
                            
                            // Remove overlapping parts from the command guard DD
                            guardChoicesIntersection = guardChoicesIntersection * !remainingGuardChoicesIntersection;
                            
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
                    sumOfGuards = sumOfGuards * !equalsNumberOfChoicesDd;
                }
                
                return ActionDecisionDiagram(allGuards, allCommands, assignedGlobalVariables, nondeterminismVariableOffset + numberOfBinaryVariables);
            }
        }

        template <storm::dd::DdType Type>
        typename DdPrismModelBuilder<Type>::ActionDecisionDiagram DdPrismModelBuilder<Type>::combineSynchronizingActions(GenerationInformation const& generationInfo, ActionDecisionDiagram const& action1, ActionDecisionDiagram const& action2) {
            std::set<storm::expressions::Variable> assignedGlobalVariables;
            std::set_union(action1.assignedGlobalVariables.begin(), action1.assignedGlobalVariables.end(), action2.assignedGlobalVariables.begin(), action2.assignedGlobalVariables.end(), std::inserter(assignedGlobalVariables, assignedGlobalVariables.begin()));
            return ActionDecisionDiagram(action1.guardDd * action2.guardDd, action1.transitionsDd * action2.transitionsDd, assignedGlobalVariables, std::max(action1.numberOfUsedNondeterminismVariables, action2.numberOfUsedNondeterminismVariables));
        }
        
        template <storm::dd::DdType Type>
        typename DdPrismModelBuilder<Type>::ActionDecisionDiagram DdPrismModelBuilder<Type>::combineUnsynchronizedActions(GenerationInformation const& generationInfo, ActionDecisionDiagram& action1, ActionDecisionDiagram& action2, storm::dd::Add<Type> const& identityDd1, storm::dd::Add<Type> const& identityDd2) {
            storm::dd::Add<Type> action1Extended = action1.transitionsDd * identityDd2;
            storm::dd::Add<Type> action2Extended = action2.transitionsDd * identityDd1;

            STORM_LOG_TRACE("Combining unsynchronized actions.");
            
            // Make both action DDs write to the same global variables.
            std::set<storm::expressions::Variable> assignedGlobalVariables = equalizeAssignedGlobalVariables(generationInfo, action1, action2);
            
            if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::DTMC || generationInfo.program.getModelType() == storm::prism::Program::ModelType::CTMC) {
                return ActionDecisionDiagram(action1.guardDd + action2.guardDd, action1Extended + action2Extended, assignedGlobalVariables, 0);
            } else if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::MDP) {
                if (action1.transitionsDd.isZero()) {
                    return ActionDecisionDiagram(action2.guardDd, action2Extended, assignedGlobalVariables, action2.numberOfUsedNondeterminismVariables);
                } else if (action2.transitionsDd.isZero()) {
                    return ActionDecisionDiagram(action1.guardDd, action1Extended, assignedGlobalVariables, action1.numberOfUsedNondeterminismVariables);
                }
                
                // Bring both choices to the same number of variables that encode the nondeterminism.
                uint_fast64_t numberOfUsedNondeterminismVariables = std::max(action1.numberOfUsedNondeterminismVariables, action2.numberOfUsedNondeterminismVariables);
                if (action1.numberOfUsedNondeterminismVariables > action2.numberOfUsedNondeterminismVariables) {
                    storm::dd::Add<Type> nondeterminismEncoding = generationInfo.manager->getAddOne();
                    
                    for (uint_fast64_t i = action2.numberOfUsedNondeterminismVariables; i < action1.numberOfUsedNondeterminismVariables; ++i) {
                        nondeterminismEncoding *= generationInfo.manager->getEncoding(generationInfo.nondeterminismMetaVariables[i], 0).toAdd();
                    }
                    action2Extended *= nondeterminismEncoding;
                } else if (action2.numberOfUsedNondeterminismVariables > action1.numberOfUsedNondeterminismVariables) {
                    storm::dd::Add<Type> nondeterminismEncoding = generationInfo.manager->getAddOne();
                    
                    for (uint_fast64_t i = action1.numberOfUsedNondeterminismVariables; i < action2.numberOfUsedNondeterminismVariables; ++i) {
                        nondeterminismEncoding *= generationInfo.manager->getEncoding(generationInfo.nondeterminismMetaVariables[i], 0).toAdd();
                    }
                    action1Extended *= nondeterminismEncoding;
                }
                
                // Add a new variable that resolves the nondeterminism between the two choices.
                storm::dd::Add<Type> combinedTransitions = generationInfo.manager->getEncoding(generationInfo.nondeterminismMetaVariables[numberOfUsedNondeterminismVariables], 1).toAdd().ite(action2Extended, action1Extended);
                
                return ActionDecisionDiagram(action1.guardDd || action2.guardDd, combinedTransitions, assignedGlobalVariables, numberOfUsedNondeterminismVariables + 1);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidStateException, "Illegal model type.");
            }
        }

        template <storm::dd::DdType Type>
        typename DdPrismModelBuilder<Type>::ModuleDecisionDiagram DdPrismModelBuilder<Type>::createModuleDecisionDiagram(GenerationInformation& generationInfo, storm::prism::Module const& module, std::map<uint_fast64_t, uint_fast64_t> const& synchronizingActionToOffsetMap) {
            // Start by creating the action DD for the independent action.
            ActionDecisionDiagram independentActionDd = createActionDecisionDiagram(generationInfo, module, 0, 0);
            uint_fast64_t numberOfUsedNondeterminismVariables = independentActionDd.numberOfUsedNondeterminismVariables;
            
            // Create module DD for all synchronizing actions of the module.
            std::map<uint_fast64_t, ActionDecisionDiagram> actionIndexToDdMap;
            for (auto const& actionIndex : module.getSynchronizingActionIndices()) {
                STORM_LOG_TRACE("Creating DD for action '" << actionIndex << "'.");
                ActionDecisionDiagram tmp = createActionDecisionDiagram(generationInfo, module, actionIndex, synchronizingActionToOffsetMap.at(actionIndex));
                numberOfUsedNondeterminismVariables = std::max(numberOfUsedNondeterminismVariables, tmp.numberOfUsedNondeterminismVariables);
                actionIndexToDdMap.emplace(actionIndex, tmp);
            }
            
            return ModuleDecisionDiagram(independentActionDd, actionIndexToDdMap, generationInfo.moduleToIdentityMap.at(module.getName()), numberOfUsedNondeterminismVariables);
        }
        
        template <storm::dd::DdType Type>
        storm::dd::Add<Type> DdPrismModelBuilder<Type>::getSynchronizationDecisionDiagram(GenerationInformation& generationInfo, uint_fast64_t actionIndex) {
            storm::dd::Add<Type> synchronization = generationInfo.manager->getAddOne();
            if (actionIndex != 0) {
                for (uint_fast64_t i = 0; i < generationInfo.synchronizationMetaVariables.size(); ++i) {
                    if ((actionIndex - 1) == i) {
                        synchronization *= generationInfo.manager->getEncoding(generationInfo.synchronizationMetaVariables[i], 1).toAdd();
                    } else {
                        synchronization *= generationInfo.manager->getEncoding(generationInfo.synchronizationMetaVariables[i], 0).toAdd();
                    }
                }
            } else {
                for (uint_fast64_t i = 0; i < generationInfo.synchronizationMetaVariables.size(); ++i) {
                    synchronization *= generationInfo.manager->getEncoding(generationInfo.synchronizationMetaVariables[i], 0).toAdd();
                }
            }
            return synchronization;
        }
        
        template <storm::dd::DdType Type>
        storm::dd::Add<Type> DdPrismModelBuilder<Type>::createSystemFromModule(GenerationInformation& generationInfo, ModuleDecisionDiagram const& module) {
            // If the model is an MDP, we need to encode the nondeterminism using additional variables.
            if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::MDP) {
                storm::dd::Add<Type> result = generationInfo.manager->getAddZero();
                
                // First, determine the highest number of nondeterminism variables that is used in any action and make
                // all actions use the same amout of nondeterminism variables.
                uint_fast64_t numberOfUsedNondeterminismVariables = module.numberOfUsedNondeterminismVariables;

                // Compute missing global variable identities in independent action.
                std::set<storm::expressions::Variable> missingIdentities;
                std::set_difference(generationInfo.allGlobalVariables.begin(), generationInfo.allGlobalVariables.end(), module.independentAction.assignedGlobalVariables.begin(), module.independentAction.assignedGlobalVariables.end(), std::inserter(missingIdentities, missingIdentities.begin()));
                storm::dd::Add<Type> identityEncoding = generationInfo.manager->getAddOne();
                for (auto const& variable : missingIdentities) {
                    STORM_LOG_TRACE("Multiplying identity of global variable " << variable.getName() << " to independent action.");
                    identityEncoding *= generationInfo.variableToIdentityMap.at(variable);
                }
                
                // Add variables to independent action DD.
                storm::dd::Add<Type> nondeterminismEncoding = generationInfo.manager->getAddOne();
                for (uint_fast64_t i = module.independentAction.numberOfUsedNondeterminismVariables; i < numberOfUsedNondeterminismVariables; ++i) {
                    nondeterminismEncoding *= generationInfo.manager->getEncoding(generationInfo.nondeterminismMetaVariables[i], 0).toAdd();
                }
                result = identityEncoding * module.independentAction.transitionsDd * nondeterminismEncoding;

                // Add variables to synchronized action DDs.
                std::map<uint_fast64_t, storm::dd::Add<Type>> synchronizingActionToDdMap;
                for (auto const& synchronizingAction : module.synchronizingActionToDecisionDiagramMap) {
                    // Compute missing global variable identities in synchronizing actions.
                    missingIdentities = std::set<storm::expressions::Variable>();
                    std::set_difference(generationInfo.allGlobalVariables.begin(), generationInfo.allGlobalVariables.end(), synchronizingAction.second.assignedGlobalVariables.begin(), synchronizingAction.second.assignedGlobalVariables.end(), std::inserter(missingIdentities, missingIdentities.begin()));
                    identityEncoding = generationInfo.manager->getAddOne();
                    for (auto const& variable : missingIdentities) {
                        STORM_LOG_TRACE("Multiplying identity of global variable " << variable.getName() << " to synchronizing action '" << synchronizingAction.first << "'.");
                        identityEncoding *= generationInfo.variableToIdentityMap.at(variable);
                    }
                    
                    nondeterminismEncoding = generationInfo.manager->getAddOne();
                    for (uint_fast64_t i = synchronizingAction.second.numberOfUsedNondeterminismVariables; i < numberOfUsedNondeterminismVariables; ++i) {
                        nondeterminismEncoding *= generationInfo.manager->getEncoding(generationInfo.nondeterminismMetaVariables[i], 0).toAdd();
                    }
                    synchronizingActionToDdMap.emplace(synchronizingAction.first, identityEncoding * synchronizingAction.second.transitionsDd * nondeterminismEncoding);
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
            } else if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::DTMC || generationInfo.program.getModelType() == storm::prism::Program::ModelType::CTMC) {
                // Simply add all actions.
                storm::dd::Add<Type> result = module.independentAction.transitionsDd;
                for (auto const& synchronizingAction : module.synchronizingActionToDecisionDiagramMap) {
                    result += synchronizingAction.second.transitionsDd;
                }
                return result;
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Illegal model type.");
            }
        }
        
        template <storm::dd::DdType Type>
        typename DdPrismModelBuilder<Type>::SystemResult DdPrismModelBuilder<Type>::createSystemDecisionDiagram(GenerationInformation& generationInfo) {
            // Create the initial offset mapping.
            std::map<uint_fast64_t, uint_fast64_t> synchronizingActionToOffsetMap;
            for (auto const& actionIndex : generationInfo.program.getSynchronizingActionIndices()) {
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
                for (auto const& actionIndex : generationInfo.program.getSynchronizingActionIndices()) {
                    if (system.hasSynchronizingAction(actionIndex)) {
                        synchronizingActionToOffsetMap[actionIndex] = system.synchronizingActionToDecisionDiagramMap[actionIndex].numberOfUsedNondeterminismVariables;
                    }
                }
                
                ModuleDecisionDiagram currentModuleDd = createModuleDecisionDiagram(generationInfo, currentModule, synchronizingActionToOffsetMap);
                
                // Combine the non-synchronizing action.
                uint_fast64_t numberOfUsedNondeterminismVariables = currentModuleDd.numberOfUsedNondeterminismVariables;
                system.independentAction = combineUnsynchronizedActions(generationInfo, system.independentAction, currentModuleDd.independentAction, system.identity, currentModuleDd.identity);
                numberOfUsedNondeterminismVariables = std::max(numberOfUsedNondeterminismVariables, system.independentAction.numberOfUsedNondeterminismVariables);
                
                ActionDecisionDiagram emptyAction(*generationInfo.manager);
                
                // For all synchronizing actions that the next module does not have, we need to multiply the identity of the next module.
                for (auto& action : system.synchronizingActionToDecisionDiagramMap) {
                    if (!currentModuleDd.hasSynchronizingAction(action.first)) {
                        action.second = combineUnsynchronizedActions(generationInfo, action.second, emptyAction, system.identity, currentModuleDd.identity);
                    }
                }
                
                // Combine synchronizing actions.
                for (auto const& actionIndex : currentModule.getSynchronizingActionIndices()) {
                    if (system.hasSynchronizingAction(actionIndex)) {
                        system.synchronizingActionToDecisionDiagramMap[actionIndex] = combineSynchronizingActions(generationInfo, system.synchronizingActionToDecisionDiagramMap[actionIndex], currentModuleDd.synchronizingActionToDecisionDiagramMap[actionIndex]);
                        numberOfUsedNondeterminismVariables = std::max(numberOfUsedNondeterminismVariables, system.synchronizingActionToDecisionDiagramMap[actionIndex].numberOfUsedNondeterminismVariables);
                    } else {
                        system.synchronizingActionToDecisionDiagramMap[actionIndex] = combineUnsynchronizedActions(generationInfo, emptyAction, currentModuleDd.synchronizingActionToDecisionDiagramMap[actionIndex], system.identity, currentModuleDd.identity);
                        numberOfUsedNondeterminismVariables = std::max(numberOfUsedNondeterminismVariables, system.synchronizingActionToDecisionDiagramMap[actionIndex].numberOfUsedNondeterminismVariables);
                    }
                }
                
                // Combine identity matrices.
                system.identity = system.identity * currentModuleDd.identity;
                
                // Keep track of the number of nondeterminism variables used.
                system.numberOfUsedNondeterminismVariables = std::max(system.numberOfUsedNondeterminismVariables, numberOfUsedNondeterminismVariables);
            }
            
            storm::dd::Add<Type> result = createSystemFromModule(generationInfo, system);

            // Create an auxiliary DD that is used later during the construction of reward models.
            storm::dd::Add<Type> stateActionDd = result.sumAbstract(generationInfo.columnMetaVariables);

            // For DTMCs, we normalize each row to 1 (to account for non-determinism).
            if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::DTMC) {
                result = result / stateActionDd;
            } else if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::MDP) {
                // For MDPs, we need to throw away the nondeterminism variables from the generation information that
                // were never used.
                for (uint_fast64_t index = system.numberOfUsedNondeterminismVariables; index < generationInfo.nondeterminismMetaVariables.size(); ++index) {
                    generationInfo.allNondeterminismVariables.erase(generationInfo.nondeterminismMetaVariables[index]);
                }
                generationInfo.nondeterminismMetaVariables.resize(system.numberOfUsedNondeterminismVariables);
            }
            
            return SystemResult(result, system, stateActionDd);
        }
        
        template <storm::dd::DdType Type>
        storm::models::symbolic::StandardRewardModel<Type, double> DdPrismModelBuilder<Type>::createRewardModelDecisionDiagrams(GenerationInformation& generationInfo, storm::prism::RewardModel const& rewardModel, ModuleDecisionDiagram const& globalModule, storm::dd::Add<Type> const& transitionMatrix, storm::dd::Add<Type> const& reachableStatesAdd, storm::dd::Add<Type> const& stateActionDd) {
            
            // Start by creating the state reward vector.
            boost::optional<storm::dd::Add<Type>> stateRewards;
            if (rewardModel.hasStateRewards()) {
                stateRewards = generationInfo.manager->getAddZero();
                
                for (auto const& stateReward : rewardModel.getStateRewards()) {
                    storm::dd::Add<Type> states = generationInfo.rowExpressionAdapter->translateExpression(stateReward.getStatePredicateExpression());
                    storm::dd::Add<Type> rewards = generationInfo.rowExpressionAdapter->translateExpression(stateReward.getRewardValueExpression());
                    
                    // Restrict the rewards to those states that satisfy the condition.
                    rewards = reachableStatesAdd * states * rewards;
                    
                    // Perform some sanity checks.
                    STORM_LOG_WARN_COND(rewards.getMin() >= 0, "The reward model assigns negative rewards to some states.");
                    STORM_LOG_WARN_COND(!rewards.isZero(), "The reward model does not assign any non-zero rewards.");
                    
                    // Add the rewards to the global state reward vector.
                    stateRewards.get() += rewards;
                }
            }
            
            // Next, build the state-action reward vector.
            boost::optional<storm::dd::Add<Type>> stateActionRewards;
            if (rewardModel.hasStateActionRewards()) {
                 stateActionRewards = generationInfo.manager->getAddZero();
                
                for (auto const& stateActionReward : rewardModel.getStateActionRewards()) {
                    storm::dd::Add<Type> states = generationInfo.rowExpressionAdapter->translateExpression(stateActionReward.getStatePredicateExpression());
                    storm::dd::Add<Type> rewards = generationInfo.rowExpressionAdapter->translateExpression(stateActionReward.getRewardValueExpression());
                    storm::dd::Add<Type> synchronization = generationInfo.manager->getAddOne();
                    
                    if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::MDP) {
                        synchronization = getSynchronizationDecisionDiagram(generationInfo, stateActionReward.getActionIndex());
                    }
                    ActionDecisionDiagram const& actionDd = stateActionReward.isLabeled() ? globalModule.synchronizingActionToDecisionDiagramMap.at(stateActionReward.getActionIndex()) : globalModule.independentAction;
                    states *= actionDd.guardDd * reachableStatesAdd;
                    storm::dd::Add<Type> stateActionRewardDd = synchronization * states * rewards;

                    // If we are building the state-action rewards for an MDP, we need to make sure that the encoding
                    // of the nondeterminism is present in the reward vector, so we ne need to multiply it with the
                    // legal state-actions.
                    if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::MDP) {
                        stateActionRewardDd *= stateActionDd;
                    } else if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::CTMC) {
                        // For CTMCs, we need to multiply the entries with the exit rate of the corresponding action.
                        stateActionRewardDd *= actionDd.transitionsDd.sumAbstract(generationInfo.columnMetaVariables);
                    }
                    
                    // Perform some sanity checks.
                    STORM_LOG_WARN_COND(stateActionRewardDd.getMin() >= 0, "The reward model assigns negative rewards to some states.");
                    STORM_LOG_WARN_COND(!stateActionRewardDd.isZero(), "The reward model does not assign any non-zero rewards.");
                    
                    // Add the rewards to the global transition reward matrix.
                    stateActionRewards.get() += stateActionRewardDd;
                }
                
                // Scale state-action rewards for DTMCs and CTMCs.
                if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::DTMC || generationInfo.program.getModelType() == storm::prism::Program::ModelType::CTMC) {
                    stateActionRewards.get() /= stateActionDd;
                }
            }
            
            // Then build the transition reward matrix.
            boost::optional<storm::dd::Add<Type>> transitionRewards;
            if (rewardModel.hasTransitionRewards()) {
                transitionRewards = generationInfo.manager->getAddZero();
                
                for (auto const& transitionReward : rewardModel.getTransitionRewards()) {
                    storm::dd::Add<Type> sourceStates = generationInfo.rowExpressionAdapter->translateExpression(transitionReward.getSourceStatePredicateExpression());
                    storm::dd::Add<Type> targetStates = generationInfo.rowExpressionAdapter->translateExpression(transitionReward.getTargetStatePredicateExpression());
                    storm::dd::Add<Type> rewards = generationInfo.rowExpressionAdapter->translateExpression(transitionReward.getRewardValueExpression());
                    
                    storm::dd::Add<Type> synchronization = generationInfo.manager->getAddOne();
                    
                    storm::dd::Add<Type> transitions;
                    if (transitionReward.isLabeled()) {
                        if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::MDP) {
                            synchronization = getSynchronizationDecisionDiagram(generationInfo, transitionReward.getActionIndex());
                        }
                        transitions = globalModule.synchronizingActionToDecisionDiagramMap.at(transitionReward.getActionIndex()).transitionsDd;
                    } else {
                        if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::MDP) {
                            synchronization = getSynchronizationDecisionDiagram(generationInfo);
                        }
                        transitions = globalModule.independentAction.transitionsDd;
                    }
                    
                    storm::dd::Add<Type> transitionRewardDd = synchronization * sourceStates * targetStates * rewards;
                    if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::DTMC) {
                        // For DTMCs we need to keep the weighting for the scaling that follows.
                        transitionRewardDd = transitions * transitionRewardDd;
                    } else {
                        // For all other model types, we do not scale the rewards.
                        transitionRewardDd = transitions.notZero().toAdd() * transitionRewardDd;
                    }
                    
                    // Perform some sanity checks.
                    STORM_LOG_WARN_COND(transitionRewardDd.getMin() >= 0, "The reward model assigns negative rewards to some states.");
                    STORM_LOG_WARN_COND(!transitionRewardDd.isZero(), "The reward model does not assign any non-zero rewards.");
                    
                    // Add the rewards to the global transition reward matrix.
                    transitionRewards.get() += transitionRewardDd;
                }
                
                // Scale transition rewards for DTMCs.
                if (generationInfo.program.getModelType() == storm::prism::Program::ModelType::DTMC) {
                    transitionRewards.get() /= stateActionDd;
                }
            }
            
            return storm::models::symbolic::StandardRewardModel<Type, double>(stateRewards, stateActionRewards, transitionRewards);
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
            
            STORM_LOG_DEBUG("Building representation of program :" << std::endl << preparedProgram << std::endl);
            
            // Start by initializing the structure used for storing all information needed during the model generation.
            // In particular, this creates the meta variables used to encode the model.
            GenerationInformation generationInfo(preparedProgram);

            SystemResult system = createSystemDecisionDiagram(generationInfo);
            storm::dd::Add<Type> transitionMatrix = system.allTransitionsDd;
            ModuleDecisionDiagram const& globalModule = system.globalModule;
            storm::dd::Add<Type> stateActionDd = system.stateActionDd;
            
            // If we were asked to treat some states as terminal states, we cut away their transitions now.
            if (options.terminalStates || options.negatedTerminalStates) {
                storm::dd::Add<Type> terminalStatesAdd = generationInfo.manager->getAddZero();
                if (options.terminalStates) {
                    storm::expressions::Expression terminalExpression;
                    if (options.terminalStates.get().type() == typeid(storm::expressions::Expression)) {
                        terminalExpression = boost::get<storm::expressions::Expression>(options.terminalStates.get());
                    } else {
                        std::string const& labelName = boost::get<std::string>(options.terminalStates.get());
                        terminalExpression = preparedProgram.getLabelExpression(labelName);
                    }
                
                    STORM_LOG_TRACE("Making the states satisfying " << terminalExpression << " terminal.");
                    terminalStatesAdd = generationInfo.rowExpressionAdapter->translateExpression(terminalExpression);
                }
                if (options.negatedTerminalStates) {
                    storm::expressions::Expression nonTerminalExpression;
                    if (options.negatedTerminalStates.get().type() == typeid(storm::expressions::Expression)) {
                        nonTerminalExpression = boost::get<storm::expressions::Expression>(options.negatedTerminalStates.get());
                    } else {
                        std::string const& labelName = boost::get<std::string>(options.terminalStates.get());
                        nonTerminalExpression = preparedProgram.getLabelExpression(labelName);
                    }
                    
                    STORM_LOG_TRACE("Making the states *not* satisfying " << nonTerminalExpression << " terminal.");
                    terminalStatesAdd |= !generationInfo.rowExpressionAdapter->translateExpression(nonTerminalExpression);
                }
                
                transitionMatrix *= !terminalStatesAdd;
            }
            
            // Cut the transitions and rewards to the reachable fragment of the state space.
            storm::dd::Bdd<Type> initialStates = createInitialStatesDecisionDiagram(generationInfo);
            storm::dd::Bdd<Type> transitionMatrixBdd = transitionMatrix.notZero();
            if (program.getModelType() == storm::prism::Program::ModelType::MDP) {
                transitionMatrixBdd = transitionMatrixBdd.existsAbstract(generationInfo.allNondeterminismVariables);
            }
            
            storm::dd::Bdd<Type> reachableStates = computeReachableStates(generationInfo, initialStates, transitionMatrixBdd);
            storm::dd::Add<Type> reachableStatesAdd = reachableStates.toAdd();
            transitionMatrix *= reachableStatesAdd;
            stateActionDd *= reachableStatesAdd;

            // Detect deadlocks and 1) fix them if requested 2) throw an error otherwise.
            storm::dd::Bdd<Type> statesWithTransition = transitionMatrixBdd.existsAbstract(generationInfo.columnMetaVariables);
            storm::dd::Add<Type> deadlockStates = (reachableStates && !statesWithTransition).toAdd();

            if (!deadlockStates.isZero()) {
                // If we need to fix deadlocks, we do so now.
                if (!storm::settings::generalSettings().isDontFixDeadlocksSet()) {
                    STORM_LOG_INFO("Fixing deadlocks in " << deadlockStates.getNonZeroCount() << " states. The first three of these states are: ");
                    
                    uint_fast64_t count = 0;
                    for (auto it = deadlockStates.begin(), ite = deadlockStates.end(); it != ite && count < 3; ++it, ++count) {
                        STORM_LOG_INFO((*it).first.toPrettyString(generationInfo.rowMetaVariables) << std::endl);
                    }

                    if (program.getModelType() == storm::prism::Program::ModelType::DTMC) {
                        // For DTMCs, we can simply add the identity of the global module for all deadlock states.
                        transitionMatrix += deadlockStates * globalModule.identity;
                    } else if (program.getModelType() == storm::prism::Program::ModelType::MDP) {
                        // For MDPs, however, we need to select an action associated with the self-loop, if we do not
                        // want to attach a lot of self-loops to the deadlock states.
                        storm::dd::Add<Type> action = generationInfo.manager->getAddOne();
                        std::for_each(generationInfo.allNondeterminismVariables.begin(), generationInfo.allNondeterminismVariables.end(), [&action,&generationInfo] (storm::expressions::Variable const& metaVariable) { action *= !generationInfo.manager->getIdentity(metaVariable); } );
                        // Make sure that global variables do not change along the introduced self-loops.
                        for (auto const& var : generationInfo.allGlobalVariables) {
                            action *= generationInfo.variableToIdentityMap.at(var);
                        }
                        transitionMatrix += deadlockStates * globalModule.identity * action;
                    }
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The model contains " << deadlockStates.getNonZeroCount() << " deadlock states. Please unset the option to not fix deadlocks, if you want to fix them automatically.");
                }
            }
            
            // Now build the reward models.
            std::vector<std::reference_wrapper<storm::prism::RewardModel const>> selectedRewardModels;
            
            // First, we make sure that all selected reward models actually exist.
            for (auto const& rewardModelName : options.rewardModelsToBuild) {
                STORM_LOG_THROW(rewardModelName.empty() || preparedProgram.hasRewardModel(rewardModelName), storm::exceptions::InvalidArgumentException, "Model does not possess a reward model with the name '" << rewardModelName << "'.");
            }
            
            for (auto const& rewardModel : preparedProgram.getRewardModels()) {
                if (options.buildAllRewardModels || options.rewardModelsToBuild.find(rewardModel.getName()) != options.rewardModelsToBuild.end()) {
                    selectedRewardModels.push_back(rewardModel);
                }
            }
            // If no reward model was selected until now and a referenced reward model appears to be unique, we build
            // the only existing reward model (given that no explicit name was given for the referenced reward model).
            if (selectedRewardModels.empty() && preparedProgram.getNumberOfRewardModels() == 1 && options.rewardModelsToBuild.size() == 1 && *options.rewardModelsToBuild.begin() == "") {
                selectedRewardModels.push_back(preparedProgram.getRewardModel(0));
            }
            
            std::unordered_map<std::string, storm::models::symbolic::StandardRewardModel<Type, double>> rewardModels;
            for (auto const& rewardModel : selectedRewardModels) {
                rewardModels.emplace(rewardModel.get().getName(), createRewardModelDecisionDiagrams(generationInfo, rewardModel.get(), globalModule, transitionMatrix, reachableStatesAdd, stateActionDd));
            }
            
            // Build the labels that can be accessed as a shortcut.
            std::map<std::string, storm::expressions::Expression> labelToExpressionMapping;
            for (auto const& label : preparedProgram.getLabels()) {
                labelToExpressionMapping.emplace(label.getName(), label.getStatePredicateExpression());
            }
            
            if (program.getModelType() == storm::prism::Program::ModelType::DTMC) {
                return std::shared_ptr<storm::models::symbolic::Model<Type>>(new storm::models::symbolic::Dtmc<Type>(generationInfo.manager, reachableStates, initialStates, transitionMatrix, generationInfo.rowMetaVariables, generationInfo.rowExpressionAdapter, generationInfo.columnMetaVariables, generationInfo.columnExpressionAdapter, generationInfo.rowColumnMetaVariablePairs, labelToExpressionMapping, rewardModels));
            } else if (program.getModelType() == storm::prism::Program::ModelType::CTMC) {
                return std::shared_ptr<storm::models::symbolic::Model<Type>>(new storm::models::symbolic::Ctmc<Type>(generationInfo.manager, reachableStates, initialStates, transitionMatrix, generationInfo.rowMetaVariables, generationInfo.rowExpressionAdapter, generationInfo.columnMetaVariables, generationInfo.columnExpressionAdapter, generationInfo.rowColumnMetaVariablePairs, labelToExpressionMapping, rewardModels));
            } else if (program.getModelType() == storm::prism::Program::ModelType::MDP) {
                return std::shared_ptr<storm::models::symbolic::Model<Type>>(new storm::models::symbolic::Mdp<Type>(generationInfo.manager, reachableStates, initialStates, transitionMatrix, generationInfo.rowMetaVariables, generationInfo.rowExpressionAdapter, generationInfo.columnMetaVariables, generationInfo.columnExpressionAdapter, generationInfo.rowColumnMetaVariablePairs, generationInfo.allNondeterminismVariables, labelToExpressionMapping, rewardModels));
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Invalid model type.");
            }
        }
        
        template <storm::dd::DdType Type>
        storm::dd::Bdd<Type> DdPrismModelBuilder<Type>::createInitialStatesDecisionDiagram(GenerationInformation& generationInfo) {
            storm::dd::Bdd<Type> initialStates = generationInfo.rowExpressionAdapter->translateExpression(generationInfo.program.getInitialConstruct().getInitialStatesExpression()).toBdd();
            
            for (auto const& metaVariable : generationInfo.rowMetaVariables) {
                initialStates &= generationInfo.manager->getRange(metaVariable);
            }
            
            return initialStates;
        }
        
        template <storm::dd::DdType Type>
        storm::dd::Bdd<Type> DdPrismModelBuilder<Type>::computeReachableStates(GenerationInformation& generationInfo, storm::dd::Bdd<Type> const& initialStates, storm::dd::Bdd<Type> const& transitionBdd) {
            storm::dd::Bdd<Type> reachableStates = initialStates;
            
            // Perform the BFS to discover all reachable states.
            bool changed = true;
            uint_fast64_t iteration = 0;
            do {
                STORM_LOG_TRACE("Iteration " << iteration << " of reachability analysis.");
                changed = false;
                storm::dd::Bdd<Type> tmp = reachableStates.andExists(transitionBdd, generationInfo.rowMetaVariables);
                tmp = tmp.swapVariables(generationInfo.rowColumnMetaVariablePairs);

                storm::dd::Bdd<Type> newReachableStates = tmp && (!reachableStates);
                
                // Check whether new states were indeed discovered.
                if (!newReachableStates.isZero()) {
                    changed = true;
                }
                
                reachableStates |= newReachableStates;
                ++iteration;
            } while (changed);
            
            return reachableStates;
        }
        
        // Explicitly instantiate the symbolic expression adapter
        template class DdPrismModelBuilder<storm::dd::DdType::CUDD>;
        
    } // namespace adapters
} // namespace storm


