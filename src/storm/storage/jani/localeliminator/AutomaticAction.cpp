#include "AutomaticAction.h"
#include "UnfoldAction.h"
#include "EliminateAutomaticallyAction.h"
#include "RebuildWithoutUnreachableAction.h"
#include <boost/graph/strong_components.hpp>
#include "storm/storage/expressions/ExpressionManager.h"

using namespace boost;

namespace storm {
    namespace jani {
        namespace elimination_actions {
            AutomaticAction::AutomaticAction() {

            }

            std::string AutomaticAction::getDescription() {
                return "AutomaticAction";
            }

            void AutomaticAction::doAction(JaniLocalEliminator::Session &session) {
                if (session.getModel().getAutomata().size() > 1) {
                    session.addToLog("Flattening model");
                    session.flatten_automata();
                }

                std::string const& autName = session.getModel().getAutomata()[0].getName();

                session.addToLog("Adding missing guards");
                session.addMissingGuards(autName);

                session.addToLog("Generating variable dependency graph");
                UnfoldDependencyGraph dependencyGraph(session.getModel());
                session.addToLog(dependencyGraph.toString());


                if (!unfoldPropertyVariable(session, autName, dependencyGraph)){
                    return;
                }


                session.addToLog("Performing automatic elimination");

                EliminateAutomaticallyAction eliminatePropertyAction(autName,
                                                                     EliminateAutomaticallyAction::EliminationOrder::NewTransitionCount,
                                                                     20000);
                eliminatePropertyAction.doAction(session);

                RebuildWithoutUnreachableAction rebuildAfterPropertyAction;
                rebuildAfterPropertyAction.doAction(session);

                while (session.getModel().getAutomaton(0).getLocations().size() < 40) {
                    auto nextUnfold = chooseNextUnfold(session, autName, dependencyGraph);
                    if (!nextUnfold){
                        break;
                    }

                    unfoldGroupAndDependencies(session, autName, dependencyGraph, nextUnfold.get());

                    EliminateAutomaticallyAction eliminateAction(autName,
                                                                 EliminateAutomaticallyAction::EliminationOrder::NewTransitionCount,
                                                                 20000);
                    eliminateAction.doAction(session);

                    RebuildWithoutUnreachableAction rebuildAction;
                    rebuildAction.doAction(session);
                }
            }

            void AutomaticAction::unfoldGroupAndDependencies(JaniLocalEliminator::Session &session,
                                                             std::string autName,
                                                             UnfoldDependencyGraph &dependencyGraph,
                                                             uint32_t groupIndex) {

                auto orderedDependencies = dependencyGraph.getOrderedDependencies(groupIndex, true);
                session.addToLog("Unfolding " + dependencyGraph.variableGroups[groupIndex].getVariablesAsString() + " and their dependencies");
                for (auto dependency : orderedDependencies) {
                    auto variables = dependencyGraph.variableGroups[dependency].variables;
                    if (variables.size() != 1) {
                        STORM_LOG_WARN("Unfolding variables with circular dependencies is currently unsupported");
                    }
                    for (const auto& variable : variables) {
                        if (variable.isGlobal) {
                            // We currently always have to specify an automaton name, regardless of whether the
                            // variable is global or not. This isn't  really a problem, as there is just one automaton
                            // due to the flattening done previously (and the name of that is stored in autName)
                            session.addToLog("\tUnfolding global variable " + variable.janiVariableName);
                            UnfoldAction unfoldAction(autName, variable.janiVariableName, variable.expressionVariableName);
                            unfoldAction.doAction(session);
                        } else {
                            session.addToLog("\tUnfolding variable " + variable.janiVariableName + " (automaton: " +
                                             variable.automatonName + ")");
                            UnfoldAction unfoldAction(variable.automatonName, variable.janiVariableName, variable.expressionVariableName);
                            unfoldAction.doAction(session);
                        }
                    }
                    dependencyGraph.markUnfolded(dependency);
                }
            }

