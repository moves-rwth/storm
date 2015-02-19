#ifndef STORM_BUILDER_DDPRISMMODELBUILDER_H_
#define STORM_BUILDER_DDPRISMMODELBUILDER_H_

#include <map>
#include <boost/optional.hpp>

#include "src/logic/Formulas.h"
#include "src/storage/prism/Program.h"
#include "src/adapters/DdExpressionAdapter.h"
#include "src/utility/macros.h"

namespace storm {
    namespace builder {
        
        template <storm::dd::DdType Type>
        class DdPrismModelBuilder {
        public:
            struct Options {
                /*!
                 * Creates an object representing the default building options.
                 */
                Options();
                
                /*! Creates an object representing the suggested building options assuming that the given formula is the
                 * only one to check.
                 *
                 * @param formula The formula based on which to choose the building options.
                 */
                Options(storm::logic::Formula const& formula);
                
                /*!
                 * Sets the constants definitions from the given string. The string must be of the form 'X=a,Y=b,Z=c',
                 * etc. where X,Y,Z are the variable names and a,b,c are the values of the constants.
                 *
                 * @param program The program managing the constants that shall be defined. Note that the program itself
                 * is not modified whatsoever.
                 * @param constantDefinitionString The string from which to parse the constants' values.
                 */
                void addConstantDefinitionsFromString(storm::prism::Program const& program, std::string const& constantDefinitionString);
                
                // A flag that indicates whether or not a reward model is to be built.
                bool buildRewards;
                
                // An optional string, that, if given, indicates which of the reward models is to be built.
                boost::optional<std::string> rewardModelName;
                
                // An optional mapping that, if given, contains defining expressions for undefined constants.
                boost::optional<std::map<storm::expressions::Variable, storm::expressions::Expression>> constantDefinitions;
                
                // An optional set of labels that, if given, restricts the labels that are built.
                boost::optional<std::set<std::string>> labelsToBuild;
                
                // An optional set of expressions for which labels need to be built.
                boost::optional<std::vector<storm::expressions::Expression>> expressionLabels;
            };
            
            /*!
             * Translates the given program into a model that stores the transition relation as a decision diagram.
             *
             * @param program The program to translate.
             */
            static std::pair<storm::dd::Dd<Type>, storm::dd::Dd<Type>> translateProgram(storm::prism::Program const& program, Options const& options = Options());
            
        private:
            // This structure can store the decision diagrams representing a particular action.
            struct ActionDecisionDiagram {
                ActionDecisionDiagram() : guardDd(), transitionsDd(), numberOfUsedNondeterminismVariables(0) {
                    // Intentionally left empty.
                }
                
                ActionDecisionDiagram(storm::dd::DdManager<Type> const& manager, uint_fast64_t numberOfUsedNondeterminismVariables = 0) : guardDd(manager.getZero(true)), transitionsDd(manager.getZero(true)), numberOfUsedNondeterminismVariables(numberOfUsedNondeterminismVariables) {
                    // Intentionally left empty.
                }
                
                ActionDecisionDiagram(storm::dd::Dd<Type> guardDd, storm::dd::Dd<Type> transitionsDd, uint_fast64_t numberOfUsedNondeterminismVariables = 0) : guardDd(guardDd), transitionsDd(transitionsDd), numberOfUsedNondeterminismVariables(numberOfUsedNondeterminismVariables) {
                    // Intentionally left empty.
                }
                
                ActionDecisionDiagram(ActionDecisionDiagram const& other) = default;
                ActionDecisionDiagram& operator=(ActionDecisionDiagram const& other) = default;
                
                // The guard of the action.
                storm::dd::Dd<Type> guardDd;
                
                // The actual transitions (source and target states).
                storm::dd::Dd<Type> transitionsDd;
                
                // The number of variables that are used to encode the nondeterminism.
                uint_fast64_t numberOfUsedNondeterminismVariables;
            };
            
            // This structure holds all decision diagrams related to a module.
            struct ModuleDecisionDiagram {
                ModuleDecisionDiagram() : independentAction(), synchronizingActionToDecisionDiagramMap(), identity(), numberOfUsedNondeterminismVariables(0) {
                    // Intentionally left empty.
                }
                
                ModuleDecisionDiagram(storm::dd::DdManager<Type> const& manager) : independentAction(manager), synchronizingActionToDecisionDiagramMap(), identity(manager.getZero(true)), numberOfUsedNondeterminismVariables(0) {
                    // Intentionally left empty.
                }

