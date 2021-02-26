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
                    session.setModel(session.getModel().flattenComposition());
                }

                UnfoldDependencyGraph dependencyGraph(session.getModel());

                std::string autName = session.getModel().getAutomata()[0].getName();

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
                    return;
                }
                session.addToLog("Best total Blowup: " + std::to_string(bestBlowup));

                unfoldGroupAndDependencies(session, autName, dependencyGraph, bestVariableGroupIndex);

                session.addToLog("Performing automatic elimination");

                EliminateAutomaticallyAction eliminateAction(autName,
                                                             EliminateAutomaticallyAction::EliminationOrder::NewTransitionCount,
                                                             20000);
                eliminateAction.doAction(session);

                RebuildWithoutUnreachableAction rebuildAction;
                rebuildAction.doAction(session);


                while (session.getModel().getAutomaton(0).getLocations().size() < 40    ) {
                    std::set<uint32_t> potentialUnfolds = dependencyGraph.getGroupsWithNoDependencies();

                    if (potentialUnfolds.empty()){
                        session.addToLog("No more unfoldable variables found");
                        break;
                    }

                    session.addToLog("Potential next unfolds:");
                    for (uint32_t unf : potentialUnfolds)
                        session.addToLog("\t" + dependencyGraph.variableGroups[unf].getVariablesAsString());

                    uint32_t chosen = *potentialUnfolds.begin();

                    unfoldGroupAndDependencies(session, autName, dependencyGraph, chosen);


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
                            UnfoldAction unfoldAction(autName, variable.janiVariableName);
                            unfoldAction.doAction(session);
                        } else {
                            session.addToLog("\tUnfolding variable " + variable.janiVariableName + " (automaton: " +
                                             variable.automatonName + ")");
                            UnfoldAction unfoldAction(variable.automatonName, variable.janiVariableName);
                            unfoldAction.doAction(session);
                        }
                    }
                    dependencyGraph.markUnfolded(dependency);
                }
            }
        }
    }
}