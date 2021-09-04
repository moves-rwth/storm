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
            AutomaticAction::AutomaticAction() : locationLimit(30), newTransitionLimit(20000), maxDomainSize(100),
                                                 flatten(true) {

            }

            AutomaticAction::AutomaticAction(uint64_t locationLimit, uint64_t newTransitionLimit,
                                             uint64_t maxDomainSize, bool flatten) : locationLimit(locationLimit),
                                                                                     newTransitionLimit(
                                                                                             newTransitionLimit),
                                                                                     maxDomainSize(maxDomainSize),
                                                                                     flatten(flatten) {

            }

            std::string AutomaticAction::getDescription() {
                return "AutomaticAction";
            }

            void AutomaticAction::doAction(JaniLocalEliminator::Session &session) {
                if (flatten){
                    if (session.getModel().getAutomata().size() > 1) {
                        session.addToLog("Flattening model");
                        session.flatten_automata();
                    }
                    std::string const &autName = session.getModel().getAutomata()[0].getName();
                    processAutomaton(session, autName);
                }
                else{
                    for (uint64_t i = 0; i < session.getModel().getNumberOfAutomata(); i++){
                        std::string const &autName = session.getModel().getAutomata()[i].getName();
                        session.addToLog("Processing automaton " + autName);
                        processAutomaton(session, autName);
                    }
                }


                session.addToLog("Finished automatic state-space reduction.");
                if (session.getModel().getNumberOfAutomata() == 1) {
                    session.addToLog("Final model size: "
                                     + std::to_string(session.getModel().getAutomaton(0).getNumberOfEdges()) +
                                     " edges, " +
                                     std::to_string(session.getModel().getAutomaton(0).getNumberOfLocations()) +
                                     " locations");
                }
            }

            void AutomaticAction::processAutomaton(JaniLocalEliminator::Session &session, const std::string &autName) {
                bool isOnlyAutomaton = session.getModel().getNumberOfAutomata() == 1;

                session.addToLog("Generating variable dependency graph");
                UnfoldDependencyGraph dependencyGraph(session.getModel());
                session.addToLog(dependencyGraph.toString());

                auto nextUnfold = chooseNextUnfold(session, autName, dependencyGraph, true);
                if (!nextUnfold) {
                    session.addToLog("No property variable can be unfolded.");
                    return;
                }
                unfoldGroupAndDependencies(session, autName, dependencyGraph, nextUnfold.get());

//                if (!unfoldPropertyVariable(session, autName, dependencyGraph)) {
//                    return;
//                }

                session.addToLog("Performing automatic elimination");

                EliminateAutomaticallyAction eliminatePropertyAction(autName,
                                                                     EliminateAutomaticallyAction::EliminationOrder::NewTransitionCount,
                                                                     newTransitionLimit, !isOnlyAutomaton);
                eliminatePropertyAction.doAction(session);

                RebuildWithoutUnreachableAction rebuildAfterPropertyAction;
                rebuildAfterPropertyAction.doAction(session);

                while (session.getModel().getAutomaton(0).getLocations().size() < locationLimit) {
                    auto nextUnfold = chooseNextUnfold(session, autName, dependencyGraph, false);
                    if (!nextUnfold) {
                        break;
                    }

                    unfoldGroupAndDependencies(session, autName, dependencyGraph, nextUnfold.get());

                    RebuildWithoutUnreachableAction rebuildAfterPropertyAction;
                    rebuildAfterPropertyAction.doAction(session);

                    EliminateAutomaticallyAction eliminateAction(autName,
                                                                 EliminateAutomaticallyAction::EliminationOrder::NewTransitionCount,
                                                                 newTransitionLimit, !isOnlyAutomaton);
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
                session.addToLog("Unfolding " + dependencyGraph.variableGroups[groupIndex].getVariablesAsString() +
                                 " and their dependencies");
                for (auto dependency : orderedDependencies) {
                    auto variables = dependencyGraph.variableGroups[dependency].variables;
                    if (variables.size() != 1) {
                        STORM_LOG_WARN("Unfolding variables with circular dependencies is currently unsupported");
                    }
                    for (const auto &variable : variables) {
                        if (variable.isGlobal) {
                            // We currently always have to specify an automaton name, regardless of whether the
                            // variable is global or not. This isn't  really a problem, as there is just one automaton
                            // due to the flattening done previously (and the name of that is stored in autName)
                            session.addToLog("\tUnfolding global variable " + variable.janiVariableName);
                            UnfoldAction unfoldAction(autName, variable.janiVariableName,
                                                      variable.expressionVariableName);
                            unfoldAction.doAction(session);
                        } else {
                            session.addToLog("\tUnfolding variable " + variable.janiVariableName + " (automaton: " +
                                             variable.automatonName + ")");
                            UnfoldAction unfoldAction(variable.automatonName, variable.janiVariableName,
                                                      variable.expressionVariableName);
                            unfoldAction.doAction(session);
                        }
                    }
                    dependencyGraph.markUnfolded(dependency);
                }
            }

            bool
            AutomaticAction::unfoldPropertyVariable(JaniLocalEliminator::Session &session, std::string const &autName,
                                                    UnfoldDependencyGraph &dependencyGraph) {
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
                    if (!isGlobalVar && !isLocalVar) {
                        session.addToLog("\t\tVariable is a constant");
                        continue;
                    }

                    // This group contains all variables who are in the same SCC of the dependency graph as our variable
                    auto sccIndex = dependencyGraph.findGroupIndex(variable.getName());

                    // We now need to make sure that the variable is unfoldable (i.e. none of the assignments depend
                    // on constants. Note that dependencies on other variables are fine, as we can simply also unfold
                    // these other variables
                    if (!dependencyGraph.areDependenciesUnfoldable(sccIndex)) {
                        session.addToLog("\t\tNot all dependencies are unfoldable.");
                        continue;
                    }
                    if (!dependencyGraph.variableGroups[sccIndex].allVariablesUnfoldable) {
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

            std::map<std::string, double>
            AutomaticAction::getAssignmentCountByVariable(JaniLocalEliminator::Session &session,
                                                          std::string const &automatonName) {
                std::map<std::string, double> res;
                auto automaton = session.getModel().getAutomaton(automatonName);
                for (auto edge : automaton.getEdges()) {
                    size_t numDest = edge.getNumberOfDestinations();
                    double factor = 1.0 / numDest;
                    for (const auto &dest : edge.getDestinations()) {
                        for (const auto &asg : dest.getOrderedAssignments()) {
                            auto name = asg.getExpressionVariable().getName();
                            if (res.count(name) == 0) {
                                res[name] = 0;
                            }
                            res[name] += factor;
                        }
                    }

                }
                return res;
            }

            boost::optional<uint32_t>
            AutomaticAction::chooseNextUnfold(JaniLocalEliminator::Session &session, std::string const &automatonName,
                                              UnfoldDependencyGraph &dependencyGraph, bool onlyPropertyVariables) {
                std::map<std::string, double> variableOccurrenceCounts = getAssignmentCountByVariable(session,
                                                                                                      automatonName);

                auto propertyVariables = session.getProperty().getUsedVariablesAndConstants();

                session.addToLog("Choosing next unfold");
                if (onlyPropertyVariables){
                    session.addToLog("\tOnly groups containing a variable from the property will be considered");
                }
                session.addToLog(dependencyGraph.toString());
                std::set<uint32_t> groupsWithoutDependencies = dependencyGraph.getGroupsWithNoDependencies();

                uint32_t bestValue = 0;
                uint32_t bestGroup = 0;

                session.addToLog("\tAnalysing groups without dependencies:");
                for (auto groupIndex : groupsWithoutDependencies) {
                    bool containsPropertyVariable = false;
                    UnfoldDependencyGraph::VariableGroup &group = dependencyGraph.variableGroups[groupIndex];
                    double totalOccurences = 0;
                    for (const auto &var : group.variables) {
                        if (variableOccurrenceCounts.count(var.expressionVariableName) > 0) {
                            totalOccurences += variableOccurrenceCounts[var.expressionVariableName];
                        }
                        for (const auto &propertyVar : propertyVariables){
                            if (propertyVar.getName() == var.expressionVariableName){
                                containsPropertyVariable = true;
                            }
                        }
                    }
                    if (onlyPropertyVariables && !containsPropertyVariable){
                        continue;
                    }

                    session.addToLog("\t\t{" + group.getVariablesAsString() + "}: " + std::to_string(totalOccurences) +
                                     " occurences");
                    if (dependencyGraph.variableGroups[groupIndex].domainSize < maxDomainSize){
                        if (totalOccurences > bestValue) {
                            bestValue = totalOccurences;
                            bestGroup = groupIndex;
                        }
                    } else {
                        session.addToLog("\t\t\tSkipped (domain size too large)");
                    }
                }

                if (bestValue == 0) {
                    session.addToLog("No unfoldable variable occurs in any edges.");
                    return boost::none;
                }


                return bestGroup;
            }
        }
    }
}