                ModuleDecisionDiagram(ActionDecisionDiagram const& independentAction, std::map<uint_fast64_t, ActionDecisionDiagram> const& synchronizingActionToDecisionDiagramMap, storm::dd::Dd<Type> const& identity, uint_fast64_t numberOfUsedNondeterminismVariables = 0) : independentAction(independentAction), synchronizingActionToDecisionDiagramMap(synchronizingActionToDecisionDiagramMap), identity(identity), numberOfUsedNondeterminismVariables(numberOfUsedNondeterminismVariables) {
                    // Intentionally left empty.
                }
                
                ModuleDecisionDiagram(ModuleDecisionDiagram const& other) = default;
                ModuleDecisionDiagram& operator=(ModuleDecisionDiagram const& other) = default;
                
                bool hasSynchronizingAction(uint_fast64_t actionIndex) {
                    return synchronizingActionToDecisionDiagramMap.find(actionIndex) != synchronizingActionToDecisionDiagramMap.end();
                }
                
                // The decision diagram for the independent action.
                ActionDecisionDiagram independentAction;
                
                // A mapping from synchronizing action indices to the decision diagram.
                std::map<uint_fast64_t, ActionDecisionDiagram> synchronizingActionToDecisionDiagramMap;
                
                // A decision diagram that represents the identity of this module.
                storm::dd::Dd<Type> identity;
                
                // The number of variables encoding the nondeterminism that were actually used.
                uint_fast64_t numberOfUsedNondeterminismVariables;
            };
            
            /*!
             * Structure to store all information required to generate the model from the program.
             */
            class GenerationInformation {
            public:
                GenerationInformation(storm::prism::Program const& program) : program(program), manager(std::make_shared<storm::dd::DdManager<Type>>()), rowMetaVariables(), variableToRowMetaVariableMap(), rowExpressionAdapter(nullptr), columnMetaVariables(), variableToColumnMetaVariableMap(), columnExpressionAdapter(nullptr), rowColumnMetaVariablePairs(), nondeterminismMetaVariables(), variableToIdentityMap(), moduleToIdentityMap() {
                    // Initializes variables and identity DDs.
                    createMetaVariablesAndIdentities();
                    
                    rowExpressionAdapter = std::unique_ptr<storm::adapters::DdExpressionAdapter<Type>>(new storm::adapters::DdExpressionAdapter<Type>(*manager, variableToRowMetaVariableMap));
                    columnExpressionAdapter = std::unique_ptr<storm::adapters::DdExpressionAdapter<Type>>(new storm::adapters::DdExpressionAdapter<Type>(*manager, variableToColumnMetaVariableMap));
                }
                
                // The program that is currently translated.
                storm::prism::Program const& program;
                
                // The manager used to build the decision diagrams.
                std::shared_ptr<storm::dd::DdManager<Type>> manager;

                // The meta variables for the row encoding.
                std::set<storm::expressions::Variable> rowMetaVariables;
                std::map<storm::expressions::Variable, storm::expressions::Variable> variableToRowMetaVariableMap;
                std::unique_ptr<storm::adapters::DdExpressionAdapter<Type>> rowExpressionAdapter;
                
                // The meta variables for the column encoding.
                std::set<storm::expressions::Variable> columnMetaVariables;
                std::map<storm::expressions::Variable, storm::expressions::Variable> variableToColumnMetaVariableMap;
                std::unique_ptr<storm::adapters::DdExpressionAdapter<Type>> columnExpressionAdapter;
                
                // All pairs of row/column meta variables.
                std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> rowColumnMetaVariablePairs;
                
                // The meta variables used to encode the nondeterminism.
                std::vector<storm::expressions::Variable> nondeterminismMetaVariables;
                
                // The meta variables used to encode the synchronization.
                std::vector<storm::expressions::Variable> synchronizationMetaVariables;
                
                // A set of all variables used for encoding the nondeterminism (i.e. nondetermism + synchronization
                // variables). This is handy to abstract from this variable set.
                std::set<storm::expressions::Variable> allNondeterminismVariables;
                
                // DDs representing the identity for each variable.
                std::map<storm::expressions::Variable, storm::dd::Dd<Type>> variableToIdentityMap;
                
                // DDs representing the identity for each module.
                std::map<std::string, storm::dd::Dd<Type>> moduleToIdentityMap;
                
                // DDs representing the valid ranges of the variables of each module.
                std::map<std::string, storm::dd::Dd<Type>> moduleToRangeMap;
                
