#include "src/builder/DdPrismModelBuilder.h"
#include "src/adapters/DdExpressionAdapter.h"

#include "src/storage/dd/CuddDd.h"
#include "src/storage/dd/CuddDdManager.h"
#include "src/settings/SettingsManager.h"

#include "src/utility/prism.h"
#include "src/utility/math.h"
#include "src/utility/macros.h"

namespace storm {
    namespace adapters {
        
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
        void DdPrismModelBuilder<Type>::translateProgram(storm::prism::Program const& program, Options const& options) {
            // There might be nondeterministic variables. In that case the program must be prepared before translating.
            storm::prism::Program preparedProgram;
            if (options.constantDefinitions) {
                preparedProgram = program.defineUndefinedConstants(options.constantDefinitions.get());
            } else {
                preparedProgram = program;
            }
            
            preparedProgram = preparedProgram.substituteConstants();
            
            // Start by initializing the structure used for storing all information needed during the model generation.
            // In particular, this creates the meta variables used to encode the model.
//            GenerationInformation generationInfo(preparedProgram);

            
//            LOG4CPLUS_INFO(logger, "Creating MTBDD representation for probabilistic program.");
//            
//            // There might be nondeterministic variables. In that case the program must be prepared before translating.
//            storm::prism::Program preparedProgram = program.defineUndefinedConstants(std::map<std::string, storm::expressions::Expression>());
//            preparedProgram = preparedProgram.substituteConstants();
//
//            // Initialize the structure used for storing all information needed during the model generation.
//            GenerationInformation generationInfo(preparedProgram);
//
//            // Initial state DD
//            storm::dd::Dd<Type> initialStateDd = generationInfo.manager->getZero();
//            initialStateDd = getInitialStateDecisionDiagram(generationInfo);
//            
//            // System DD
//            storm::dd::Dd<Type> systemDd = generationInfo.manager->getZero();
//            
//            // Reachable states DD
//            storm::dd::Dd<Type> reachableStatesDd = generationInfo.manager->getOne();
//            
//            // Initialize the clock.
//            auto clock = std::chrono::high_resolution_clock::now();
//            
//            // Create system DD
//            systemDd = createSystemDecisionDiagramm(generationInfo);
//            std::cout << "Built transition matrix in " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - clock).count() << "ms." << std::endl;
//            
//            //manager->triggerReordering();
//            
//            // Get all reachable states
//            if (!storm::settings::adapterSettings().isNoReachSet()){
//                reachableStatesDd = performReachability(generationInfo, systemDd, initialStateDd);
//                // Reduce transition matrix
//                systemDd = systemDd * reachableStatesDd;
//            }
//            
//            // Fix deadlocks
//            if (!storm::settings::adapterSettings().isNoDeadlockSet()){
//                systemDd = findDeadlocks(generationInfo, systemDd, reachableStatesDd);
//            }
//            
//            // System DD with only 0/1 leaves
//            storm::dd::Dd<Type> systemBdd = systemDd.notZero();
//            
//            // Get all abstract variables (MDP)
//            std::set<std::string> abstractVariables = std::set<std::string>();
//            
//            if (program.getModelType() == storm::prism::Program::ModelType::MDP) {
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
//            // Create state labeling
//            std::unordered_map<std::string, storm::dd::Dd<Type>> labelToStateDdMap;
//            storm::dd::Dd<Type> temporary;
//            for (auto const& label : program.getLabels()) {
//                // Translate Labeling into Expression and further into DD
//                temporary = storm::adapters::SymbolicExpressionAdapter<Type>::translateExpression(label.getStatePredicateExpression().getBaseExpressionPointer(), generationInfo.program, generationInfo.manager, false);
//                temporary = temporary * reachableStatesDd;
//                labelToStateDdMap.insert(std::make_pair(label.getName(), temporary));
//            }
//            
//            // Create model
//            std::shared_ptr<storm::models::AbstractSymbolicModel<Type>> model;
//            switch (program.getModelType()){
//                case storm::prism::Program::ModelType::DTMC:
//                    model = std::shared_ptr<storm::models::AbstractSymbolicModel<Type>>(std::make_shared<storm::models::SymbolicDtmc<Type>>(std::move(systemDd),
//                                                                                                                                            std::move(systemBdd), std::move(initialStateDd), std::move(reachableStatesDd),
//                                                                                                                                            std::move(generationInfo.rewardDds.first), std::move(generationInfo.rewardDds.second), std::move(generationInfo.manager->getZero()),
//                                                                                                                                            std::move(generationInfo.rowMetaVariableNames), std::move(generationInfo.columnMetaVariableNames), std::move(generationInfo.metaVariablePairs),
//                                                                                                                                            std::move(generationInfo.allSynchronizingActions), std::move(abstractVariables), std::move(generationInfo.manager), std::move(labelToStateDdMap)));
//                    break;
//                case storm::prism::Program::ModelType::MDP:
//                    model = std::shared_ptr<storm::models::AbstractSymbolicModel<Type>>(std::make_shared<storm::models::SymbolicMdp<Type>>(std::move(systemDd),
//                                                                                                                                           std::move(systemBdd), std::move(initialStateDd), std::move(reachableStatesDd),
//                                                                                                                                           std::move(generationInfo.rewardDds.first), std::move(generationInfo.rewardDds.second), std::move(generationInfo.transitionActionDd),
//                                                                                                                                           std::move(generationInfo.rowMetaVariableNames), std::move(generationInfo.columnMetaVariableNames), std::move(generationInfo.metaVariablePairs),
//                                                                                                                                           std::move(generationInfo.allSynchronizingActions), std::move(abstractVariables), std::move(generationInfo.manager), std::move(labelToStateDdMap)));
//                    break;
//                default:
//                    STORM_LOG_ASSERT(false, "Illegal model type.");
//            }
//            
//            LOG4CPLUS_INFO(logger, "Done creating MTBDD representation for probabilistic preparedProgram.");
//            LOG4CPLUS_INFO(logger, "MTBDD: " << systemDd.getNodeCount() << " nodes, " << systemDd.getLeafCount() << " leaves, " << generationInfo.manager->getNumberOfMetaVariables() << " variables.");
//            return model;
        }
        
//        template <storm::dd::DdType Type>
//        void SymbolicModelAdapter<Type>::GenerationInformation::addDecisionDiagramVariables() {
//            
//            uint_fast64_t numberOfSynchronizingActions = allSynchronizingActions.size();
//            
//            // Add synchronizing variables
//            for (uint_fast64_t i = 0; i < numberOfSynchronizingActions; ++i) {
//                manager->addMetaVariable("sync" + std::to_string(i), 0, 1);
//            }
//            
//            // Add nondet. variables ( number of modules + number of commands)
//            numberOfNondetVariables = program.getModules().size();
//            for (uint_fast64_t i = 0; i < program.getModules().size(); ++i) {
//                numberOfNondetVariables += program.getModule(i).getCommands().size();
//            }
//            
//            for (uint_fast64_t i = numberOfNondetVariables; i > 0; --i) {
//                manager->addMetaVariable("nondet" + std::to_string(i), 0, 1);
//            }
//            
//            std::vector<std::string> newVariables;
//            
//            // Global Boolean Variables
//            for (uint_fast64_t j = 0; j < program.getGlobalBooleanVariables().size(); ++j) {
//                storm::prism::BooleanVariable const& booleanVariable = program.getGlobalBooleanVariables().at(j);
//                
//                // Add row and column meta variable
//                manager->addMetaVariable(booleanVariable.getName(), 0, 1);
//                
//                rowMetaVariableNames.insert(booleanVariable.getName());
//                columnMetaVariableNames.insert(booleanVariable.getName() + "'");
//                
//                // Create pair (column,row)
//                std::pair<std::string, std::string> variablePair(booleanVariable.getName() + "'", booleanVariable.getName());
//                metaVariablePairs.push_back(variablePair);
//            }
//            
//            // Global Integer Variables
//            for (uint_fast64_t j = 0; j < program.getGlobalIntegerVariables().size(); ++j) {
//                storm::prism::IntegerVariable const& integerVariable = program.getGlobalIntegerVariables().at(j);
//                
//                int_fast64_t low = integerVariable.getLowerBoundExpression().evaluateAsInt();
//                int_fast64_t high = integerVariable.getUpperBoundExpression().evaluateAsInt();
//                
//                // Add row and column meta variable
//                manager->addMetaVariable(integerVariable.getName(), low, high);
//                
//                rowMetaVariableNames.insert(integerVariable.getName());
//                columnMetaVariableNames.insert(integerVariable.getName() + "'");
//                
//                // Create pair (column,row)
//                std::pair<std::string, std::string> variablePair(integerVariable.getName() + "'", integerVariable.getName());
//                metaVariablePairs.push_back(variablePair);
//            }
//            
//            // Add boolean and integer variables
//            for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
//                storm::prism::Module const& module = program.getModule(i);
//                
//                // Boolean Variables
//                for (uint_fast64_t j = 0; j < module.getNumberOfBooleanVariables(); ++j) {
//                    storm::prism::BooleanVariable const& booleanVariable = module.getBooleanVariables().at(j);
//                    
//                    // Add row and column meta variable
//                    manager->addMetaVariable(booleanVariable.getName(), 0, 1);
//                    
//                    rowMetaVariableNames.insert(booleanVariable.getName());
//                    columnMetaVariableNames.insert(booleanVariable.getName() + "'");
//                    
//                    // Create pair (column,row)
//                    std::pair<std::string, std::string> variablePair(booleanVariable.getName() + "'", booleanVariable.getName());
//                    metaVariablePairs.push_back(variablePair);
//                }
//                
//                // Integer Variables
//                for (uint_fast64_t j = 0; j < module.getNumberOfIntegerVariables(); ++j) {
//                    storm::prism::IntegerVariable const& integerVariable = module.getIntegerVariables().at(j);
//                    
//                    int_fast64_t low = integerVariable.getLowerBoundExpression().evaluateAsInt();
//                    int_fast64_t high = integerVariable.getUpperBoundExpression().evaluateAsInt();
//                    
//                    // Add row and column meta variable
//                    manager->addMetaVariable(integerVariable.getName(), low, high);
//                    
//                    rowMetaVariableNames.insert(integerVariable.getName());
//                    columnMetaVariableNames.insert(integerVariable.getName() + "'");
//                    
//                    // Create pair (column,row)
//                    std::pair<std::string, std::string> variablePair(integerVariable.getName() + "'", integerVariable.getName());
//                    metaVariablePairs.push_back(variablePair);
//                }
//            }
//            
//            std::cout << "Variable Ordering: ";
//            for(auto x: manager->getDdVariableNames()){
//                std::cout << x << ",";
//            }
//            std::cout << std::endl;
//            std::cout << std::endl;
//        }
//        
//        template <storm::dd::DdType Type>
//        void SymbolicModelAdapter<Type>::GenerationInformation::createIdentityDecisionDiagrams() {
//            
//            // Global Boolean variables
//            for (uint_fast64_t j = 0; j < program.getGlobalBooleanVariables().size(); ++j) {
//                storm::prism::BooleanVariable const & booleanVariable = program.getGlobalBooleanVariables().at(j);
//                
//                storm::dd::Dd<Type> identity = manager->getZero();
//                
//                // f(0)=0 leads to 1
//                identity.setValue(booleanVariable.getName(), 0, booleanVariable.getName() + "'", 0, 1);
//                // f(1)=1 leads to 1
//                identity.setValue(booleanVariable.getName(), 1, booleanVariable.getName() + "'", 1, 1);
//                
//                variableToIdentityDecisionDiagramMap.insert(std::make_pair(booleanVariable.getName(), identity));
//            }
//            
//            // Global Integer variables
//            for (uint_fast64_t j = 0; j < program.getGlobalIntegerVariables().size(); ++j) {
//                storm::prism::IntegerVariable const& integerVariable = program.getGlobalIntegerVariables().at(j);
//                storm::dd::Dd<Type> identity = manager->getZero();
//                
//                int_fast64_t low = integerVariable.getLowerBoundExpression().evaluateAsInt();
//                int_fast64_t high = integerVariable.getUpperBoundExpression().evaluateAsInt();
//                
//                for (int_fast64_t i = low; i <= high; ++i) {
//                    // f(i)=i leads to 1
//                    identity.setValue(integerVariable.getName(), i, integerVariable.getName() + "'", i, 1);
//                }
//                
//                variableToIdentityDecisionDiagramMap.insert(std::make_pair(integerVariable.getName(), identity));
//            }
//            
//            
//            for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
//                storm::prism::Module const& module = program.getModule(i);
//                
//                // Module identity matrix
//                storm::dd::Dd<Type> moduleIdentity = manager->getOne();
//                
//                // Boolean variables
//                for (uint_fast64_t j = 0; j < module.getNumberOfBooleanVariables(); ++j) {
//                    storm::prism::BooleanVariable const& booleanVariable = module.getBooleanVariables().at(j);
//                    
//                    storm::dd::Dd<Type> identity = manager->getZero();
//                    
//                    // f(0)=0 leads to 1
//                    identity.setValue(booleanVariable.getName(), 0, booleanVariable.getName() + "'", 0, 1);
//                    // f(1)=1 leads to 1
//                    identity.setValue(booleanVariable.getName(), 1, booleanVariable.getName() + "'", 1, 1);
//                    
//                    variableToIdentityDecisionDiagramMap.insert(std::make_pair(booleanVariable.getName(), identity));
//                    moduleIdentity = moduleIdentity * identity;
//                }
//                
//                // Integer variables
//                for (uint_fast64_t j = 0; j < module.getNumberOfIntegerVariables(); ++j) {
//                    storm::prism::IntegerVariable const& integerVariable = module.getIntegerVariables().at(j);
//                    storm::dd::Dd<Type> identity = manager->getZero();
//                    
//                    int_fast64_t low = integerVariable.getLowerBoundExpression().evaluateAsInt();
//                    int_fast64_t high = integerVariable.getUpperBoundExpression().evaluateAsInt();
//                    
//                    for (int_fast64_t i = low; i <= high; ++i) {
//                        // f(i)=i leads to 1
//                        identity.setValue(integerVariable.getName(), i, integerVariable.getName() + "'", i, 1);
//                    }
//                    
//                    variableToIdentityDecisionDiagramMap.insert(std::make_pair(integerVariable.getName(), identity));
//                    moduleIdentity = moduleIdentity * identity;
//                }
//                
//                // Create module identity matrix
//                moduleToIdentityDecisionDiagramMap.insert(std::make_pair(module.getName(), moduleIdentity));
//            }
//            
//        }
//        
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
//        template <storm::dd::DdType Type>
//        ModuleDecisionDiagram<Type> SymbolicModelAdapter<Type>::combineCommandsDTMC(std::shared_ptr<storm::dd::DdManager<Type>> const & manager, uint_fast64_t numberOfCommands, std::vector<storm::dd::Dd<Type>> const& commandDds, std::vector<storm::dd::Dd<Type>> const & guardDds){
//            
//            // Module DD
//            ModuleDecisionDiagram<Type> moduleDd;
//            
//            // Initialize DDs
//            storm::dd::Dd<Type> guardRangeDd = manager->getZero();
//            storm::dd::Dd<Type> commandsDd = manager->getZero();
//            storm::dd::Dd<Type> temporary = manager->getZero();
//            
//            for (uint_fast64_t i = 0; i < numberOfCommands; ++i) {
//                
//                // Check guard
//                if (guardDds[i].isZero()) continue;
//                
//                // Check for overlapping guards
//                temporary = guardDds[i] * guardRangeDd;
//                if (!(temporary.isZero())) {
//                    LOG4CPLUS_WARN(logger, "Overlapping guard in command " << (i + 1) << ".");
//                }
//                
//                // Sum up all guards
//                guardRangeDd = guardRangeDd + guardDds[i];
//                
//                // Create command DD (multiply guard and updates)
//                temporary = guardDds[i] * commandDds[i];
//                
//                // Sum up all command DDs
//                commandsDd = commandsDd + temporary;
//            }
//            
//            moduleDd.guardDd = guardRangeDd;
//            moduleDd.commandsDd = commandsDd;
//            moduleDd.usedNondetVariables = 0;
//            
//            return moduleDd;
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


