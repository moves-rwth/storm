#include "src/storage/prism/ToJaniConverter.h"

#include "src/storage/expressions/ExpressionManager.h"

#include "src/storage/prism/Program.h"
#include "src/storage/prism/CompositionToJaniVisitor.h"
#include "src/storage/jani/Model.h"

namespace storm {
    namespace prism {
        
        storm::jani::Model convert(storm::prism::Program const& program, bool allVariablesGlobal) {
            std::shared_ptr<storm::expressions::ExpressionManager> manager = program.getManager().getSharedPointer();
            
            // Start by creating an empty JANI model.
            storm::jani::ModelType modelType;
            switch (program.getModelType()) {
                case Program::ModelType::DTMC: modelType = storm::jani::ModelType::DTMC;
                    break;
                case Program::ModelType::CTMC: modelType = storm::jani::ModelType::CTMC;
                    break;
                case Program::ModelType::MDP: modelType = storm::jani::ModelType::MDP;
                    break;
                case Program::ModelType::CTMDP: modelType = storm::jani::ModelType::CTMDP;
                    break;
                case Program::ModelType::MA: modelType = storm::jani::ModelType::MA;
                    break;
                default: modelType = storm::jani::ModelType::UNDEFINED;
            }
            storm::jani::Model janiModel("jani_from_prism", modelType, 1, manager);
            
            // Add all constants of the PRISM program to the JANI model.
            for (auto const& constant : program.getConstants()) {
                janiModel.addConstant(storm::jani::Constant(constant.getName(), constant.getExpressionVariable(), constant.isDefined() ? boost::optional<storm::expressions::Expression>(constant.getExpression()) : boost::none));
            }
            
            // Maintain a mapping from expression variables to JANI variables so we can fill in the correct objects when
            // creating assignments.
            std::map<storm::expressions::Variable, std::reference_wrapper<storm::jani::Variable const>> variableToVariableMap;
            
            // Add all global variables of the PRISM program to the JANI model.
            for (auto const& variable : program.getGlobalIntegerVariables()) {
                if (variable.hasInitialValue()) {
                    storm::jani::BoundedIntegerVariable const& createdVariable = janiModel.addVariable(storm::jani::BoundedIntegerVariable(variable.getName(), variable.getExpressionVariable(), variable.getInitialValueExpression(), false, variable.getLowerBoundExpression(), variable.getUpperBoundExpression()));
                    variableToVariableMap.emplace(variable.getExpressionVariable(), createdVariable);
                } else {
                    storm::jani::BoundedIntegerVariable const& createdVariable = janiModel.addVariable(storm::jani::BoundedIntegerVariable(variable.getName(), variable.getExpressionVariable(), false, variable.getLowerBoundExpression(), variable.getUpperBoundExpression()));
                    variableToVariableMap.emplace(variable.getExpressionVariable(), createdVariable);
                }
            }
            for (auto const& variable : program.getGlobalBooleanVariables()) {
                if (variable.hasInitialValue()) {
                    storm::jani::BooleanVariable const& createdVariable = janiModel.addVariable(storm::jani::BooleanVariable(variable.getName(), variable.getExpressionVariable(), variable.getInitialValueExpression(), false));
                    variableToVariableMap.emplace(variable.getExpressionVariable(), createdVariable);
                } else {
                    storm::jani::BooleanVariable const& createdVariable = janiModel.addVariable(storm::jani::BooleanVariable(variable.getName(), variable.getExpressionVariable(), false));
                    variableToVariableMap.emplace(variable.getExpressionVariable(), createdVariable);
                }
            }
            
            // Add all actions of the PRISM program to the JANI model.
            for (auto const& action : indexToActionMap) {
                // Ignore the empty action as every JANI program has predefined tau action.
                if (!action.second.empty()) {
                    janiModel.addAction(storm::jani::Action(action.second));
                }
            }
            
            // Because of the rules of JANI, we have to make all variables of modules global that are read by other modules.
            
            // Create a mapping from variables to the indices of module indices that write/read the variable.
            std::map<storm::expressions::Variable, std::set<uint_fast64_t>> variablesToAccessingModuleIndices;
            for (uint_fast64_t index = 0; index < program.getNumberOfModules(); ++index) {
                storm::prism::Module const& module = program.getModule(index);
                
                for (auto const& command : module.getCommands()) {
                    std::set<storm::expressions::Variable> variables = command.getGuardExpression().getVariables();
                    for (auto const& variable : variables) {
                        variablesToAccessingModuleIndices[variable].insert(index);
                    }
                    
                    for (auto const& update : command.getUpdates()) {
                        for (auto const& assignment : update.getAssignments()) {
                            variables = assignment.getExpression().getVariables();
                            for (auto const& variable : variables) {
                                variablesToAccessingModuleIndices[variable].insert(index);
                            }
                            variablesToAccessingModuleIndices[assignment.getVariable()].insert(index);
                        }
                    }
                }
            }
            
            // Now create the separate JANI automata from the modules of the PRISM program. While doing so, we use the
            // previously built mapping to make variables global that are read by more than one module.
            for (auto const& module : program.getModules()) {
                storm::jani::Automaton automaton(module.getName());
                for (auto const& variable : module.getIntegerVariables()) {
                    storm::jani::BoundedIntegerVariable newIntegerVariable = *storm::jani::makeBoundedIntegerVariable(variable.getName(), variable.getExpressionVariable(), variable.hasInitialValue() ? boost::make_optional(variable.getInitialValueExpression()) : boost::none, false, variable.getLowerBoundExpression(), variable.getUpperBoundExpression());
                    std::set<uint_fast64_t> const& accessingModuleIndices = variablesToAccessingModuleIndices[variable.getExpressionVariable()];
                    // If there is exactly one module reading and writing the variable, we can make the variable local to this module.
                    if (!allVariablesGlobal && accessingModuleIndices.size() == 1) {
                        storm::jani::BoundedIntegerVariable const& createdVariable = automaton.addVariable(newIntegerVariable);
                        variableToVariableMap.emplace(variable.getExpressionVariable(), createdVariable);
                    } else if (!accessingModuleIndices.empty()) {
                        // Otherwise, we need to make it global.
                        storm::jani::BoundedIntegerVariable const& createdVariable = janiModel.addVariable(newIntegerVariable);
                        variableToVariableMap.emplace(variable.getExpressionVariable(), createdVariable);
                    }
                }
                for (auto const& variable : module.getBooleanVariables()) {
                    storm::jani::BooleanVariable newBooleanVariable = *storm::jani::makeBooleanVariable(variable.getName(), variable.getExpressionVariable(), variable.hasInitialValue() ? boost::make_optional(variable.getInitialValueExpression()) : boost::none, false);
                    std::set<uint_fast64_t> const& accessingModuleIndices = variablesToAccessingModuleIndices[variable.getExpressionVariable()];
                    // If there is exactly one module reading and writing the variable, we can make the variable local to this module.
                    if (!allVariablesGlobal && accessingModuleIndices.size() == 1) {
                        storm::jani::BooleanVariable const& createdVariable = automaton.addVariable(newBooleanVariable);
                        variableToVariableMap.emplace(variable.getExpressionVariable(), createdVariable);
                    } else if (!accessingModuleIndices.empty()) {
                        // Otherwise, we need to make it global.
                        storm::jani::BooleanVariable const& createdVariable = janiModel.addVariable(newBooleanVariable);
                        variableToVariableMap.emplace(variable.getExpressionVariable(), createdVariable);
                    }
                }
                automaton.setInitialStatesRestriction(manager->boolean(true));
                
                // Create a single location that will have all the edges.
                uint64_t onlyLocation = automaton.addLocation(storm::jani::Location("l"));
                automaton.addInitialLocation(onlyLocation);
                
                for (auto const& command : module.getCommands()) {
                    boost::optional<storm::expressions::Expression> rateExpression;
                    std::vector<storm::jani::EdgeDestination> destinations;
                    if (program.getModelType() == Program::ModelType::CTMC || program.getModelType() == Program::ModelType::CTMDP) {
                        for (auto const& update : command.getUpdates()) {
                            if (rateExpression) {
                                rateExpression = rateExpression.get() + update.getLikelihoodExpression();
                            } else {
                                rateExpression = update.getLikelihoodExpression();
                            }
                        }
                    }
                    
                    for (auto const& update : command.getUpdates()) {
                        std::vector<storm::jani::Assignment> assignments;
                        for (auto const& assignment : update.getAssignments()) {
                            assignments.push_back(storm::jani::Assignment(variableToVariableMap.at(assignment.getVariable()).get(), assignment.getExpression()));
                        }
                        
                        if (rateExpression) {
                            destinations.push_back(storm::jani::EdgeDestination(onlyLocation, manager->integer(1) / rateExpression.get(), assignments));
                        } else {
                            destinations.push_back(storm::jani::EdgeDestination(onlyLocation, update.getLikelihoodExpression(), assignments));
                        }
                    }
                    automaton.addEdge(storm::jani::Edge(onlyLocation, janiModel.getActionIndex(command.getActionName()), rateExpression, command.getGuardExpression(), destinations));
                }
                
                janiModel.addAutomaton(automaton);
            }
            
            // Translate the reward models.
            for (auto const& rewardModel : program.getRewardModels()) {
                
            }
            
            // Create an initial state restriction if there was an initial construct in the program.
            if (program.hasInitialConstruct()) {
                janiModel.setInitialStatesRestriction(program.getInitialConstruct().getInitialStatesExpression());
            } else {
                janiModel.setInitialStatesRestriction(manager->boolean(true));
            }
            
            // Set the standard system composition. This is possible, because we reject non-standard compositions anyway.
            if (program.specifiesSystemComposition()) {
                CompositionToJaniVisitor visitor;
                janiModel.setSystemComposition(visitor.toJani(program.getSystemCompositionConstruct().getSystemComposition(), janiModel));
            } else {
                janiModel.setSystemComposition(janiModel.getStandardSystemComposition());
            }
            
            janiModel.finalize();
            
            return janiModel;
        }
        
    }
}