            private:
                /*!
                 * Creates the required meta variables and variable/module identities.
                 */
                void createMetaVariablesAndIdentities() {
                    // Add synchronization variables.
                    for (auto const& actionIndex : program.getActionIndices()) {
                        std::pair<storm::expressions::Variable, storm::expressions::Variable> variablePair = manager->addMetaVariable(program.getActionName(actionIndex));
                        synchronizationMetaVariables.push_back(variablePair.first);
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
                        
                        rowMetaVariables.insert(variablePair.first);
                        variableToRowMetaVariableMap.emplace(integerVariable.getExpressionVariable(), variablePair.first);
                        
                        columnMetaVariables.insert(variablePair.second);
                        variableToColumnMetaVariableMap.emplace(integerVariable.getExpressionVariable(), variablePair.second);
                        
                        storm::dd::Dd<Type> variableIdentity = manager->getIdentity(variablePair.first).equals(manager->getIdentity(variablePair.second)) * manager->getRange(variablePair.first, true);
                        variableToIdentityMap.emplace(integerVariable.getExpressionVariable(), variableIdentity);
                        rowColumnMetaVariablePairs.push_back(variablePair);
                    }
                    for (storm::prism::BooleanVariable const& booleanVariable : program.getGlobalBooleanVariables()) {
                        std::pair<storm::expressions::Variable, storm::expressions::Variable> variablePair = manager->addMetaVariable(booleanVariable.getName());
                        
                        rowMetaVariables.insert(variablePair.first);
                        variableToRowMetaVariableMap.emplace(booleanVariable.getExpressionVariable(), variablePair.first);
                        
                        columnMetaVariables.insert(variablePair.second);
                        variableToColumnMetaVariableMap.emplace(booleanVariable.getExpressionVariable(), variablePair.second);
                        
                        storm::dd::Dd<Type> variableIdentity = manager->getIdentity(variablePair.first).equals(manager->getIdentity(variablePair.second));
                        variableToIdentityMap.emplace(booleanVariable.getExpressionVariable(), variableIdentity);
                        
                        rowColumnMetaVariablePairs.push_back(variablePair);
                    }

                    // Create meta variables for each of the modules' variables.
                    for (storm::prism::Module const& module : program.getModules()) {
                        storm::dd::Dd<Type> moduleIdentity = manager->getOne(true);
                        storm::dd::Dd<Type> moduleRange = manager->getOne(true);
                        
                        for (storm::prism::IntegerVariable const& integerVariable : module.getIntegerVariables()) {
                            int_fast64_t low = integerVariable.getLowerBoundExpression().evaluateAsInt();
                            int_fast64_t high = integerVariable.getUpperBoundExpression().evaluateAsInt();
                            std::pair<storm::expressions::Variable, storm::expressions::Variable> variablePair = manager->addMetaVariable(integerVariable.getName(), low, high);
                            STORM_LOG_TRACE("Created meta variables for integer variable: " << variablePair.first.getName() << "[" << variablePair.first.getIndex() << "] and " << variablePair.second.getName() << "[" << variablePair.second.getIndex() << "]");
                            
                            rowMetaVariables.insert(variablePair.first);
                            variableToRowMetaVariableMap.emplace(integerVariable.getExpressionVariable(), variablePair.first);
                            
                            columnMetaVariables.insert(variablePair.second);
                            variableToColumnMetaVariableMap.emplace(integerVariable.getExpressionVariable(), variablePair.second);
                            
                            storm::dd::Dd<Type> variableIdentity = manager->getIdentity(variablePair.first).equals(manager->getIdentity(variablePair.second)) * manager->getRange(variablePair.first, true) * manager->getRange(variablePair.second, true);
                            variableToIdentityMap.emplace(integerVariable.getExpressionVariable(), variableIdentity);
                            moduleIdentity *= variableIdentity;
                            moduleRange *= manager->getRange(variablePair.first, true);
                            
                            rowColumnMetaVariablePairs.push_back(variablePair);
                        }
                        for (storm::prism::BooleanVariable const& booleanVariable : module.getBooleanVariables()) {
                            std::pair<storm::expressions::Variable, storm::expressions::Variable> variablePair = manager->addMetaVariable(booleanVariable.getName());
                            STORM_LOG_TRACE("Created meta variables for boolean variable: " << variablePair.first.getName() << "[" << variablePair.first.getIndex() << "] and " << variablePair.second.getName() << "[" << variablePair.second.getIndex() << "]");

                            rowMetaVariables.insert(variablePair.first);
                            variableToRowMetaVariableMap.emplace(booleanVariable.getExpressionVariable(), variablePair.first);
                            
                            columnMetaVariables.insert(variablePair.second);
                            variableToColumnMetaVariableMap.emplace(booleanVariable.getExpressionVariable(), variablePair.second);
                            
                            storm::dd::Dd<Type> variableIdentity = manager->getIdentity(variablePair.first).equals(manager->getIdentity(variablePair.second)) * manager->getRange(variablePair.first, true) * manager->getRange(variablePair.second, true);
                            variableToIdentityMap.emplace(booleanVariable.getExpressionVariable(), variableIdentity);
                            moduleIdentity *= variableIdentity;
                            moduleRange *= manager->getRange(variablePair.first, true);

                            rowColumnMetaVariablePairs.push_back(variablePair);
                        }
                        moduleToIdentityMap[module.getName()] = moduleIdentity;
                        moduleToRangeMap[module.getName()] = moduleRange;
                    }
                }
            };
            
