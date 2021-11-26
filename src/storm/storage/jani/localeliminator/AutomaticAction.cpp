#include "AutomaticAction.h"
#include "UnfoldAction.h"
#include "EliminateAutomaticallyAction.h"
#include "RebuildWithoutUnreachableAction.h"
#include <boost/graph/strong_components.hpp>
#include "storm/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace jani {
        namespace elimination_actions {
            AutomaticAction::AutomaticAction() : locationLimit(30), newTransitionLimit(20000), maxDomainSize(100), flatten(true) {
            }

            AutomaticAction::AutomaticAction(uint64_t locationLimit, uint64_t newTransitionLimit, uint64_t maxDomainSize, bool flatten) : locationLimit(locationLimit), newTransitionLimit(newTransitionLimit), maxDomainSize(maxDomainSize), flatten(flatten) {
            }

            std::string AutomaticAction::getDescription() {
                return "AutomaticAction";
            }

            void AutomaticAction::doAction(JaniLocalEliminator::Session &session) {
                if (flatten){
                    if (session.getModel().getAutomata().size() > 1) {
                        if (session.isLogEnabled()){
                            session.addToLog("Flattening model");
                        }
                        session.flatten_automata();
                    }
                    std::string const &autName = session.getModel().getAutomata()[0].getName();
                    processAutomaton(session, autName);
                }
                else{
                    for (uint64_t i = 0; i < session.getModel().getNumberOfAutomata(); i++){
                        std::string const &autName = session.getModel().getAutomata()[i].getName();
                        if (session.isLogEnabled()){
                            session.addToLog("Processing automaton " + autName);
                        }
                        processAutomaton(session, autName);
                    }
                }

                if (session.isLogEnabled()){
                    session.addToLog("Finished automatic state-space reduction.");
                    if (session.getModel().getNumberOfAutomata() == 1) {
                        session.addToLog("Final model size: "
                                         + std::to_string(session.getModel().getAutomaton(0).getNumberOfEdges()) +
                                         " edges, " +
                                         std::to_string(session.getModel().getAutomaton(0).getNumberOfLocations()) +
                                         " locations");
                    }
                }
            }

            void AutomaticAction::processAutomaton(JaniLocalEliminator::Session &session, const std::string &autName) {
                bool isOnlyAutomaton = session.getModel().getNumberOfAutomata() == 1;
                if (session.isLogEnabled()){
                    session.addToLog("Generating variable dependency graph");
                }
                UnfoldDependencyGraph dependencyGraph(session.getModel());
                if (session.isLogEnabled()){
                    session.addToLog(dependencyGraph.toString());
                }

                auto nextUnfold = chooseNextUnfold(session, autName, dependencyGraph, true);
                if (!nextUnfold) {
                    if (session.isLogEnabled()){
                        session.addToLog("No property variable can be unfolded.");
                    }
                    return;
                }
                unfoldGroupAndDependencies(session, autName, dependencyGraph, nextUnfold.get());
                if (session.isLogEnabled()){
                    session.addToLog("Performing automatic elimination");
                }

                EliminateAutomaticallyAction eliminatePropertyAction(autName,
                                                                     EliminateAutomaticallyAction::EliminationOrder::NewTransitionCount,
                                                                     newTransitionLimit, !isOnlyAutomaton);
                eliminatePropertyAction.doAction(session);

                RebuildWithoutUnreachableAction rebuildAfterPropertyAction;
                rebuildAfterPropertyAction.doAction(session);

                while (session.getModel().getAutomaton(0).getLocations().size() < locationLimit) {
                    nextUnfold = chooseNextUnfold(session, autName, dependencyGraph, false);
                    if (!nextUnfold) {
                        break;
                    }

                    unfoldGroupAndDependencies(session, autName, dependencyGraph, nextUnfold.get());

                    RebuildWithoutUnreachableAction rebuildAfterUnfoldingAction;
                    rebuildAfterUnfoldingAction.doAction(session);

                    EliminateAutomaticallyAction eliminateAction(autName,
                                                                 EliminateAutomaticallyAction::EliminationOrder::NewTransitionCount,
                                                                 newTransitionLimit, !isOnlyAutomaton);
                    eliminateAction.doAction(session);

                    RebuildWithoutUnreachableAction rebuildAfterEliminationAction;
                    rebuildAfterEliminationAction.doAction(session);
                }
            }

            void AutomaticAction::unfoldGroupAndDependencies(JaniLocalEliminator::Session &session,
                                                             const std::string& autName,
                                                             UnfoldDependencyGraph &dependencyGraph,
                                                             uint32_t groupIndex) {

                auto orderedDependencies = dependencyGraph.getOrderedDependencies(groupIndex, true);
                if (session.isLogEnabled()) {
                    session.addToLog("Unfolding " + dependencyGraph.variableGroups[groupIndex].getVariablesAsString() +
                                     " and their dependencies");
                }
                for (auto dependency : orderedDependencies) {
                    auto variables = dependencyGraph.variableGroups[dependency].variables;
                    if (variables.size() != 1) {
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Unfolding variables with circular dependencies is currently implemented");
                    }
                    for (const auto &variable : variables) {
                        if (variable.isGlobal) {
                            // We currently always have to specify an automaton name, regardless of whether the
                            // variable is global or not. This isn't  really a problem, as there is just one automaton
                            // due to the flattening done previously (and the name of that is stored in autName)
                            if (session.isLogEnabled()) {
                                session.addToLog("\tUnfolding global variable " + variable.janiVariableName);
                            }
                            UnfoldAction unfoldAction(autName, variable.janiVariableName,
                                                      variable.expressionVariableName);
                            unfoldAction.doAction(session);
                        } else {
                            if (session.isLogEnabled()) {
                                session.addToLog("\tUnfolding variable " + variable.janiVariableName + " (automaton: " +
                                                 variable.automatonName + ")");
                            }
                            UnfoldAction unfoldAction(variable.automatonName, variable.janiVariableName,
                                                      variable.expressionVariableName);
                            unfoldAction.doAction(session);
                        }
                    }
                    dependencyGraph.markUnfolded(dependency);
                }
            }

            std::map<std::string, double> AutomaticAction::getAssignmentCountByVariable(JaniLocalEliminator::Session &session,
                                                          std::string const &automatonName) {
                std::map<std::string, double> res;
                auto automaton = session.getModel().getAutomaton(automatonName);
                for (auto edge : automaton.getEdges()) {
                    size_t numDest = edge.getNumberOfDestinations();

                    // The factor is used to ensure all edges contribute equally. Otherwise, a single edge with hundreds of destinations may skew the scores
                    // significantly and lead to a variable being unfolded that isn't used throughout the model (thus creating few eliminable locations)
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
                if (session.isLogEnabled()) {
                    session.addToLog("Choosing next unfold");
                    if (onlyPropertyVariables){
                        session.addToLog("\tOnly groups containing a variable from the property will be considered");
                    }
                    session.addToLog(dependencyGraph.toString());
                }
                std::set<uint32_t> groupsWithoutDependencies = dependencyGraph.getGroupsWithNoDependencies();

                uint32_t bestValue = 0;
                uint32_t bestGroup = 0;
                if (session.isLogEnabled()) {
                    session.addToLog("\tAnalysing groups without dependencies:");
                }
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
                    if (session.isLogEnabled()) {
                        session.addToLog("\t\t{" + group.getVariablesAsString() + "}: " + std::to_string(totalOccurences) +
                                         " occurrences");
                    }
                    if (dependencyGraph.variableGroups[groupIndex].domainSize < maxDomainSize){
                        if (totalOccurences > bestValue) {
                            bestValue = totalOccurences;
                            bestGroup = groupIndex;
                        }
                    } else if (session.isLogEnabled()) {
                        session.addToLog("\t\t\tSkipped (domain size too large)");
                    }
                }

                if (bestValue == 0) {
                    if (session.isLogEnabled()) {
                        session.addToLog("No unfoldable variable occurs in any edges.");
                    }
                    return boost::none;
                }

                return bestGroup;
            }
        }
    }
}