            bool AutomaticAction::unfoldPropertyVariable(JaniLocalEliminator::Session &session, std::string const& autName, UnfoldDependencyGraph& dependencyGraph) {
                auto variables = session.getProperty().getUsedVariablesAndConstants();

                uint32_t bestVariableGroupIndex;
                uint32_t bestBlowup = UINT32_MAX;

                session.addToLog("Analysing variables in property");
                for (const expressions::Variable &variable : variables) {
                    session.addToLog("\tAnalysing variable " + variable.getName());
                    // The "variable" might be a constant, so we first need to ensure we're dealing with a variable:
                    bool isGlobalVar = session.getModel().getGlobalVariables().hasVariable(variable.getName());
                    bool isLocalVar = session.getModel().getAutomaton(autName).getVariables().hasVariable(
                            variable.getName());
                    if (!isGlobalVar && !isLocalVar){
                        session.addToLog("\t\tVariable is a constant");
                        continue;
                    }

                    // This group contains all variables who are in the same SCC of the dependency graph as our variable
                    auto sccIndex = dependencyGraph.findGroupIndex(variable.getName());

                    // We now need to make sure that the variable is unfoldable (i.e. none of the assignments depend
                    // on constants. Note that dependencies on other variables are fine, as we can simply also unfold
                    // these other variables
                    if (!dependencyGraph.areDependenciesUnfoldable(sccIndex)){
                        session.addToLog("\t\tNot all dependencies are unfoldable.");
                        continue;
                    }
                    if (!dependencyGraph.variableGroups[sccIndex].allVariablesUnfoldable){
                        session.addToLog("\t\tNot all variables in the SCC are unfoldable");
                        continue;
                    }

                    // TODO: areDependenciesUnfoldable(...) also computes the dependencies. A single call would be more efficient.
                    auto orderedDependencies = dependencyGraph.getOrderedDependencies(sccIndex, true);

                    uint32_t totalBlowup = dependencyGraph.getTotalBlowup(orderedDependencies);
                    session.addToLog("\t\tTotal blowup: " + std::to_string(totalBlowup));

                    if (totalBlowup < bestBlowup) {
                        bestBlowup = totalBlowup;
                        bestVariableGroupIndex = sccIndex;
                    }
                }

                if (bestBlowup == UINT32_MAX) {
                    session.addToLog("Could not unfold any of the variables occurring in the property");
                    return false;
                }

                session.addToLog("Best total Blowup: " + std::to_string(bestBlowup));

                unfoldGroupAndDependencies(session, autName, dependencyGraph, bestVariableGroupIndex);

                return true;
            }

            std::map<std::string, uint32_t> AutomaticAction::getAssignmentCountByVariable(JaniLocalEliminator::Session &session, std::string const& automatonName) {
                std::map<std::string, uint32_t> res;
                auto automaton = session.getModel().getAutomaton(automatonName);
                for (auto edge : automaton.getEdges()) {
                    for (auto dest : edge.getDestinations()){
                        for (auto asg : dest.getOrderedAssignments()){
                            auto name = asg.getExpressionVariable().getName();
                            if (res.count(name) == 0) {
                                res[name] = 0;
                            }
                            res[name] += 1;
                        }
                    }

                }

                session.addToLog("\tNumber of (left-side) variable occurences: ");
                for (auto entry : res){
                    session.addToLog("\t\t" + entry.first + ": " + std::to_string(entry.second));
                }

                return res;
            }

            boost::optional<uint32_t> AutomaticAction::chooseNextUnfold(JaniLocalEliminator::Session &session, std::string const& automatonName, UnfoldDependencyGraph &dependencyGraph) {
                std::map<std::string, uint32_t> variableOccurrenceCounts = getAssignmentCountByVariable(session, automatonName);
                session.addToLog(dependencyGraph.toString());
                std::set<uint32_t> groupsWithoutDependencies = dependencyGraph.getGroupsWithNoDependencies();

                uint32_t bestValue = 0;
                uint32_t bestGroup = 0;

                session.addToLog("\tAnalysing groups without dependencies:");
                for (auto groupIndex : groupsWithoutDependencies){
                    UnfoldDependencyGraph::VariableGroup &group = dependencyGraph.variableGroups[groupIndex];
                    uint32_t totalOccurences = 0;
                    for (auto var : group.variables){
                        if (variableOccurrenceCounts.count(var.expressionVariableName) > 0){
                            totalOccurences += variableOccurrenceCounts[var.expressionVariableName];
                        }
                    }
                    if (totalOccurences > bestValue){
                        bestValue = totalOccurences;
                        bestGroup = groupIndex;
                    }
                    session.addToLog("\t\t{" + group.getVariablesAsString() + "}: " + std::to_string(totalOccurences) + " occurences");
                }

                if (bestValue == 0){
                    session.addToLog("No unfoldable variable occurs in any edges.");
                    return boost::none;
                }


                return bestGroup;
            }
        }
    }
}