        private:
            
            static storm::dd::Dd<Type> encodeChoice(GenerationInformation& generationInfo, uint_fast64_t nondeterminismVariableOffset, uint_fast64_t numberOfBinaryVariables, int_fast64_t value);
            
            static storm::dd::Dd<Type> createUpdateDecisionDiagram(GenerationInformation& generationInfo, storm::prism::Module const& module, storm::dd::Dd<Type> const& guard, storm::prism::Update const& update);

            static ActionDecisionDiagram createCommandDecisionDiagram(GenerationInformation& generationInfo, storm::prism::Module const& module, storm::prism::Command const& command);

            static ActionDecisionDiagram createActionDecisionDiagram(GenerationInformation& generationInfo, storm::prism::Module const& module, boost::optional<uint_fast64_t> synchronizationActionIndex, uint_fast64_t nondeterminismVariableOffset);

            static ActionDecisionDiagram combineCommandsToActionDTMC(GenerationInformation& generationInfo, std::vector<ActionDecisionDiagram> const& commandDds);

            static ActionDecisionDiagram combineCommandsToActionMDP(GenerationInformation& generationInfo, std::vector<ActionDecisionDiagram> const& commandDds, uint_fast64_t nondeterminismVariableOffset);

            static ActionDecisionDiagram combineSynchronizingActions(GenerationInformation const& generationInfo, ActionDecisionDiagram const& action1, ActionDecisionDiagram const& action2);

            static ActionDecisionDiagram combineUnsynchronizedActions(GenerationInformation const& generationInfo, ActionDecisionDiagram const& action1, ActionDecisionDiagram const& action2, storm::dd::Dd<Type> const& identityDd1, storm::dd::Dd<Type> const& identityDd2);

            static ModuleDecisionDiagram createModuleDecisionDiagram(GenerationInformation& generationInfo, storm::prism::Module const& module, std::map<uint_fast64_t, uint_fast64_t> const& synchronizingActionToOffsetMap);

            static storm::dd::Dd<Type> createSystemFromModule(GenerationInformation& generationInfo, ModuleDecisionDiagram const& module);
            
            static std::pair<storm::dd::Dd<Type>, ModuleDecisionDiagram> createSystemDecisionDiagram(GenerationInformation& generationInfo);
            
            static storm::dd::Dd<Type> createInitialStatesDecisionDiagram(GenerationInformation& generationInfo);

            static storm::dd::Dd<Type> computeReachableStates(GenerationInformation& generationInfo, storm::dd::Dd<Type> const& initialStates, storm::dd::Dd<Type> const& transitions);
            
//            /*!
//             * Calculates the reachable states of the given transition matrix
//             *
//             * @param systemDd The transition matrix DD
//             * @param initialStateDd All initial states
//             * @return A DD representing all reachable states
//             */
//            static storm::dd::Dd<Type> performReachability(GenerationInformation & generationInfo, storm::dd::Dd<Type> const& systemDd, storm::dd::Dd<Type> const& initialStateDd);
//
//            /*!
//             * Adds a self-loop to deadlock states
//             *
//             * @param systemDd The given DD
//             * @param reachableStatesDd DD representing all reachable states
//             * @return A DD with fixed deadlocks.
//             */
//            static storm::dd::Dd<Type> findDeadlocks(GenerationInformation const & generationInfo, storm::dd::Dd<Type> systemDd, storm::dd::Dd<Type> const& reachableStatesDd);
//            
//            /*!
//             * Computes state and transition rewards
//             *
//             * @param systemDds System DDs
//             */
//            static std::pair<std::vector<storm::dd::Dd<Type>>, std::vector<storm::dd::Dd<Type>>> computeRewards(GenerationInformation const & generationInfo, SystemComponentDecisionDiagram<Type> const& systemDds);
        };
        
    } // namespace adapters
} // namespace storm

#endif /* STORM_BUILDER_DDPRISMMODELBUILDER_H_ */
