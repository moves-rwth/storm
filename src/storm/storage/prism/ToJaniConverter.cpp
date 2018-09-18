#include "storm/storage/prism/ToJaniConverter.h"

#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/storage/prism/Program.h"
#include "storm/storage/prism/CompositionToJaniVisitor.h"
#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Property.h"
#include "storm/storage/jani/TemplateEdge.h"
#include "storm/storage/jani/expressions/FunctionCallExpression.h"

#include "storm/settings/SettingsManager.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotImplementedException.h"

namespace storm {
    namespace prism {
        
        storm::jani::Model ToJaniConverter::convert(storm::prism::Program const& program, bool allVariablesGlobal, std::string suffix, bool standardCompliant) {
            labelRenaming.clear();
            rewardModelRenaming.clear();
            
            std::shared_ptr<storm::expressions::ExpressionManager> manager = program.getManager().getSharedPointer();
            
            bool produceStateRewards = !standardCompliant || program.getModelType() == storm::prism::Program::ModelType::CTMC || program.getModelType() == storm::prism::Program::ModelType::MA;
                        
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
            
            janiModel.getModelFeatures().add(storm::jani::ModelFeature::DerivedOperators);
            
            // Add all constants of the PRISM program to the JANI model.
            for (auto const& constant : program.getConstants()) {
                janiModel.addConstant(storm::jani::Constant(constant.getName(), constant.getExpressionVariable(), constant.isDefined() ? boost::optional<storm::expressions::Expression>(constant.getExpression()) : boost::none));
            }
            
            // Add all formulas of the PRISM program to the JANI model.
            // Also collect a set of variables that occurr in a formula expressions as these need to be made global.
            std::set<storm::expressions::Variable> formulaExpressionVariables;
            for (auto const& formula : program.getFormulas()) {
                storm::jani::FunctionDefinition funDef(formula.getName(), formula.getType(), {}, formula.getExpression());
                janiModel.addFunctionDefinition(funDef);
                formula.getExpression().getBaseExpression().gatherVariables(formulaExpressionVariables);
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
                    storm::jani::BoundedIntegerVariable const& createdVariable = janiModel.addVariable(storm::jani::BoundedIntegerVariable(variable.getName(), variable.getExpressionVariable(), variable.getLowerBoundExpression(), variable.getUpperBoundExpression()));
                    variableToVariableMap.emplace(variable.getExpressionVariable(), createdVariable);
                }
            }
            for (auto const& variable : program.getGlobalBooleanVariables()) {
                if (variable.hasInitialValue()) {
                    storm::jani::BooleanVariable const& createdVariable = janiModel.addVariable(storm::jani::BooleanVariable(variable.getName(), variable.getExpressionVariable(), variable.getInitialValueExpression(), false));
                    variableToVariableMap.emplace(variable.getExpressionVariable(), createdVariable);
                } else {
                    storm::jani::BooleanVariable const& createdVariable = janiModel.addVariable(storm::jani::BooleanVariable(variable.getName(), variable.getExpressionVariable()));
                    variableToVariableMap.emplace(variable.getExpressionVariable(), createdVariable);
                }
            }
            
