#include "AutomaticAction.h"
#include "UnfoldAction.h"
#include "EliminateAutomaticallyAction.h"
#include <boost/graph/strong_components.hpp>
#include "storm/storage/expressions/ExpressionManager.h"
#include "UnfoldDependencyGraph.h"

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

                for (const expressions::Variable &variable : variables) {
                    // The "variable" might be a constant, so we first need to ensure we're dealing with a variable:
                    bool isGlobalVar = session.getModel().getGlobalVariables().hasVariable(variable.getName());
                    bool isLocalVar = session.getModel().getAutomaton(autName).getVariables().hasVariable(
                            variable.getName());
                    if (!isGlobalVar && !isLocalVar)
                        continue;

                    // This group contains all variables who are in the same SCC of the dependency graph as our variable
                    auto sccIndex = dependencyGraph.findGroupIndex(variable.getName());

                    // We now need to make sure that the variable is unfoldable (i.e. none of the assignments depend
                    // on constants. Note that dependencies on other variables are fine, as we can simply also unfold
                    // these other variables
                    if (!dependencyGraph.areDependenciesUnfoldable(sccIndex) ||
                        !dependencyGraph.variableGroups[sccIndex].allVariablesUnfoldable)
                        continue;

                    // TODO: areDependenciesUnfoldable(...) also computes the dependencies. A single call would be more efficient.
                    auto orderedDependencies = dependencyGraph.getOrderedDependencies(sccIndex, true);

                    uint32_t totalBlowup = dependencyGraph.getTotalBlowup(orderedDependencies);

                    if (totalBlowup < bestBlowup) {
                        bestBlowup = totalBlowup;
                        bestVariableGroupIndex = sccIndex;
                    }
                }

                if (bestBlowup == UINT32_MAX) {
                    STORM_LOG_WARN("Could not unfold any of the variables occuring in the property");
                    return;
                }

                auto orderedDependencies = dependencyGraph.getOrderedDependencies(bestVariableGroupIndex, true);

                for (auto dependency : orderedDependencies) {
                    auto variables = dependencyGraph.variableGroups[dependency].variables;
                    if (variables.size() != 1) {
                        STORM_LOG_WARN("Unfolding variables with circular dependencies is currently unsupported");
                    }
                    for (auto variable : variables) {
                        if (variable.isGlobal) {
                            // We currently always have to specify an automaton name, regardless of whether the
                            // variable is global or not. This isn't  really a problem, as there is just one automaton
                            // due to the flattening done previously (and the name of that is stored in autName)
                            std::cout << "Unfolding global variable " << variable.janiVariableName << std::endl;
                            UnfoldAction unfoldAction(autName, variable.janiVariableName);
                            unfoldAction.doAction(session);
                        } else {
                            std::cout << "Unfolding variable " << variable.janiVariableName << " (automaton: "
                                      << variable.automatonName << ")" << std::endl;
                            UnfoldAction unfoldAction(variable.automatonName, variable.janiVariableName);
                            unfoldAction.doAction(session);
                        }
                    }
                }

                EliminateAutomaticallyAction eliminateAction(autName,
                                                             EliminateAutomaticallyAction::EliminationOrder::NewTransitionCount,
                                                             20000);
                eliminateAction.doAction(session);
            }
        }
    }
}