            // Add all actions of the PRISM program to the JANI model.
            for (auto const& action : program.getActions()) {
                // Ignore the empty action as every JANI program has predefined tau action.
                if (!action.empty()) {
                    janiModel.addAction(storm::jani::Action(action));
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
            
            // Create a mapping from variables to a flag indicating whether it should be made global
            std::map<storm::expressions::Variable, bool> variablesToMakeGlobal;
            for (auto const& varMods : variablesToAccessingModuleIndices) {
                assert(!varMods.second.empty());
                // If there is exactly one module reading and writing the variable, we can make the variable local to this module.
                variablesToMakeGlobal[varMods.first] = allVariablesGlobal || (varMods.second.size() > 1) || formulaExpressionVariables.count(varMods.first) > 0;
            }
            
            // Go through the labels and construct assignments to transient variables that are added to the locations.
            std::vector<storm::jani::Assignment> transientLocationAssignments;
            for (auto const& label : program.getLabels()) {
                bool renameLabel = manager->hasVariable(label.getName()) || program.hasRewardModel(label.getName());
                std::string finalLabelName = renameLabel ? "label_" + label.getName() + suffix : label.getName();
                if (renameLabel) {
                    STORM_LOG_INFO("Label '" << label.getName() << "' was renamed to '" << finalLabelName << "' in PRISM-to-JANI conversion, as another variable with that name already exists.");
                    labelRenaming[label.getName()] = finalLabelName;
                }
                auto newExpressionVariable = manager->declareBooleanVariable(finalLabelName);
                storm::jani::BooleanVariable const& newTransientVariable = janiModel.addVariable(storm::jani::BooleanVariable(newExpressionVariable.getName(), newExpressionVariable, manager->boolean(false), true));
                transientLocationAssignments.emplace_back(storm::jani::LValue(newTransientVariable), label.getStatePredicateExpression());

                // Variables that are accessed in the label predicate expression should be made global.
                std::set<storm::expressions::Variable> variables = label.getStatePredicateExpression().getVariables();
                for (auto const& variable : variables) {
                    variablesToMakeGlobal[variable] = true;
                }
            }
            
            // Create an initial state restriction if there was an initial construct in the program.
            if (program.hasInitialConstruct()) {
                janiModel.setInitialStatesRestriction(program.getInitialConstruct().getInitialStatesExpression());
                // Variables in the initial state expression should be made global
                std::set<storm::expressions::Variable> variables = program.getInitialConstruct().getInitialStatesExpression().getVariables();
                for (auto const& variable : variables) {
                    variablesToMakeGlobal[variable] = true;
                }
            } else {
                janiModel.setInitialStatesRestriction(manager->boolean(true));
            }
            
            // Go through the reward models and construct assignments to the transient variables that are to be added to
            // edges and transient assignments that are added to the locations.
            std::map<uint_fast64_t, std::vector<storm::jani::Assignment>> transientEdgeAssignments;
            for (auto const& rewardModel : program.getRewardModels()) {
                std::string finalRewardModelName;
                if (rewardModel.getName().empty()) {
                    finalRewardModelName = "default_reward_model";
                } else {
                    if (manager->hasVariable(rewardModel.getName())) {
                        // Rename
                        finalRewardModelName = "rewardmodel_" + rewardModel.getName() + suffix;
                        STORM_LOG_INFO("Rewardmodel '" << rewardModel.getName() << "' was renamed to '" << finalRewardModelName << "' in PRISM-to-JANI conversion, as another variable with that name already exists.");
                        rewardModelRenaming[rewardModel.getName()] = finalRewardModelName;
                    } else {
                        finalRewardModelName = rewardModel.getName();
                    }
                }
                
                auto newExpressionVariable = manager->declareRationalVariable(finalRewardModelName);
                storm::jani::RealVariable const& newTransientVariable = janiModel.addVariable(storm::jani::RealVariable(finalRewardModelName, newExpressionVariable, manager->rational(0.0), true));
                
                if (rewardModel.hasStateRewards()) {
                    storm::expressions::Expression transientLocationExpression;
                    for (auto const& stateReward : rewardModel.getStateRewards()) {
                        storm::expressions::Expression rewardTerm = stateReward.getStatePredicateExpression().isTrue() ? stateReward.getRewardValueExpression() : storm::expressions::ite(stateReward.getStatePredicateExpression(), stateReward.getRewardValueExpression(), manager->rational(0));
                        if (transientLocationExpression.isInitialized()) {
                            transientLocationExpression = transientLocationExpression + rewardTerm;
                        } else {
                            transientLocationExpression = rewardTerm;
                        }
                    }
                    transientLocationAssignments.emplace_back(storm::jani::LValue(newTransientVariable), transientLocationExpression);
                    // Variables that are accessed in a reward term should be made global.
                    std::set<storm::expressions::Variable> variables = transientLocationExpression.getVariables();
                    for (auto const& variable : variables) {
                        variablesToMakeGlobal[variable] = true;
                    }
                }
                
                std::map<uint_fast64_t, storm::expressions::Expression> actionIndexToExpression;
                for (auto const& actionReward : rewardModel.getStateActionRewards()) {
                    storm::expressions::Expression rewardTerm = actionReward.getStatePredicateExpression().isTrue() ? actionReward.getRewardValueExpression() : storm::expressions::ite(actionReward.getStatePredicateExpression(), actionReward.getRewardValueExpression(), manager->rational(0));
                    auto it = actionIndexToExpression.find(janiModel.getActionIndex(actionReward.getActionName()));
                    if (it != actionIndexToExpression.end()) {
                        it->second = it->second + rewardTerm;
                    } else {
                        actionIndexToExpression[janiModel.getActionIndex(actionReward.getActionName())] = rewardTerm;
                    }
                }
                
                for (auto const& entry : actionIndexToExpression) {
                    auto it = transientEdgeAssignments.find(entry.first);
                    if (it != transientEdgeAssignments.end()) {
                        it->second.push_back(storm::jani::Assignment(storm::jani::LValue(newTransientVariable), entry.second));
                    } else {
                        std::vector<storm::jani::Assignment> assignments = {storm::jani::Assignment(storm::jani::LValue(newTransientVariable), entry.second)};
                        transientEdgeAssignments.emplace(entry.first, assignments);
                    }
                    // Variables that are accessed in a reward term should be made global.
                    std::set<storm::expressions::Variable> variables = entry.second.getVariables();
                    for (auto const& variable : variables) {
                        variablesToMakeGlobal[variable] = true;
                    }
                }
                STORM_LOG_THROW(!rewardModel.hasTransitionRewards(), storm::exceptions::NotImplementedException, "Transition reward translation currently not implemented.");
            }
            STORM_LOG_THROW(transientEdgeAssignments.empty() || transientLocationAssignments.empty() || !program.specifiesSystemComposition(), storm::exceptions::NotImplementedException, "Cannot translate reward models from PRISM to JANI that specify a custom system composition.");
            
            // If we are not allowed to produce state rewards, we need to create a mapping from action indices to transient
            // location assignments. This is done so that all assignments are added only *once* for synchronizing actions.
            std::map<uint_fast64_t, std::vector<storm::jani::Assignment>> transientRewardLocationAssignmentsPerAction;
            if (!produceStateRewards) {
                for (auto const& action : program.getActions()) {
                    auto& list = transientRewardLocationAssignmentsPerAction[janiModel.getActionIndex(action)];
                    for (auto const& assignment : transientLocationAssignments) {
                        if (assignment.isTransient() && assignment.getVariable().isRealVariable()) {
                            list.emplace_back(assignment);
                        }
                    }
                }
            }
            
            // Now create the separate JANI automata from the modules of the PRISM program. While doing so, we use the
            // previously built mapping to make variables global that are read by more than one module.
            std::set<uint64_t> firstModules;
            bool firstModule = true;
            for (auto const& module : program.getModules()) {
                // Keep track of the action indices contained in this module.
                std::set<uint_fast64_t> actionIndicesOfModule;

                storm::jani::Automaton automaton(module.getName(), manager->declareIntegerVariable("_loc_prism2jani_" + module.getName() + "_" + suffix));
                for (auto const& variable : module.getIntegerVariables()) {
                    storm::jani::BoundedIntegerVariable newIntegerVariable = *storm::jani::makeBoundedIntegerVariable(variable.getName(), variable.getExpressionVariable(), variable.hasInitialValue() ? boost::make_optional(variable.getInitialValueExpression()) : boost::none, false, variable.getLowerBoundExpression(), variable.getUpperBoundExpression());
                    auto findRes = variablesToMakeGlobal.find(variable.getExpressionVariable());
                    if (findRes != variablesToMakeGlobal.end()) {
                        bool makeVarGlobal = findRes->second;
                        storm::jani::BoundedIntegerVariable const& createdVariable = makeVarGlobal ? janiModel.addVariable(newIntegerVariable) : automaton.addVariable(newIntegerVariable);
                        variableToVariableMap.emplace(variable.getExpressionVariable(), createdVariable);
                    } else {
                        STORM_LOG_INFO("Variable " << variable.getName() << " is declared but never used.");
                    }
                }
                for (auto const& variable : module.getBooleanVariables()) {
                    storm::jani::BooleanVariable newBooleanVariable = *storm::jani::makeBooleanVariable(variable.getName(), variable.getExpressionVariable(), variable.hasInitialValue() ? boost::make_optional(variable.getInitialValueExpression()) : boost::none, false);
                    auto findRes = variablesToMakeGlobal.find(variable.getExpressionVariable());
                    if (findRes != variablesToMakeGlobal.end()) {
                        bool makeVarGlobal = findRes->second;
                        storm::jani::BooleanVariable const& createdVariable = makeVarGlobal ? janiModel.addVariable(newBooleanVariable) : automaton.addVariable(newBooleanVariable);
                        variableToVariableMap.emplace(variable.getExpressionVariable(), createdVariable);
                    } else {
                        STORM_LOG_INFO("Variable " << variable.getName() << " is declared but never used.");
                    }
                }
                automaton.setInitialStatesRestriction(manager->boolean(true));
                
                // Create a single location that will have all the edges.
                uint64_t onlyLocationIndex = automaton.addLocation(storm::jani::Location("l"));
                automaton.addInitialLocation(onlyLocationIndex);
                
                // If we are translating the first module that has the action, we need to add the transient assignments to the location.
                // However, in standard compliant JANI, there are no state rewards
                if (firstModule) {
                    storm::jani::Location& onlyLocation = automaton.getLocation(onlyLocationIndex);
                    for (auto const& assignment : transientLocationAssignments) {
                        if (assignment.getVariable().isBooleanVariable() || produceStateRewards) {
                            onlyLocation.addTransientAssignment(assignment);
                        }
                    }
                }
                
                for (auto const& command : module.getCommands()) {
                    std::shared_ptr<storm::jani::TemplateEdge> templateEdge = std::make_shared<storm::jani::TemplateEdge>(command.getGuardExpression());
                    automaton.registerTemplateEdge(templateEdge);
                    actionIndicesOfModule.insert(janiModel.getActionIndex(command.getActionName()));
                    
                    boost::optional<storm::expressions::Expression> rateExpression;
                    if (program.getModelType() == Program::ModelType::CTMC || program.getModelType() == Program::ModelType::CTMDP || (program.getModelType() == Program::ModelType::MA && command.isMarkovian())) {
                        for (auto const& update : command.getUpdates()) {
                            if (rateExpression) {
                                rateExpression = rateExpression.get() + update.getLikelihoodExpression();
                            } else {
                                rateExpression = update.getLikelihoodExpression();
                            }
                        }
                    }
                                        
                    std::vector<std::pair<uint64_t, storm::expressions::Expression>> destinationLocationsAndProbabilities;
                    for (auto const& update : command.getUpdates()) {
                        std::vector<storm::jani::Assignment> assignments;
                        for (auto const& assignment : update.getAssignments()) {
                            assignments.push_back(storm::jani::Assignment(storm::jani::LValue(variableToVariableMap.at(assignment.getVariable()).get()), assignment.getExpression()));
                        }
                        
                        if (rateExpression) {
                            destinationLocationsAndProbabilities.emplace_back(onlyLocationIndex, update.getLikelihoodExpression() / rateExpression.get());
                        } else {
                            destinationLocationsAndProbabilities.emplace_back(onlyLocationIndex, update.getLikelihoodExpression());
                        }
                        
                        templateEdge->addDestination(storm::jani::TemplateEdgeDestination(assignments));
                    }
                    
                    // Then add the transient assignments for the rewards. Note that we may do this only for the first
                    // module that has this action, so we remove the assignments from the global list of assignments
                    // to add after adding them to the created edge.
                    auto transientEdgeAssignmentsToAdd = transientEdgeAssignments.find(janiModel.getActionIndex(command.getActionName()));
                    if (transientEdgeAssignmentsToAdd != transientEdgeAssignments.end()) {
                        for (auto const& assignment : transientEdgeAssignmentsToAdd->second) {
                            templateEdge->addTransientAssignment(assignment);
                        }
                    }
                    if (!produceStateRewards) {
                        transientEdgeAssignmentsToAdd = transientRewardLocationAssignmentsPerAction.find(janiModel.getActionIndex(command.getActionName()));
                        for (auto const& assignment : transientEdgeAssignmentsToAdd->second) {
                            templateEdge->addTransientAssignment(assignment, true);
                        }
                    }

                    // Create the edge object.
                    storm::jani::Edge newEdge;
                    if (command.getActionName().empty()) {
                        newEdge = storm::jani::Edge(onlyLocationIndex, storm::jani::Model::SILENT_ACTION_INDEX, rateExpression, templateEdge, destinationLocationsAndProbabilities);
                    } else {
                        newEdge = storm::jani::Edge(onlyLocationIndex, janiModel.getActionIndex(command.getActionName()), rateExpression, templateEdge, destinationLocationsAndProbabilities);
                    }
                    
                    // Finally add the constructed edge.
                    automaton.addEdge(newEdge);
                }
                
                // Now remove for all actions of this module the corresponding transient assignments, because we must
                // not deal out this reward multiple times.
                // NOTE: This only works for the standard composition and not for any custom compositions. This case
                // must be checked for earlier.
                for (auto actionIndex : actionIndicesOfModule) {
                    // Do not delete rewards dealt out on non-synchronizing edges.
                    if (actionIndex == janiModel.getActionIndex("")) {
                        continue;
                    }
                    
                    auto it = transientEdgeAssignments.find(actionIndex);
                    if (it != transientEdgeAssignments.end()) {
                        transientEdgeAssignments.erase(it);
                    }
                }
                
                janiModel.addAutomaton(automaton);
                firstModule = false;
            }
            
            // Set the standard system composition. This is possible, because we reject non-standard compositions anyway.
            if (program.specifiesSystemComposition()) {
                CompositionToJaniVisitor visitor;
                janiModel.setSystemComposition(visitor.toJani(program.getSystemCompositionConstruct().getSystemComposition(), janiModel));
            } else {
                janiModel.setSystemComposition(janiModel.getStandardSystemComposition());
            }
            
            // if there are formulas, replace the placeholder variables by actual function calls in all expressions
            if (program.getNumberOfFormulas() > 0) {
                janiModel.getModelFeatures().add(storm::jani::ModelFeature::Functions);
                std::map<storm::expressions::Variable, storm::expressions::Expression> substitution;
                for (auto const& formula : program.getFormulas()) {
                    storm::expressions::Expression functionCallExpr = std::make_shared<storm::expressions::FunctionCallExpression>(*manager, formula.getType(), formula.getName(), std::vector<std::shared_ptr<storm::expressions::BaseExpression const>>())->toExpression();
                    substitution.emplace(formula.getExpressionVariable(), functionCallExpr);
                }
                janiModel.substitute(substitution);
            }
            
            janiModel.finalize();
            
            return janiModel;
        }
        
        bool ToJaniConverter::labelsWereRenamed() const {
            return !labelRenaming.empty();
        }
        
        bool ToJaniConverter::rewardModelsWereRenamed() const {
            return !rewardModelRenaming.empty();
        }
        
        std::map<std::string, std::string> const& ToJaniConverter::getLabelRenaming() const {
            return labelRenaming;
        }
        
        std::map<std::string, std::string> const& ToJaniConverter::getRewardModelRenaming() const {
            return rewardModelRenaming;
        }
        
        storm::jani::Property ToJaniConverter::applyRenaming(storm::jani::Property const& property) const {
            if (rewardModelsWereRenamed()) {
                auto res = property.substituteRewardModelNames(getRewardModelRenaming());
                if (labelsWereRenamed()) {
                    res = res.substituteLabels(getLabelRenaming());
                }
                return res;
            } else {
                if (labelsWereRenamed()) {
                    return property.substituteLabels(getLabelRenaming());
                } else {
                    return property.clone();
                }
            }
        }
        
        std::vector<storm::jani::Property> ToJaniConverter::applyRenaming(std::vector<storm::jani::Property> const& properties) const {
            std::vector<storm::jani::Property> result;
            for (auto const& p : properties) {
                result.push_back(applyRenaming(p));
            }
            return result;
        }
    }
}
